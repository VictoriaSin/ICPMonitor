#include "datadownloadpage.h"
#include "ui_datadownloadpage.h"
#include "ui_abstractdialogpage.h"
#include "gui/dialogWindows/messagedialog.h"
#include "gui/dialogWindows/blockdevicesselectiondialog.h"
#include "gui/dialogWindows/movedatadialog.h"
#include "gui/gui_funcs.h"
#include "global_enums.h"
#include "controller/monitorcontroller.h"
#include "controller/fileProcessing/filecontroller_events.h"
#include "controller/settings.h"
#include "controller/fileProcessing/export_enums.h"

#include <sys/mount.h>

#include <QButtonGroup>
#include <QPushButton>
#include <QTimer>

DataDownloadPage::DataDownloadPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::DataDownloadPage),
    mScreenGroup(new QButtonGroup(this)),
    mAverageSensorReadingsICPGroup(new QButtonGroup(this)),
    mSensorReadingsGroup(new QButtonGroup(this)),
    mMessageDialog(new MessageDialog(this)),
    mSelectUsbDialog(new BlockDevicesSelectionDialog(this)),
    mSelectConvertionDialog(new SelectionDialog(this)),
    mMoveDataDialog(new MoveDataDialog(this))
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    mScreenGroup->addButton(ui->dontDownloadScreensRadioButton, 0);
    mScreenGroup->addButton(ui->downloadLastScreenRadioButton, 1);
    mScreenGroup->addButton(ui->downloadAllScreensRadioButton, 2);

    mAverageSensorReadingsICPGroup->addButton(ui->dontDownloadAverageICPRadioButton, 0);
    mAverageSensorReadingsICPGroup->addButton(ui->downloadLast24HrsAvreageICPRadioButton, 1);
    mAverageSensorReadingsICPGroup->addButton(ui->downloadAllAverageICPRadioButton, 2);

    mSensorReadingsGroup->addButton(ui->dontDownloadSensorReadingsLabelRadioButton, 0);
    mSensorReadingsGroup->addButton(ui->downloadLastrHourSensorReadingsLabelRadioButton, 1);
    mSensorReadingsGroup->addButton(ui->downloadLast24HrsSensorReadingsLabelRadioButton, 2);

#ifndef PC_BUILD
    // Необходимо для того, чтобы окно всегда стояло по центру
    // иначе при открытии на устройстве оно не успевает установить
    // текст и переместиться в центр
    //mMessageDialog->setMinimumSize(720, 320);
    //mMoveDataDialog->setMinimumSize(1080, 320);
#endif

    // Установка картинки окна
    //setUpperSvgIcon(":/icons/downloadData.svg");

    ui->separatorSVG_1->load(QString(":/icons/separator.svg"));
    ui->separatorSVG_1->setMaximumSize(15, 500);

    ui->separatorSVG_2->load(QString(":/icons/separator.svg"));
    ui->separatorSVG_2->setMaximumSize(15, 500);

    // Заполнение окна конвертации доступными форматами
    mSelectConvertionDialog->addToolButton("", QIcon(":/icons/jpegFormat.svg"), QSize(175, 200), ConversionPictureFormat::JPEG, 0, ConversionPictureFormat::JPEG);
    mSelectConvertionDialog->addToolButton("", QIcon(":/icons/pngFormat.svg"), QSize(175, 200), ConversionPictureFormat::PNG, 0, ConversionPictureFormat::PNG);
    mSelectConvertionDialog->addToolButton("", QIcon(":/icons/ppmFormat.svg"), QSize(175, 200), ConversionPictureFormat::NONE, 0, ConversionPictureFormat::NONE);

    // Настройка нажатия кнопок конвертирования фото
    connect(mSelectConvertionDialog->getAcceptButton(), &QPushButton::clicked, mSelectConvertionDialog, &SelectionDialog::accept);
    connect(mSelectConvertionDialog->getRejectButton(), &QPushButton::clicked, mSelectConvertionDialog, &SelectionDialog::reject);

    // Если пользователь выбрал продолжить после выбора USB
    connect(mSelectUsbDialog->getAcceptButton(), &QPushButton::clicked, mSelectUsbDialog, &SelectionDialog::accept);

    // Если пользователь выбрал отмену при выборе USB
    connect(mSelectUsbDialog->getRejectButton(), &QPushButton::clicked, mSelectUsbDialog, &BlockDevicesSelectionDialog::reject);
}

DataDownloadPage::~DataDownloadPage()
{
    delete ui;
}

void DataDownloadPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Экспорт данных"));
    setBottomInfoLabel(tr("Данные текущих показаний хранятся на устройстве 24 часа.\n"
                          "Данные средних показаний хранятся на устройстве 14 дней.\n"
                          "Скриншоты хранятся на устройстве в кол-ве 50 штук."));

    mSelectConvertionDialog->setInfoText(tr("Выбор формата изображений"));

    mMessageDialog->retranslate();
    mMoveDataDialog->retranslate();
    mSelectConvertionDialog->retranslate();
    mSelectUsbDialog->retranslate();
}

