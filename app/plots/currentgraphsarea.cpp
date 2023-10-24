#include "currentgraphsarea.h"
#include "ui_currentgraphsarea.h"
#include "ui_abstractgraphareawidget.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "plots/waveformplot.h"
#include "plots/intervalplot.h"
#include "gui/gui_funcs.h"
#include "plots/labelmarkitem.h"
#include "controller/labels/labelmanager.h"
#include "controller/labels/label.h"
#include "unistd.h"
#include "ui_mainpage.h"

#include "math.h"
#include <QTimer>

#include "global_define.h"

WaveFormPlot *mWaveGraph {nullptr};
WaveFormPlot *mComplianceGraph {nullptr};

QVector<LabelMarkItem *> mLabelItemsContainer {nullptr};
MarkItem *mIntervalsContainer[4];
bool isIntervalCreating {false};
uint8_t mIntervalsCount {0};

extern uint16_t dVolume;
extern float Po;
extern float Pk;
double dVConst;

extern uint32_t windowWidth;
//_bufferRecord bufferRecord_1, bufferRecord_2;
//uint8_t currentBufferRecord;


//#define TIME_INTERVAL_FOR_RECORD_IN_FILE (4)
//#define TIME_INTERVAL_FOR_WRITE_ON_GRAPH (40) //40 миллисекунд - 25 раз в секунду



CurrentGraphsArea::CurrentGraphsArea(QWidget *parent) :
    AbstractMultipleGraphsAreasWidget(parent),
    ui(new Ui::CurrentGraphsArea)
{
    ui->setupUi(AbstractGraphAreaWidget::ui->widget);

    //bufferRecord_1.currentPos = 0;
    //bufferRecord_2.currentPos = 0;
    //currentBufferRecord = 1;
    isRecord = false;
    mLabelItemsContainer.clear();
    // Записываем для быстрого доступа
    mWaveGraph = ui->waveGraph;
    mWaveGraph->setType(0);
    AbstractGraphAreaWidget::ui->xRangeGraphToolButton->hide();

    //Записываем для быстрого доступа и скрываем график
    mRecordedGraph = ui->recordedGraph;
    mRecordedGraph->hide();

    mFirstInterval = ui->firstInterval;
    mFirstInterval->hide();

    mSecondInterval = ui->secondInterval;
    mSecondInterval->hide();

    mComplianceGraph = ui->complianceGraph;
    mComplianceGraph->setType(1);
    mComplianceGraph->hide();

    // Добавляем графики в общий контейнер
    mGraphContainer.append(mWaveGraph); // 0
    mGraphContainer.append(mRecordedGraph); // 1
    mGraphContainer.append(mFirstInterval); // 2
    mGraphContainer.append(mSecondInterval); // 3
    mGraphContainer.append(mComplianceGraph); // 4

    // Указываем индекс отображаемого графика
    mCurrentGraphIndex = 0;
    mDisplayedGraph = mGraphContainer[mCurrentGraphIndex]->type();

    // Заполняем контейнер масштабов и инициализируем кнопку смены диапазона по оси X
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(3600, QIcon(":/icons/oneHour_pressed.svg"), QIcon(":/icons/oneHour.svg")));
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(7200, QIcon(":/icons/twoHour_pressed.svg"), QIcon(":/icons/twoHour.svg")));
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(10800, QIcon(":/icons/threeHour_pressed.svg"), QIcon(":/icons/threeHour.svg")));
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(14400, QIcon(":/icons/fourHour_pressed.svg"), QIcon(":/icons/fourHour.svg")));
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(28800, QIcon(":/icons/eightHour_pressed.svg"), QIcon(":/icons/eightHour.svg")));
//    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(43200, QIcon(":/icons/twelveHour_pressed.svg"), QIcon(":/icons/twelveHour.svg")));
//    initializeXRangeToolButton();
    // Изменения диапазона графиков по оси X
//    connect(AbstractGraphAreaWidget::ui->xRangeGraphToolButton, &QToolButton::clicked, this, &CurrentGraphsArea::nextXRange);

    // Изменения диапазона графиков по оси Y
//    connect(AbstractGraphAreaWidget::ui->yRangeGraphToolButton, &QToolButton::clicked, this, &CurrentGraphsArea::nextYRange);



    // Устанавливаем фильтр событий для отслеживания нажатия на график записи
    mRecordedGraph->installEventFilter(this);

    //resetAllLabelItems();

   // AverageIntervalSec = 1;// mICPSettings->getCurrentAverageIntervalSec();
   // maxBuffSizeAvg =(uint) (1000.0 / TIME_INTERVAL_FOR_WRITE_ON_GRAPH * AverageIntervalSec);
   // CurrDataForAverage = new float[maxBuffSizeAvg];

}

CurrentGraphsArea::~CurrentGraphsArea()
{
  //delete [] CurrDataForAverage;
  delete ui;
}


