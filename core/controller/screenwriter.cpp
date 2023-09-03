#include "screenwriter.h"
#include "settings.h"
#include "global_functions.h"
#include <blockDevices/blockdevice.h>

#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QPixmap>
#include <QDateTime>

ScreenWriter::ScreenWriter(const QString &relativeScreensPath, uint maxScreens, QObject *parent)
    : QObject{parent},
      mRelativePath(relativeScreensPath),
      mMaxScreens(maxScreens)
{
    initFBCatProcess();
}

void ScreenWriter::initFBCatProcess()
{
    if (mProcess) {
        return;
    }

    // Таймер для уничтожения процесса при зависании
    mKillTimer = new QTimer(this);
    mKillTimer->setInterval(KillTimeMs);

    // Настройка процесса для выполнения скриншота
    mProcess = new QProcess(this);

    connect(mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenWriter::writeFinished);
    connect(mProcess, &QProcess::started, this, [this](){
        if (mKillTimer) {
            mKillTimer->start();
        }
    });
    connect(mKillTimer, &QTimer::timeout, this, [this](){
        if (mProcess) {
            qDebug() << "Kill fbcat";
            mProcess->kill();
        }
    });

}

void ScreenWriter::writeScreenFromLinuxFB()
{
    // Если папка для скринов не создана(лась)
    if (!makeScreensDir()) {
        emit screenWrittenFinished(false);
        return;
    }

    // Если скриншот еще делается
    if (mProcess->state() != QProcess::NotRunning) {
        emit screenWrittenFinished(false);
        return;
    }

    // Делаем скрин (асинхронно)
    mProcess->start("bash", QStringList() << "-c" << QString("fbcat > %1").arg(mScreensDir->path() + '/' + genNameOfScreen("ppm")));
}

void ScreenWriter::writeScreenFromWidget(const QPixmap &screen)
{
    // Если папка для скринов не создана(лась)
    if (!makeScreensDir()) {
        emit screenWrittenFinished(false);
        return;
    }

    // Уведомляем об окончании записи
    const QString fileName = mScreensDir->path() + '/' + genNameOfScreen("ppm");
    screen.save(fileName, "PPM");
    emit screenWrittenFinished(QFile::exists(fileName));

    // Удаляем старые скриншоты
    deleteOldScreens();
}

bool ScreenWriter::isReadyWriteScreen() const
{
    return mProcess->state() != QProcess::NotRunning && isReadyScreenDir();
}

void ScreenWriter::setBlockDevice(const std::shared_ptr<BlockDevice> &blockDev)
{
    mBlockDevice = blockDev;
    resetScreensDir();
}

void ScreenWriter::setRelativePath(const QString &relativePath)
{
    mRelativePath = relativePath;
    resetScreensDir();
}

QString ScreenWriter::getRelativePath() const
{
    return mRelativePath;
}

bool ScreenWriter::deleteOldScreens()
{
    if (!isReadyScreenDir()) {
        return false;
    }

    const auto screensInfoList = mScreensDir->entryInfoList();
    for (int i = 0; i < int(screensInfoList.count() - mMaxScreens); ++i) {
        mScreensDir->remove(screensInfoList[i].fileName());
    }

    return true;
}

QString ScreenWriter::genNameOfScreen(const QString &format)
{
    return QString("IMG-%1.%2").arg(QDateTime::currentDateTime().toString("hhmmss-ddMMyyyy")).arg(format);
}

void ScreenWriter::setMaxScreens(uint maxScreens)
{
    mMaxScreens = maxScreens;
    deleteOldScreens();
}

uint ScreenWriter::getMaxScreens() const
{
    return mMaxScreens;
}

bool ScreenWriter::makeScreensDir()
{
    if (!isReadyBlockDev()) {
        return false;
    }

    if (isReadyScreenDir()) {
        return true;
    }

    resetScreensDir();

    const QString screensDirPath = mBlockDevice->getLastMountPoint() + '/' + mRelativePath;
    if (makeDir(screensDirPath)) {
        mScreensDir = new QDir(screensDirPath, "", QDir::Time | QDir::Reversed, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    }

    return isReadyScreenDir();
}

void ScreenWriter::resetScreensDir()
{
    if (mScreensDir) {
        delete mScreensDir;
        mScreensDir = nullptr;
    }
}

bool ScreenWriter::isReadyScreenDir() const
{
    if (!mScreensDir) {
        return false;
    }

    mScreensDir->refresh();
    return mScreensDir->exists();
}

bool ScreenWriter::isReadyBlockDev()
{
    if (!mBlockDevice) {
        return false;
    }

    mBlockDevice->refresh();

    if (!mBlockDevice->isReadyToReadWrite()) {
        return false;
    }
    return true;
}

void ScreenWriter::writeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (mKillTimer) {
        mKillTimer->stop();
    }

    deleteOldScreens();

    if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit) {
        emit screenWrittenFinished(true);
    } else {
        emit screenWrittenFinished(false);
    }
}

QString ScreenWriter::conversionPPMtoAny(const QString &ppmPicturePath, ConversionPictureFormat format)
{
    QFileInfo ppmPicture(ppmPicturePath);
    ppmPicture.refresh();

    // Если файла не существует
    if (!ppmPicture.exists()) {
        return QString();
    }

    QFileInfo newPicFormat;
    QString commandToConvert;

    switch (format) {
    case ConversionPictureFormat::PNG: {
        commandToConvert = "pnmtopng";
        newPicFormat.setFile(ppmPicture.absolutePath() + '/' + ppmPicture.baseName() + ".png");
        break;
    }
    case ConversionPictureFormat::JPEG: {
        commandToConvert = "pnmtojpeg";
        newPicFormat.setFile(ppmPicture.absolutePath() + '/' + ppmPicture.baseName() + ".jpeg");
        break;
    }
    default: {
        return QString();
    }
    }

    qDebug() << "Convert " << ppmPicture.absoluteFilePath() << " to " << newPicFormat.absoluteFilePath();
    executeAConsoleCommand("bash", QStringList() << "-c" << QString("%1 %2 > %3").
                             arg(commandToConvert).arg(ppmPicture.absoluteFilePath()).arg(newPicFormat.absoluteFilePath()));
    newPicFormat.refresh();

    if (newPicFormat.exists()) {
        return newPicFormat.absoluteFilePath();
    }
    return QString();
}

bool ScreenWriter::convertAndMovePicture(const QString &sourcePicturePath, const QString &targetPath, ConversionPictureFormat format)
{
    // Получаем путь к сконвертированному фото
    const QString convertPicture = ScreenWriter::conversionPPMtoAny(sourcePicturePath, format);
    // Если конвертация прошла успешно, перемещаем
    if (!convertPicture.isEmpty()) {
        // Если перемещение сконвертированного файла не успешно, то удаляем его
        if (!moveFile(convertPicture, targetPath)) {
            QFile::remove(convertPicture); // Удаляем сконвертированный файл
        } else {
            return true;
        }
    }
    return false;
}