void DataDownloadPage::controllerEventHandler(ControllerEvent event, const QVariantMap &args)
{
    switch (event) {
    case ControllerEvent::ExportProgress: {
        // Передаваемый файл и кол-во передаваемых файлов
        const int transferredFile = args[keyfilecontrollerevent::transferredFiles].toInt();
        const int countFiles = args[keyfilecontrollerevent::countFiles].toInt();

        // Если окно прогресса скрыто
        if (mMoveDataDialog->isHidden()) {
            mMoveDataDialog->setWindowModality(Qt::WindowModality::ApplicationModal);
            mMoveDataDialog->show();
            mMoveDataDialog->setRange(0, countFiles);
        }

        // Устанавливаем значения в окно прогресса
        mMoveDataDialog->setValue(transferredFile);
        mMoveDataDialog->setMovingFileText(tr("Экспортирование %1").arg(args[keyfilecontrollerevent::transferredFileName].toString()));
        mMoveDataDialog->setProgressText(tr("Экспортировано %1 из %2").arg(transferredFile).arg(countFiles));
        mMoveDataDialog->setCenter();
        break;
    }
    case ControllerEvent::ExportError: {
        // Если окно прогресса не скрыто
        if (!mMoveDataDialog->isHidden()) {
            mMoveDataDialog->hide();
        }

        // Устанавливаем сообщение
        mMessageDialog->setTextMessage(args[keyfilecontrollerevent::errorMessage].toString());
        mMessageDialog->exec();

        // Запуск следующего состояния
        runNextState();
        break;
    }
    case ControllerEvent::ExportDone:
    {
        // Если окно прогресса не скрыто
        if (!mMoveDataDialog->isHidden()) {
            mMoveDataDialog->hide();
        }

        // Передаваемый файл, кол-во передаваемых файлов и тип данных
        const int transferredFile = args[keyfilecontrollerevent::transferredFiles].toInt();
        const int countFiles = args[keyfilecontrollerevent::countFiles].toInt();
        const QString dataType = args[keyfilecontrollerevent::dataType].toString();

        mMessageDialog->setTextMessage(transferredFile == countFiles ?
                                           dataType + tr("\nуспешно переданы.") : tr("Экспортируемые данные:\n"
                                                                                    "%1\n"
                                                                                    "Из %2 передано %3").
                                           arg(dataType).
                                           arg(countFiles).
                                           arg(transferredFile));
        mMessageDialog->exec();

        // Запуск следующего состояния
        runNextState();
        break;
    }
    default: break;
    }
}

void DataDownloadPage::exportCurrentSensorReadings()
{
    // Устанавливаем состояние текущего объекта
    mCurrentState = State::exportCSR;

    // Если выбран экспорт текущих показаний датчика
    if (!mSensorReadingsGroup->button(0)->isChecked()) {
        QTimer::singleShot(0, mController, [this]() {
            this->mController->exportLastAbsoluteTimeCurrentSensorReadings(this->mSensorReadingsGroup->checkedId() == 1 ? 1 * 60 * 60 * 1000  : 24 * 60 * 60 * 1000,
                                                                           ExportValues::IndicationTime | ExportValues::Indication |
                                                                           ExportValues::Validity | ExportValues::AlarmState, ExportDataFunc::TimeStamp);
        });
        return;
    }

    // Запуск следующего состояния
    runNextState();
}

void DataDownloadPage::exportAverageSensorReadings()
{
    // Устанавливаем состояние текущего объекта
    mCurrentState = State::exportASR;

    // Если выбран экспорт средних показаний датчика
    if (!mAverageSensorReadingsICPGroup->button(0)->isChecked()) {
        QTimer::singleShot(0, mController, [this]() {
            this->mController->exportLastAbsoluteTimeAverageSensorReadings(this->mAverageSensorReadingsICPGroup->checkedId() == 1 ? 24 * 60 * 60  : 14 * 24 * 60 * 60,
                                                                           ExportValues::IndicationTime | ExportValues::Indication |
                                                                           ExportValues::AlarmState | ExportValues::UserLabel |
                                                                           ExportValues::UserInfoLabel, ExportDataFunc::DateTimeTrained);
        });
        return;
    }

    // Запуск следующего состояния
    runNextState();
}

void DataDownloadPage::exportScreenshots()
{
    // Устанавливаем состояние текущего объекта
    mCurrentState = State::exportScreens;

//    // Если выбран экспорт фотографий
//    if (!mScreenGroup->button(0)->isChecked()) {
//        // Ожидаем выбора пользователем формата конвертации
//        if (mSelectConvertionDialog->exec() == QDialog::Accepted) {
//            // Делаем со скриншотами то, что выбрал пользователь
//            QTimer::singleShot(10, mController, [this](){
//                mController->exportScreens(ConversionPictureFormat(mSelectConvertionDialog->getSelectedId()),
//                                               mScreenGroup->checkedId() == 1 ? 1 : mController->getCountPictureInDir());
//            });
//        } else {
//            // Устанавливаем состояние покоя
//            runState(State::defaultState);
//        }
//        return;
//    }

    // Запуск следующего состояния
    runNextState();
}