void CurrentGraphsArea::resetGraphOfCurrentValues()
{    
    // Если график текущих значений создан     
    if (mWaveGraph)
    {

        mWaveGraph->resetGraph();
        if (isRecord)
        {
            mWaveGraph->changePenColor(QColor("#ff5000"));
        }
        else
        {
            mWaveGraph->changePenColor(QColor(Qt::yellow));
        }
    }
    float temp = mWaveGraph->xAxis->range().upper;
    if (mRecordedGraph)
    {
        mRecordedGraph->resetGraph();
        mRecordedGraph->setXRange(0, temp);
    }
    // Обнуляем счётчик пришедших точек
    mCounterSensorReadings = 0;
}
void CurrentGraphsArea::goToLabel(bool direction)
{
    //qDebug() << "count" <<mLabelItemsContainer.count();
    uint16_t labelCount = mLabelItemsContainer.count();
    float leftPos = 0, rightPos = mRecordedMaxXRange;//mRecordedGraph->mCurrentMaxXRange;

    direction == previous ? mCurrentLabelIndex = (mCurrentLabelIndex + labelCount - 1) % labelCount //назад
            : mCurrentLabelIndex = (mCurrentLabelIndex + 1) % labelCount; //вперед
qDebug() << "currind" << mCurrentLabelIndex;
    float curLabelPosX =mLabelItemsContainer[mCurrentLabelIndex]->getLabel()->mCurrentPos;

    float tempRangeDiv2 = (mRecordedGraph->xAxis->range().size())/2;
    if (curLabelPosX + tempRangeDiv2 > mRecordedMaxXRange)//mRecordedGraph->mCurrentMaxXRange)
    {
        leftPos = curLabelPosX - (mRecordedMaxXRange/*mRecordedGraph->mCurrentMaxXRange*/ - curLabelPosX);
    }
    else
    if ((curLabelPosX - tempRangeDiv2) < 0)
    {
        rightPos = curLabelPosX + curLabelPosX;
    }
    else
    {
        leftPos = curLabelPosX - tempRangeDiv2;
        rightPos = curLabelPosX + tempRangeDiv2;
    }
    mRecordedGraph->setXRange(leftPos, rightPos);
}


void CurrentGraphsArea::addOrDeleteNewItem(bool state)
{
    mRecordedGraph->setInteraction(QCP::iRangeDrag, true);

    if (state == true)
    {
        //mLabelItemsContainer.back()->replotLine();
        if (isLabelCreating)
        {
            mCoordLabelX = 0;
        }
//        else if (isIntervalCreating)
//        {
//            //mIntervalPos = 0;
//            //mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = 0;
//        }
    }
    else
    {
        if (isLabelCreating)
        {
            qDebug() << "vec count=" << mLabelItemsContainer.count();
//            mRecordedGraph->removeItem(mLabelItemsContainer.back());
            mLabelItemsContainer.back()->deleteLine();
            mLabelItemsContainer.pop_back();
            //mLabelManagerGlobal->mCountLabels --;

            mController->deleteLabel(); // бесполезная убрать
            //mLabelItemsContainer.back()->deleteLater();
            qDebug() << "vec count=" << mLabelItemsContainer.count();
        }
        else if (isIntervalCreating)
        {
            mRecordedGraph->removeItem(mIntervalsContainer[mIntervalsCount]);
            //mIntervalsContainer[mIntervalsCount]->deleteLine();

        }

    }
    isLabelCreating = false;
    isIntervalCreating = false;

}


void CurrentGraphsArea::resetRecordedGraph() //починить
{
//    // Если график записи создан
//    if (mRecordedGraph) {
//        mRecordedGraph->resetGraph();
//        resetAllLabelItems();
//    }

    mTotalTimeOfStoredReadingsMs = 0;
    mPrevTimeOfSensorReadingMs = 0;
    mDateTimeOfFirstData = QDateTime();
    mLastXRange = qMakePair(0, 0);
}

void CurrentGraphsArea::resetAllLabelItems()
{
    // Освобождаем динамическую память от меток
    for (auto &labelItem : qAsConst(mLabelItemsContainer)) {
        mRecordedGraph->removeItem(labelItem);
    }

    // Очищаем контейнер от указателей
    mLabelItemsContainer.clear();

    // Очищаем список элементов оптимизации
    mRecordedGraph->clearOtimizationLabelItems();
}

void CurrentGraphsArea::scaleFont(float scaleFactor)
{
    AbstractMultipleGraphsAreasWidget::scaleFont(scaleFactor);
    mWaveGraph->scaleFont(scaleFactor);
    mRecordedGraph->scaleFont(scaleFactor);
    mFirstInterval->scaleFont(scaleFactor);
    mSecondInterval->scaleFont(scaleFactor);
    // Скейлим шрифт для LabelItems
    mFontForLabelItems = FontScaling(mFontForLabelItems, scaleFactor);

}

