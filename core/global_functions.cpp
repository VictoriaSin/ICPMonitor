#include "global_functions.h"
#include "crc8table.h"

#include <QDir>
#include <QProcess>
#include <QDebug>

uint8_t crc8(const char *arr, uint32_t size, uint8_t crc)
{
    if (!arr) {
        return 0x0;
    }

    while (size--){
        crc = Crc8Table[crc ^ *(arr++)];
    }

    return crc;
}

bool moveFile(const QString &currentFilePath, const QString &targetPath)
{
    const QFileInfo fileInfo(currentFilePath);
    const QDir targetDir(targetPath);

    // Если файла для перемещения не существует
    if (!fileInfo.exists()) {
        return false;
    }

    // Если папка, в которую перемещаем отсутствует, создаём
    if (!targetDir.exists()) {
        // Если не удалось создать папку
        if (!targetDir.mkpath(targetDir.path())) {
            return false;
        }
    }

    return QFile::rename(fileInfo.absoluteFilePath(), targetPath + '/' + fileInfo.fileName());
}

QString executeAConsoleCommand(const QString &nameProgram, const QStringList &arguments)
{
    QProcess mProcess;
    mProcess.start(nameProgram, arguments);
    mProcess.waitForFinished();
    QString response(mProcess.readAllStandardOutput());
    QString respError(mProcess.readAllStandardError());
    mProcess.close();
    if (respError != "") return respError;
    return response;
}

bool makeDir(const QString /*&path*/)
{
    //if (path.isEmpty()) {
    //    return false;
    //}

    //if (!QDir(path, "", QDir::NoSort).exists()) {
    //    return QDir().mkdir(path);
    //}
    return true;
}

bool isGoodFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }

    if (!QFile::exists(filePath)) {
        return false;
    }

    return true;
}

QVector<int64_t> parseIntegerNumbers(const QString &value)
{
    QVector<int64_t> numbers;
    QStringList chunks = value.split(QRegExp("\\D+"), QString::SkipEmptyParts);
    //QStringList chunks = value.split(QRegExp("\\D+"), Qt::SkipEmptyParts);
    bool ok;
    int number;

    for (const QString &chunk : chunks) {
        number = chunk.toLongLong(&ok);
        if (ok) {
            numbers.append(number);
        }
    }

    return numbers;
}

int64_t totalRAMBytesLinux()
{
    const auto numbers = parseIntegerNumbers(executeAConsoleCommand(LinuxCommands::AwkCommandName, QStringList() << "/MemTotal:/ { print $2 } " << LinuxPaths::RAMInfoPathLinux));
    if (numbers.size() <= 0) {
        return -1;
    }
    return numbers.at(0);
}

int64_t freeRAMBytesLinux()
{
    const auto numbers = parseIntegerNumbers(executeAConsoleCommand(LinuxCommands::AwkCommandName, QStringList() << "/MemFree:/ { print $2 } " << LinuxPaths::RAMInfoPathLinux));
    if (numbers.size() <= 0) {
        return -1;
    }
    return numbers.at(0);
}