bool DataDownloadPage::selectFlashDrive()
{
    // Обновляем доступные флешки в системе
    mUSBMaxPart = getMaxPartOfFlashsInSystem();

    // Заполняем диалоговое окно доступными флешками
//    if (!mSelectUsbDialog->setAvailableFlashDrivesOnDialog(mUSBMaxPart)) {
//        mMessageDialog->setTextMessage(tr("Флеш-накопители не обнаружены!"));
//        mMessageDialog->exec();
//        return false;
//    }

    // Выбираем флеш-накопитель
    if (mUSBMaxPart.size() == 1) {
        //mSelectUsbDialog->selectFirstButton();
    } else {
        // Если пользователь решил отменить выбор флеш-накопителя
        if (mSelectUsbDialog->exec() == QDialog::Rejected) {
            return false;
        }
    }

    return true;
}

void DataDownloadPage::runNextState()
{
    runState(State(mCurrentState + 1));
}

void DataDownloadPage::runState(State state)
{
    switch (state) {
    case State::exportScreens: {
        exportScreenshots();
        break;
    }
    case State::exportASR: {
        exportAverageSensorReadings();
        break;
    }
    case State::exportCSR: {
        exportCurrentSensorReadings();
        break;
    }
    default:
        mCurrentState = State::defaultState;
        // Размонтируем флешку
        umountUSB(Settings::BasePathForMount);
    }
}

void DataDownloadPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);

    connect(mController, &MonitorController::controllerEvent,
            this, &DataDownloadPage::controllerEventHandler);

    retranslate();
}

void DataDownloadPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    mMessageDialog->scaleFont(scaleFactor);
    mSelectUsbDialog->scaleFont(scaleFactor);
    mSelectConvertionDialog->scaleFont(scaleFactor);
    mMoveDataDialog->scaleFont(scaleFactor);

    WFontScaling(ui->screenLabel, scaleFactor);
    WFontScaling(ui->averageICPLabel, scaleFactor);
    WFontScaling(ui->sensorReadingsLabel, scaleFactor);

    WFontScaling(ui->dontDownloadScreensRadioButton, scaleFactor);
    WFontScaling(ui->downloadLastScreenRadioButton, scaleFactor);
    WFontScaling(ui->downloadAllScreensRadioButton, scaleFactor);

    WFontScaling(ui->dontDownloadAverageICPRadioButton, scaleFactor);
    WFontScaling(ui->downloadLast24HrsAvreageICPRadioButton, scaleFactor);
    WFontScaling(ui->downloadAllAverageICPRadioButton, scaleFactor);

    WFontScaling(ui->dontDownloadSensorReadingsLabelRadioButton, scaleFactor);
    WFontScaling(ui->downloadLastrHourSensorReadingsLabelRadioButton, scaleFactor);
    WFontScaling(ui->downloadLast24HrsSensorReadingsLabelRadioButton, scaleFactor);

}

void DataDownloadPage::showEvent(QShowEvent *event)
{
    AbstractDialogPage::showEvent(event);
    mScreenGroup->button(0)->setChecked(true);
    mAverageSensorReadingsICPGroup->button(0)->setChecked(true);
    mSensorReadingsGroup->button(0)->setChecked(true);
}

void DataDownloadPage::done(int exodus)
{
    // Если пользователь решил вернуться на предыдущую страницу
    if (exodus != QDialog::Accepted) {
        emit previousPage();
        return;
    }

    // Если отсутствует контроллер
    if (!mController) {
        return;
    }

    // Если не выбраны данные для загрузки на флешку
    if (mScreenGroup->button(0)->isChecked()
            && mAverageSensorReadingsICPGroup->button(0)->isChecked()
            && mSensorReadingsGroup->button(0)->isChecked())
    {
        mMessageDialog->setTextMessage(tr("Не выбраны данные\nдля загрузки на флешку!"));
        mMessageDialog->exec();
        return;
    }

    // Даём пользователю выбрать флеш-накопитель
    // Если флеш-накопитель не был выбран
    if (!selectFlashDrive()) {
        return;
    }

    // Узнаём выбранную флешку и монтируем её
    int indexFlashDrive = mSelectUsbDialog->getSelectedId();
    if (indexFlashDrive != -1) {
        mountUSB(Settings::BasePathForMount, "/dev/" + mUSBMaxPart[indexFlashDrive].drivePartitionName);

        // Если точка монтирования USB не совпадает с базовой точкой монтирования для программы
        if (Settings::BasePathForMount != mountPointOfFlashDrive(mUSBMaxPart[indexFlashDrive].drivePartitionName)) {
            mMessageDialog->setTextMessage(tr("Попробуйте вставить флеш-накопитель заново.\nДанные не были переданы."));
            mMessageDialog->exec();
            return;
        }
    }

    // Запуск сохранения скриншотов
    runState(State::exportScreens);
}