void CurrentGraphsArea::installController(MonitorController *controller)
{
    AbstractGraphAreaWidget::installController(controller);

    // Обрабатываем события контроллера
    connect(mController, &MonitorController::controllerEvent, this, &CurrentGraphsArea::controllerEventHandler);

    // Для обновления данных на основном графике
    //connect(mController, &MonitorController::dataReadyForGraph, this, &CurrentGraphsArea::addDataOnWavePlot);


    // Установка диапазона осей и делений для графика текущих значений
    updateIntervalsOnGraphs();
    updateTicksOnGraphs();

    // Установка состояний и значений линий тревоги
    updateAlarmLevelsOnWidgets();
    updateAlarmStatesOnWidgets();
}

bool CurrentGraphsArea::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mRecordedGraph) {
        // Тип события
        const auto &eventType = event->type();

        // Если пользователь взаимодействовал с графиком мышкой, то устанавливаем флаг
        if (eventType == QEvent::Wheel  || //eventType == QEvent::MouseButtonDblClick ||
            eventType == QEvent::Scroll || eventType == QEvent::MouseButtonDblClick ||
            eventType == QEvent::MouseButtonRelease)
        {

            mUserTouchOrMouseEventOnRecordedGraph = true;
        }

        // Если пользователь взаимодействовал с графиком сенсором, то устанавливаем флаг
        if (eventType == QEvent::TouchBegin  || eventType == QEvent::TouchEnd ||
            eventType == QEvent::TouchCancel || eventType == QEvent::TouchUpdate)
        {
                mUserTouchOrMouseEventOnRecordedGraph = true;
        }

        // Для дальнейшей обработки событий
        return false;
    }
    else
    {
        return AbstractGraphAreaWidget::eventFilter(watched, event);
    }
}

void CurrentGraphsArea::retranslate()
{
    AbstractMultipleGraphsAreasWidget::retranslate();
    ui->retranslateUi(this);
    mWaveGraph->retranslate();
    mRecordedGraph->retranslate();
    mFirstInterval->retranslate();
    mSecondInterval->retranslate();
}

void CurrentGraphsArea::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch(event) {
    case ControllerEvent::UpdatedAlarmLevels: {
        updateAlarmLevelsOnWidgets();
        break;
    }
    case ControllerEvent::UpdatedAlarmStates: {
        updateAlarmStatesOnWidgets();
        break;
    }
    case ControllerEvent::GlobalTimeUpdate: {
        //resetGraphOfCurrentValues();
        resetRecordedGraph();
        break;
    }
    case ControllerEvent::LabelCreated: {
        addLabelOnRecordedGraph();
        break;
    }
    case ControllerEvent::SessionStarted: {
        resetGraphOfCurrentValues();
        resetRecordedGraph();
        break;
    }
    case ControllerEvent::UpdateGraphIntervals: {
        updateIntervalsOnGraphs();
        break;
    }
    case ControllerEvent::UpdateGraphTicks: {
        updateTicksOnGraphs();
        break;
    }
    case ControllerEvent::ChangePressureUnits: {
        changePressureUnits();
        break;
    }
        default: break;
    }
}

void CurrentGraphsArea::replotDisplayedGraph()
{
    switch (mDisplayedGraph)
    {
        case AbstractCustomPlot::GraphType::WaveFormGraph:
        {
            replotWaveGraph();
            break;
        }
        case AbstractCustomPlot::GraphType::RecordedGraph:
        {
            replotRecordedGraph();
            break;
        }
        case AbstractCustomPlot::GraphType::IntervalGraph:
        {
            replotIntervalGraph();
            break;
        }
    default: break;
    }
}

void CurrentGraphsArea::replotWaveGraph()
{
    mWaveGraph->replot();
    mComplianceGraph->replot();

    const auto curTime = QDateTime::currentMSecsSinceEpoch();
    mWaveGraph->avgBenchTime += (curTime - mWaveGraph->benchTime);
    mWaveGraph->benchTime = curTime;
    mWaveGraph->benchCount++;
}

void CurrentGraphsArea::replotRecordedGraph()
{
    mRecordedGraph->replot();

    const auto curTime = QDateTime::currentMSecsSinceEpoch();
    mRecordedGraph->avgBenchTime += (curTime - mRecordedGraph->benchTime);
    mRecordedGraph->benchTime = curTime;
    mRecordedGraph->benchCount++;
}

void CurrentGraphsArea::replotIntervalGraph()
{
    if (mCurrentIntervalNum == 1)
    {
        mFirstInterval->replot();
    }
    else if (mCurrentIntervalNum == 2)
    {
        mSecondInterval->replot();
    }
}

void CurrentGraphsArea::updateIntervalsOnGraphs()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }
//#define indexPressureH2O 13.595
    mWaveGraph->setXRange(0, settings->getCurrentReadingsGraphIntervalX());
    mComplianceGraph->setXRange(0, settings->getCurrentReadingsGraphIntervalX());
    if (mICPSettings->getCurrentPressureIndex() == 1)
    {
        mWaveGraph->setYRange(10*indexPressureH2O, settings->getCurrentReadingsGraphIntervalY());
        mComplianceGraph->setYRange(10*indexPressureH2O, settings->getCurrentReadingsGraphIntervalY());
        mRecordedGraph->setYRange(10*indexPressureH2O, settings->getCurrentReadingsGraphIntervalY());
        mFirstInterval->setYRange(10*indexPressureH2O, settings->getCurrentReadingsGraphIntervalY());
        mSecondInterval->setYRange(10*indexPressureH2O, settings->getCurrentReadingsGraphIntervalY());
   }
    else
    {
        mWaveGraph->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
        mComplianceGraph->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
        mWaveGraph->setLowerAlarmLevelLine(settings->getLowLevelAlarm());
        mWaveGraph->setUpperAlarmLevelLine(settings->getHighLevelAlarm());
        mRecordedGraph->setXRange(0, settings->getCurrentReadingsGraphIntervalX());
        mRecordedGraph->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
        mFirstInterval->setXRange(0, settings->getCurrentReadingsGraphIntervalX());//
        mFirstInterval->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
        mSecondInterval->setXRange(0, settings->getCurrentReadingsGraphIntervalX());//
        mSecondInterval->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
    }

    mWaveGraph->resetGraph();
    mComplianceGraph->resetGraph();
    mRecordedGraph->resetGraph();
    mFirstInterval->resetGraph();
    mSecondInterval->resetGraph();
}

void CurrentGraphsArea::updateTicksOnGraphs()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    mWaveGraph->xAxis->ticker()->setTickCount(settings->getCurrentTickCountX());
    mWaveGraph->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY());

    mComplianceGraph->xAxis->ticker()->setTickCount(settings->getCurrentTickCountX());
    mComplianceGraph->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY());

    mRecordedGraph->xAxis->ticker()->setTickCount(settings->getCurrentTickCountX());
    mRecordedGraph->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY());

    mFirstInterval->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY()); // нужны ли деления по оси x
    mSecondInterval->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY()); // нужны ли деления по оси x
}

void CurrentGraphsArea::changePressureUnits()
{
    if (mICPSettings->getCurrentPressureIndex() == 0)
    {
        mRecordedGraph->yAxis ->setLabel(tr("мм рт ст"));
        mWaveGraph->yAxis     ->setLabel(tr("мм рт ст"));
        mFirstInterval->xAxis ->setLabel(tr("мм рт ст"));
        mSecondInterval->xAxis->setLabel(tr("мм рт ст"));
    }
    else
    {
        mRecordedGraph->yAxis ->setLabel(tr("мм вод ст"));
        mWaveGraph->yAxis     ->setLabel(tr("мм вод ст"));
        mFirstInterval->xAxis ->setLabel(tr("мм вод ст"));
        mSecondInterval->xAxis->setLabel(tr("мм вод ст"));
    }
    retranslate();
}




void CurrentGraphsArea::addDataOnRecordedPlot()
{
    mRecordedGraph->addDataOnGraphic();
    //currentBufferRecord == 1 ? writeRawData(&bufferRecord_1) : writeRawData(&bufferRecord_2);
}
void CurrentGraphsArea::updateAlarmLevelsOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    const int LLA = settings->getLowLevelAlarm();
    const int HLA = settings->getHighLevelAlarm();

    mWaveGraph->setLowerAlarmLevelLine(LLA);
    mWaveGraph->setUpperAlarmLevelLine(HLA);
}

void CurrentGraphsArea::updateAlarmStatesOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    const bool LLSA = settings->getLowLevelStateAlarm();
    const bool HLSA = settings->getHighLevelStateAlarm();

    mWaveGraph->enableLowerAlarm(LLSA);
    mWaveGraph->enableUpperAlarm(HLSA);
}


void CurrentGraphsArea::addLabelOnRecordedGraph()
{
    // Если нет графика записанных показания или контроллера
    if (!mRecordedGraph || !mController) {  return; }

    // Менеджер меток

    const LabelManager* const labelManager = mController->getLabelManager();

    // Если нет менеджера меток
    if (!labelManager) {    return;    }
    // Последняя созданная метка
    const std::shared_ptr<Label> label = labelManager->getLastCreatedLabel();

    // Если нет последней созданной метки
    if (!label) {   return;    }

    // Создаём метку
    LabelMarkItem *labelItem = new LabelMarkItem(mRecordedGraph, label, mFontForLabelItems, TopMarginForLabel, LabelOrientation::moVerticalTop);
    labelItem->setPositionMark((float)label->getTimeStartLabelMS() / 1000); // Установка позиции метки

    labelItem->addLine(); // Добавление вертикальной пунктирной линии

    // Добавляем элемент для оптимизации
    mRecordedGraph->addOptimizationLabelItem(labelItem);

    // Добавляем в контейнер меток
    mLabelItemsContainer.append(labelItem);
    qDebug() << mLabelItemsContainer.count();
}

void CurrentGraphsArea::addIntervalOnRecordedGraph()
{
    // Если нет графика записанных показания или контроллера
    if (!mRecordedGraph || !mController) {  return; }

    isIntervalCreating = true;
    float newIntervalPos = 0.0;

    if (mIntervalsCount % 2 == 0)
    {
        newIntervalPos = mRecordedGraph->xAxis->range().lower + mRecordedGraph->xAxis->range().size()*0.1;

    }
    else
    {
        newIntervalPos = mRecordedGraph->xAxis->range().lower + mRecordedGraph->xAxis->range().size()*0.9;
    }

    if (mIntervalsCount != 0)
    {
        float prevIntervalPos = (float)(mIntervalsContainer[mIntervalsCount-1]->mIntervalPos)/1000;

        if ((newIntervalPos >= mRecordedGraph->xAxis->range().lower) && (newIntervalPos < mRecordedGraph->xAxis->range().upper))
        {
            if (newIntervalPos <= prevIntervalPos)
            {
                float offset = mRecordedGraph->xAxis->range().size() * 0.01;
                if ((prevIntervalPos + offset) < mRecordedMaxXRange)//mRecordedGraph->mCurrentMaxXRange)
                {
                    newIntervalPos = prevIntervalPos + offset;
                }
                else
                {
                    newIntervalPos = mRecordedGraph->xAxis->range().upper;
                }
            }
        }
        else
        {
            //isIntervalCreating = false;
            return; // обработать
        }
    }
    if (mIntervalsCount < 2)
    {
        mIntervalsContainer[mIntervalsCount] = new MarkItem(mRecordedGraph, mIntervalsCount, QColor(Qt::magenta), mFontForLabelItems, newIntervalPos);
    }
    else
    {
        mIntervalsContainer[mIntervalsCount] = new MarkItem(mRecordedGraph, mIntervalsCount, QColor(Qt::cyan), mFontForLabelItems, newIntervalPos);
    }
    // Добавляем элемент для оптимизации
    //mRecordedGraph->addOptimizationLabelItem(mIntervalsContainer[mIntervalsCount]);
    mIntervalsCount++;
}

void CurrentGraphsArea::changeXInterval(bool interval)
{
#define calcRangePercent(indexLeft, indexRight)  ((((float)mIntervalsContainer[indexRight]->mIntervalPos/1000) - ((float)mIntervalsContainer[indexLeft]->mIntervalPos/1000))*0.02)

#define setIntervalPosition(indexLeft, indexRight, _OFFSET_LEFT, _OFFSET_RIGHT)\
    mRecordedGraph->setXRange((float)mIntervalsContainer[indexLeft]->mIntervalPos/1000 - _OFFSET_LEFT,\
                             (float)mIntervalsContainer[indexRight]->mIntervalPos/1000 + _OFFSET_RIGHT)

#define calcPosInCoord(index) (float)mIntervalsContainer[index]->mIntervalPos/1000
float leftPos, rightPos;
float tempOffset;
// вынести в отдельную функцию
    if (interval == first)
    {
        tempOffset = calcRangePercent(0, 1);
        if (calcPosInCoord(0) - tempOffset < 0) { leftPos = calcPosInCoord(0); }
        else { leftPos = tempOffset; }

        if (calcPosInCoord(1) + tempOffset > mRecordedMaxXRange)//mRecordedGraph->mCurrentMaxXRange)
        {
            rightPos = mRecordedMaxXRange/*mRecordedGraph->mCurrentMaxXRange*/ - calcPosInCoord(1);
        }
        else {rightPos = tempOffset; }

        setIntervalPosition(0, 1, leftPos, rightPos);
    }
    else if (interval == second)
    {
        tempOffset = calcRangePercent(2, 3);
        if (calcPosInCoord(2) - tempOffset < 0) { leftPos = calcPosInCoord(2); }
        else { leftPos = tempOffset; }

        if (calcPosInCoord(3) + tempOffset > mRecordedMaxXRange)//mRecordedGraph->mCurrentMaxXRange)
        {
            rightPos = mRecordedMaxXRange/*mRecordedGraph->mCurrentMaxXRange*/ - calcPosInCoord(3);
        }
        else {rightPos = tempOffset; }

        setIntervalPosition(2, 3, leftPos, rightPos);
    }
}


void CurrentGraphsArea::colorInterval()
{
    qDebug() << "interval count" << mIntervalsCount;
    //QVector<QPair <float, float>> tempVec;
    QPair <int, int> points;
    if (mIntervalsCount < 3)
    {
        points = mRecordedGraph->addInterval(mIntervalsCount, QColor(Qt::magenta));// проверить
        mCurrentIntervalNum = 1;
        mFirstInterval->setup(points, QColor(Qt::magenta));
        setMarksOnInterval();
    }
    else
    {
        points = mRecordedGraph->addInterval(mIntervalsCount, QColor(Qt::cyan));
        mCurrentIntervalNum = 2;
        mSecondInterval->setup(points, QColor(Qt::cyan));
        setMarksOnInterval();
    }
}

void CurrentGraphsArea::setMarksOnInterval()
{
    QString fileMarksData;
    QStringList listOfMarks;
    mMarksFile.open(QIODevice::ReadOnly);
    if (mMarksFile.isOpen())
    {
        fileMarksData = mMarksFile.readAll();
        mMarksFile.seek(0);
        mMarksFile.close();
    }
    else
    {
        qDebug() << "not open";
    }
    listOfMarks = fileMarksData.split("\n");
    MarkItem *newMark = nullptr;


    for (int i=0; i<listOfMarks.count()-1; i++)
    {
        float currPos = listOfMarks[i].split(": ")[1].toDouble()/1000;
        if (mCurrentIntervalNum == 1)
        {
            if (currPos > mFirstInterval->mMainGraph->data()->at(0)->key && currPos < mFirstInterval->mMainGraph->data()->at(mFirstInterval->iterTemp-1)->key)
            {
                newMark = new MarkItem(mFirstInterval, listOfMarks[i].split(": ")[0], currPos, mFontForLabelItems);
            }
        }
        else
        {
            if (currPos > mSecondInterval->mMainGraph->data()->at(0)->key && currPos < mSecondInterval->mMainGraph->data()->at(mSecondInterval->iterTemp-1)->key)
            {
                newMark = new MarkItem(mSecondInterval, listOfMarks[i].split(": ")[0], currPos, mFontForLabelItems);
            }
        }
    }
    //qDebug() << listOfMarks;
}

void CurrentGraphsArea::nextYRange()
{
//    if (!mController) {
//        return;
//    }

//    const Settings * const settings = mController->settings();
//    if (!settings) {
//        return;
//    }

//    const auto &YRanges = settings->getCurrentReadingsGraphYRanges();
//    if (YRanges.isEmpty()) {
//        return;
//    }

//    const auto &range = YRanges[mCurrentYRangeIndex];

//    // Сброс индекса, если были перемещения по оси Y у какого-либо графика
//    if (!qFuzzyCompare(mWaveGraph->yAxis->range().upper, range.second) ||
//            !qFuzzyCompare(mWaveGraph->yAxis->range().lower, range.first)) {
//        mWaveGraph->yAxis->setRange(range.first, range.second);
//        return;
//    }

//    // Новый диапазон по оси Y
//    ++mCurrentYRangeIndex;
//    mCurrentYRangeIndex %= YRanges.size();
//    const auto &newYRange = YRanges[mCurrentYRangeIndex];

//    // Здесь всем графикам меняем приближение
//    mWaveGraph->setYRange(newYRange.first, newYRange.second);
}

bool CurrentGraphsArea::nextXRange()
{
    // Кол-во доступных диапазонов
    const int countIntervals = mXRangesOfSecondsWithIcons.size();

    // Если в списке нет диапазонов или первое показание еще не приходило
    if (countIntervals == 0 || mDateTimeOfFirstData.isNull()) {
        return false;
    }

    // Преобразуем время предыдущего показания в QDateTime
    //const QDateTime &prevTimeOfSensorReadingMs = QDateTime::fromMSecsSinceEpoch(mPrevTimeOfSensorReadingMs);



    // Получаем объект
    ++mCurrentXRangeIndex;
    mCurrentXRangeIndex %= countIntervals;
    const auto &intervalAndIcon = mXRangesOfSecondsWithIcons[mCurrentXRangeIndex];

    // Устанавливаем картинку кнопке
    AbstractGraphAreaWidget::ui->xRangeGraphToolButton->setIcon(intervalAndIcon.defaultButtonIcon, intervalAndIcon.pressedButtonIcon);



    return true;
}

void CurrentGraphsArea::addDataOnFile()
{
  //READ_SPI();
  //if (isRecord)
  //{
  //  currIndex ++;
  //  qDebug() << "currIndex" << currIndex;
  //  _mSPIData temp;
  //  temp.timeStamp = (uint32_t)(currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE);
  //  temp.data = data;
  //  mRawDataFile.write((char*)&temp, sizeof(_mSPIData));
  //}
}

void CurrentGraphsArea::addDataOnWavePlot()
{
//  READ_SPI();
//  if (isRecord)
//  {
//    currIndex ++;
//    qDebug() << "currIndex" << currIndex;
//    //_mSPIData temp;
//    //temp.timeStamp = (uint32_t)(currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE);
//    //temp.data = data;
//    //mRawDataFile.write((char*)&temp, sizeof(_mSPIData));
//  }
//
//  //plotIndex++;
//  //if (isNeedCalc == 1)
//  //{
//  //  data *= indexPressureH2O;
//  //}
//  //mAverageValue = calcAverage(data);
//  //emit(averageReady(mAverageValue));
//  //mWaveGraph->addDataOnGraphic((unsigned int)(plotIndex * TIME_INTERVAL_FOR_WRITE_ON_GRAPH), data);
}


void CurrentGraphsArea::startWork()
{
  //isNeedCalc        = mICPSettings->getCurrentPressureIndex();
  //currIndex         = -1;
  //plotIndex         = -1;
  //mAverageValue     = 0;
  //firstBuffPointer  = 0;
  //lastBuffPointer   = 0;
  //sum               = 0;
  //cnt               = 0;
  //data              = 0;


  if (mReadSPI == nullptr)
  {
    mReadSPI = new ReadSPI();
  }
  mReadSPI->start();




  //if (mTimerGetDataGraph == nullptr)
  //{
  //  mTimerGetDataGraph = new QTimer(this);
  //  connect(mTimerGetDataGraph, &QTimer::timeout, this, &CurrentGraphsArea::addDataOnWavePlot);
  //}

  //if (mTimerGetDataFile== nullptr)
  //{
  //  mTimerGetDataFile = new QTimer(this);
  //  connect(mTimerGetDataFile, &QTimer::timeout, this, &CurrentGraphsArea::addDataOnFile);
  //}

  //mTimerGetDataFile ->start(TIME_INTERVAL_FOR_RECORD_IN_FILE);
  //mTimerGetDataGraph->start(100);//TIME_INTERVAL_FOR_RECORD_IN_FILE);//TIME_INTERVAL_FOR_WRITE_ON_GRAPH);

  //startPlotting();
}
void CurrentGraphsArea::stopWork()
{
  if (mReadSPI != nullptr)
  {
    mReadSPI->isRunning = false;
    while(mReadSPI->isStopped == false);
    mReadSPI = nullptr;
  }
  isRecord = false;
  //mTimerGetDataGraph->stop();
  //mTimerGetDataFile->stop();
  //stopPlotting();
}

void CurrentGraphsArea::stopWorkDraw()
{
  if (mDrawGraphs != nullptr)
  {
    mDrawGraphs->isRunning = false;
    while(mDrawGraphs->isStopped == false);
    mDrawGraphs = nullptr;
  }
}

//float CurrentGraphsArea::calcAverage(uint16_t data)
//{
//    firstBuffPointer = (++firstBuffPointer) % maxBuffSizeAvg;
//    sum += data;
//    if (cnt < maxBuffSizeAvg)
//    {
//        cnt++;
//    }
//    else
//    {
//        lastBuffPointer = (++lastBuffPointer) % maxBuffSizeAvg;
//        sum -= CurrDataForAverage[lastBuffPointer];
//    }
//    CurrDataForAverage[firstBuffPointer] = data;
//    return sum/cnt;
//}

void CurrentGraphsArea::removeAllGraphs()
{
//    mRecordedGraph->clearGraphs();
//    mWaveGraph->clearGraphs();
//    mFirstInterval->clearGraphs();
//    mSecondInterval->clearGraphs();
    //qDebug() << mRecordedGraph->mIntervalFirst->data().data();
    if (mRecordedGraph->mIntervalFirst != nullptr)
//    {mRecordedGraph->removePlottable(mRecordedGraph->mIntervalFirst);}
    {mRecordedGraph->mIntervalFirst->data().data()->clear();}
    if (mRecordedGraph->mIntervalSecond != nullptr)
//    {mRecordedGraph->removePlottable(mRecordedGraph->mIntervalSecond);}
    {mRecordedGraph->mIntervalSecond->data().data()->clear();}
    if (mRecordedGraph != nullptr)
    {mRecordedGraph->mMainGraph->data().clear();}
    mWaveGraph->mMainGraph->data()->clear();//дописать?
    mWaveGraph->mHistGraph->data().clear();//дописать?
    if (mFirstInterval != nullptr)
    {mFirstInterval->mMainGraph->data().data()->clear();}
    if (mSecondInterval != nullptr)
    {mSecondInterval->mMainGraph->data().data()->clear();}
    for (int8_t i=0; i<mIntervalsCount; i++)
    {
        mRecordedGraph->removeItem(mIntervalsContainer[i]);
    }
    mIntervalsCount = 0;
    for (int8_t i=0; i<mLabelItemsContainer.count(); i++)
    {
        mLabelItemsContainer[i]->deleteLine();
    }
    mLabelManagerGlobal->mCountLabels = 0;
    mLabelItemsContainer.clear();

}

_mSPIData currData;
//uint16_t inputData[36] = {12979, 13722, 15682, 17898, 20345, 21989, 22926, 22695, 22975, 22865,
//                       22963, 23072, 23024, 22549, 22452, 22585, 21965, 21758, 21587, 20942,
//                       20345, 19676, 18896, 18701, 17618, 17082, 16206, 15658, 15231, 14708,
//                       13965, 13868, 13235, 12869, 13113, 14391};
#define CNT_AVG_VALUES 20
void CurrentGraphsArea::getData()
{
    uint32_t time = currData.timeStamp;
    static int currIndexArr = 0;
    uint64_t avg = 0;
    for (uint8_t i=0; i<CNT_AVG_VALUES; i++)
    {
       // avg += inputData[currIndexArr%36+i];//(uint16_t)(30 + 15 * sin(3.14 * (double)(((time+i*2) % 2000) + 1)/500));
        currIndexArr++;
    }
    currData.data = avg/CNT_AVG_VALUES;
}


void CurrentGraphsArea::calcCompliance()
{
    float Acp = (mFirstInterval->averageA + mSecondInterval->averageA)/2;
    Acp *= 13.545;
    float Pcp = (Po + Pk)/2;//(mFirstInterval->averageP + mSecondInterval->averageP)/2;//
    Pcp *= 13.545;

    dVConst = (dVolume * log10((Pcp + Acp)/(Pcp - Acp))) / (log10(Pk/Po));//(mFirstInterval->averageP / mSecondInterval->averageP)
    qDebug() << "Acp" << Acp << "Pcp" << Pcp << "dVConst" << dVConst;
    float param = 1.0;
    mWaveGraph->xAxis->setLabel("");
    mWaveGraph->mLowerAlarmLimit->setVisible(false);
    mWaveGraph->mUpperAlarmLimit->setVisible(false);
    mWaveGraph->mMainGraph->data().data()->clear();
    mWaveGraph->yAxis->setRange(0, 30);

    mComplianceGraph->xAxis->setLabel("");
    mComplianceGraph->mLowerAlarmLimit->setVisible(false);
    mComplianceGraph->mUpperAlarmLimit->setVisible(false);
    mComplianceGraph->mMainGraph->data().data()->clear();
    mComplianceGraph->yAxis->setRange(0, 1);

    if (mDrawGraphs == nullptr)
    {
      mDrawGraphs = new DrawGraphs();
    }
    mDrawGraphs->start();
    mDrawGraphs->isRunning = true;

//    //qint64 startTime = getCurrentTimeStamp_ms();
//    //mWaveGraph->replot();

//    QVector<float> pressure;
//    QVector<float> time;
//    uint32_t N = mWaveGraph->mMainGraph->data()->size();
//    float avgBuff[windowWidth];
//    double avgValue = 0;
//    for (uint i=0; i<N-windowWidth; i++)
//    {
//      avgValue = 0;
//      for (uint32_t j=0; j<windowWidth; j++)
//      {
//        avgValue += mWaveGraph->mMainGraph->data()->at(i+j)->value;
//      }
//      avgValue /= windowWidth;

//      pressure.append(avgValue);
//      time.append(mWaveGraph->mMainGraph->data()->at(i)->key);
//    }

//    N = pressure.size();
//    QVector<double> amplitudeVector;
//    QVector<double> amplitudeVectorX;
//    float mxValue = 0;
//    uint32_t mxIndex = 0;
//    bool isMaxFound = false;
//    for (uint i=0; i<N; i++)
//    {
//      if (mWaveGraph->mMainGraph->data()->at(i)->value >= pressure.at(i))
//      {
//        if (mWaveGraph->mMainGraph->data()->at(i)->value > mxValue)
//        {
//          mxValue = mWaveGraph->mMainGraph->data()->at(i)->value;
//          mxIndex = i;
//        }
//        isMaxFound = true;
//      }
//      else if (isMaxFound)
//      {
//        amplitudeVector.append(mxValue - pressure.at(mxIndex));
//        amplitudeVectorX.append(time.at(mxIndex));
//        isMaxFound = false;
//        mxValue = -1;
//        mxIndex = 0;
//      }
//    }

//    N = amplitudeVector.size();
//    for (uint i=0; i<N; i++)
//    {
//        mComplianceGraph->mMainGraph->addData(amplitudeVectorX.at(i), dVConst/(2*amplitudeVector.at(i)));
//    }
}


//void CurrentGraphsArea::addRawData(_bufferRecord *buffer)
//{
//    //qDebug("1"); // Болт забили !!!!!
//    buffer->field[buffer->currentPos].rawData = (u16)(mController->getLastSensorValue().value *500);
//    buffer->field[buffer->currentPos].timeOffset = uint32_t(mController->getLastSensorValue().timestamp - startTimeStampRecord);
//    buffer->currentPos++;
//    if(buffer->currentPos >= MAX_CNT_RECORD)
//    {
//        //writeRawData(buffer);
//        buffer->currentPos = 0;
//        if (currentBufferRecord == 1) {currentBufferRecord = 2; }
//        else {currentBufferRecord = 1; }
//    }
//}

//void CurrentGraphsArea::writeRawData(_bufferRecord *buffer)
//{
////    mRawDataFile.open(QIODevice::WriteOnly | QIODevice::Append);
////    mRawDataFile.write((char*)buffer->field, buffer->currentPos * sizeof(_recordFields));//проверить кол-во
////    mRawDataFile.close();
////    buffer->currentPos = 0;
//}

