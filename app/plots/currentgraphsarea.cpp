#include "currentgraphsarea.h"
#include "ui_currentgraphsarea.h"
#include "ui_abstractgraphareawidget.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "plots/waveformplot.h"
#include "gui/gui_funcs.h"
#include "plots/labelmarkitem.h"
#include "controller/labels/labelmanager.h"
#include "controller/labels/label.h"
#include "unistd.h"
#include "ui_mainpage.h"

#include <QTimer>
QVector<LabelMarkItem *> mLabelItemsContainer {nullptr};
MarkItem *mIntervalsContainer[4];
bool isIntervalCreating {false};
uint8_t mIntervalsCount {0};

_bufferRecord bufferRecord_1, bufferRecord_2;
uint8_t currentBufferRecord;


CurrentGraphsArea::CurrentGraphsArea(QWidget *parent) :
    AbstractMultipleGraphsAreasWidget(parent),
    ui(new Ui::CurrentGraphsArea)
{
    ui->setupUi(AbstractGraphAreaWidget::ui->widget);

    bufferRecord_1.currentPos = 0;
    bufferRecord_2.currentPos = 0;
    currentBufferRecord = 1;
    //qDebug() << "start curr area";
    // Записываем для быстрого доступа
    mWaveGraph = ui->waveGraph;
    // Записываем для быстрого доступа и скрываем график и кнопку изменения диапазона X
    //mTrendGraph = ui->trendGraph;
    //mTrendGraph->hide();
    AbstractGraphAreaWidget::ui->xRangeGraphToolButton->hide();

    //Записываем для быстрого доступа и скрываем график
    mRecordedGraph = ui->recordedGraph;
    mRecordedGraph->hide();

    // Добавляем графики в общий контейнер
    mGraphContainer.append(mWaveGraph);
    //mGraphContainer.append(mTrendGraph);
    mGraphContainer.append(mRecordedGraph);

    // Указываем индекс отображаемого графика
    mCurrentGraphIndex = 0;
    mDisplayedGraph = mGraphContainer[mCurrentGraphIndex]->type();

    // Заполняем контейнер масштабов и инициализируем кнопку смены диапазона по оси X
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(3600, QIcon(":/icons/oneHour_pressed.svg"), QIcon(":/icons/oneHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(7200, QIcon(":/icons/twoHour_pressed.svg"), QIcon(":/icons/twoHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(10800, QIcon(":/icons/threeHour_pressed.svg"), QIcon(":/icons/threeHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(14400, QIcon(":/icons/fourHour_pressed.svg"), QIcon(":/icons/fourHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(28800, QIcon(":/icons/eightHour_pressed.svg"), QIcon(":/icons/eightHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(43200, QIcon(":/icons/twelveHour_pressed.svg"), QIcon(":/icons/twelveHour.svg")));
    initializeXRangeToolButton();
    // Изменения диапазона графиков по оси X
    connect(AbstractGraphAreaWidget::ui->xRangeGraphToolButton, &QToolButton::clicked, this, &CurrentGraphsArea::nextXRange);

    // Изменения диапазона графиков по оси Y
    connect(AbstractGraphAreaWidget::ui->yRangeGraphToolButton, &QToolButton::clicked, this, &CurrentGraphsArea::nextYRange);

//    // Устанавливаем фильтр событий для отслеживания нажатия на график тренда
//    mTrendGraph->installEventFilter(this);


    // Устанавливаем фильтр событий для отслеживания нажатия на график записи
    mRecordedGraph->installEventFilter(this);

    resetAllLabelItems();
}

CurrentGraphsArea::~CurrentGraphsArea()
{
    delete ui;
}


void CurrentGraphsArea::resetGraphOfCurrentValues()
{    
    // Если график текущих значений создан     
    if (mWaveGraph)
    {
        mWaveGraph->resetGraph();
        if(QObject::sender()->objectName() == "MainPage")
        {
            mWaveGraph->changePenColor(QColor(Qt::red));
        }
        else
        {
            mWaveGraph->changePenColor(QColor(Qt::yellow));
        }
    }
    double temp = mWaveGraph->xAxis->range().upper;
    if (mRecordedGraph) {
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
    double leftPos = 0, rightPos = mRecordedGraph->mCurrentMaxXRange;

    direction == previous ? mCurrentLabelIndex = (mCurrentLabelIndex + labelCount - 1) % labelCount //назад
            : mCurrentLabelIndex = (mCurrentLabelIndex + 1) % labelCount; //вперед

    double curLabelPosX =mLabelItemsContainer[mCurrentLabelIndex]->getLabel()->mCurrentPos;

    double tempRangeDiv2 = (mRecordedGraph->xAxis->range().size())/2;
    if (curLabelPosX + tempRangeDiv2 > mRecordedGraph->mCurrentMaxXRange)
    {
        leftPos = curLabelPosX - (mRecordedGraph->mCurrentMaxXRange - curLabelPosX);
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

//void CurrentGraphsArea::resetTrendGraph()
//{
////    // Если график тренда создан
////    if (mTrendGraph) {
////        mTrendGraph->resetGraph();
////        resetAllLabelItems();
////    }
//
////    mTotalTimeOfStoredReadingsMs = 0;
////    mPrevTimeOfSensorReadingMs = 0;
////    mDateTimeOfFirstData = QDateTime();
////    mLastXRange = qMakePair(0, 0);
//}

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
        //!!!mTrendGraph->removeItem(labelItem);
        mRecordedGraph->removeItem(labelItem);
    }

    // Очищаем контейнер от указателей
    mLabelItemsContainer.clear();

    // Очищаем список элементов оптимизации
    //!!!mTrendGraph->clearOtimizationLabelItems();
    mRecordedGraph->clearOtimizationLabelItems();
}

void CurrentGraphsArea::scaleFont(float scaleFactor)
{
    AbstractMultipleGraphsAreasWidget::scaleFont(scaleFactor);
    mWaveGraph->scaleFont(scaleFactor);
    //mTrendGraph->scaleFont(scaleFactor);
    mRecordedGraph->scaleFont(scaleFactor);
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

    // Для обновления данных на графике тренда
    //connect(mController, &MonitorController::dataReadyFromAverageICPController, this, &CurrentGraphsArea::addDataOnTrendGraph);

    // Установка диапазона осей и делений для графика текущих значений
    updateIntervalsOnGraphs();
    updateTicksOnGraphs();

    // Установка состояний и значений линий тревоги
    updateAlarmLevelsOnWidgets();
    updateAlarmStatesOnWidgets();
}

bool CurrentGraphsArea::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mRecordedGraph) {  //!!!mTrendGraph){
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
    //mTrendGraph->retranslate();
    mRecordedGraph->retranslate();
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
        resetGraphOfCurrentValues();
        //resetTrendGraph();
        resetRecordedGraph();
        break;
    }
    case ControllerEvent::LabelCreated: {
        //addLabelOnTrendGraph();
        addLabelOnRecordedGraph();
        break;
    }
    case ControllerEvent::SessionStarted: {
        resetGraphOfCurrentValues();
        //resetTrendGraph();
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
        default: break;
    }
}

void CurrentGraphsArea::replotDisplayedGraph()
{
    switch (mDisplayedGraph) {
//    case AbstractCustomPlot::GraphType::TrendGraph: {
//        //mTrendGraph->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
//        break;
//    }
    case AbstractCustomPlot::GraphType::WaveFormGraph: {
        replotWaveGraph();
        break;
    }
    case AbstractCustomPlot::GraphType::RecordedGraph: {
        replotRecordedGraph();
        break;
    }
    default: break;
    }
}



void CurrentGraphsArea::replotWaveGraph()
{
    mWaveGraph->replot();

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

void CurrentGraphsArea::updateIntervalsOnGraphs()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    mWaveGraph->setXRange(0, settings->getCurrentReadingsGraphIntervalX());
    mWaveGraph->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
    mWaveGraph->setLowerAlarmLevelLine(settings->getLowLevelAlarm());
    mWaveGraph->setUpperAlarmLevelLine(settings->getHighLevelAlarm());
    mWaveGraph->resetGraph();

    mRecordedGraph->setXRange(0, settings->getCurrentReadingsGraphIntervalX());
    mRecordedGraph->setYRange(10, settings->getCurrentReadingsGraphIntervalY());
    mRecordedGraph->resetGraph();
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

    mRecordedGraph->xAxis->ticker()->setTickCount(settings->getCurrentTickCountX());
    mRecordedGraph->yAxis->ticker()->setTickCount(settings->getCurrentTickCountY());
}


void CurrentGraphsArea::addDataOnWavePlot(unsigned int currX, unsigned int currY)
{
    //++mCounterSensorReadings;
    //ComplexValue currValue;
    // Если кол-во точек равно кол-ву прореживания
    /*if (mCounterSensorReadings == mThinningSensorReadings)
    {
        currValue = mController->getLastConvertedSensorValue();
        mWaveGraph->addDataOnGraphic(currValue);//(mController->getLastConvertedSensorValue());
        if (isRecord)
        {
            currentBufferRecord == 1 ? addRawData(&bufferRecord_1) : addRawData(&bufferRecord_2);
            mRecordedGraph->saveDataForGraphic(currValue);//mController->getLastConvertedSensorValue());// пока оставляем
        }        
        mCounterSensorReadings = 0;
    }*/
    mWaveGraph->addDataOnGraphic(currX, currY);
    if (isRecord)
    {
        //currentBufferRecord == 1 ? addRawData(&bufferRecord_1) : addRawData(&bufferRecord_2);

        mRecordedGraph->saveDataForGraphic(currX, currY);
    }
}

void CurrentGraphsArea::addRawData(_bufferRecord *buffer)
{    
    //qDebug("1"); // Болт забили !!!!!
    buffer->field[buffer->currentPos].rawData = (u16)(mController->getLastSensorValue().value *500);
    buffer->field[buffer->currentPos].timeOffset = uint32_t(mController->getLastSensorValue().timestamp - startTimeStampRecord);
    buffer->currentPos++;
    if(buffer->currentPos >= MAX_CNT_RECORD)
    {
        //writeRawData(buffer);
        buffer->currentPos = 0;
        if (currentBufferRecord == 1) {currentBufferRecord = 2; }
        else {currentBufferRecord = 1; }
    }
}

void CurrentGraphsArea::writeRawData(_bufferRecord *buffer)
{
    mRawDataFile.open(QIODevice::WriteOnly | QIODevice::Append);
    mRawDataFile.write((char*)buffer->field, buffer->currentPos * sizeof(_recordFields));//проверить кол-во    
    mRawDataFile.close();
    buffer->currentPos = 0;
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
    //mTrendGraph->setLowerAlarmLevelLine(LLA);
    //mTrendGraph->setUpperAlarmLevelLine(HLA);
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
    //mTrendGraph->enableLowerAlarm(LLSA);
    //mTrendGraph->enableUpperAlarm(HLSA);
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
    labelItem->setPositionMark((double)label->getTimeStartLabelMS() / 1000); // Установка позиции метки

    labelItem->addLine(); // Добавление вертикальной пунктирной линии

    // Добавляем элемент для оптимизации
    mRecordedGraph->addOptimizationLabelItem(labelItem);

    // Добавляем в контейнер меток
    mLabelItemsContainer.append(labelItem);
}

void CurrentGraphsArea::addIntervalOnRecordedGraph()
{
    // Если нет графика записанных показания или контроллера
    if (!mRecordedGraph || !mController) {  return; }

    isIntervalCreating = true;
    double newIntervalPos = 0.0;

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
        double prevIntervalPos = (double)(mIntervalsContainer[mIntervalsCount-1]->mIntervalPos)/1000;

        if ((newIntervalPos >= mRecordedGraph->xAxis->range().lower) && (newIntervalPos < mRecordedGraph->xAxis->range().upper))
        {
            if (newIntervalPos <= prevIntervalPos)
            {
                double offset = mRecordedGraph->xAxis->range().size() * 0.01;
                if ((prevIntervalPos + offset) < mRecordedGraph->mCurrentMaxXRange)
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
#define calcRangePercent(indexLeft, indexRight)  ((((double)mIntervalsContainer[indexRight]->mIntervalPos/1000) - ((double)mIntervalsContainer[indexLeft]->mIntervalPos/1000))*0.02)

#define setIntervalPosition(indexLeft, indexRight, _OFFSET_LEFT, _OFFSET_RIGHT)\
    mRecordedGraph->setXRange((double)mIntervalsContainer[indexLeft]->mIntervalPos/1000 - _OFFSET_LEFT,\
                             (double)mIntervalsContainer[indexRight]->mIntervalPos/1000 + _OFFSET_RIGHT)

#define calcPosInCoord(index) (double)mIntervalsContainer[index]->mIntervalPos/1000
double leftPos, rightPos;
double tempOffset;
// вынести в отдельную функцию
    if (interval == first)
    {
        tempOffset = calcRangePercent(0, 1);
        if (calcPosInCoord(0) - tempOffset < 0) { leftPos = calcPosInCoord(0); }
        else { leftPos = tempOffset; }

        if (calcPosInCoord(1) + tempOffset > mRecordedGraph->mCurrentMaxXRange) { rightPos = mRecordedGraph->mCurrentMaxXRange - calcPosInCoord(1); }
        else {rightPos = tempOffset; }

        setIntervalPosition(0, 1, leftPos, rightPos);
    }
    else if (interval == second)
    {
        tempOffset = calcRangePercent(2, 3);
        if (calcPosInCoord(2) - tempOffset < 0) { leftPos = calcPosInCoord(2); }
        else { leftPos = tempOffset; }

        if (calcPosInCoord(3) + tempOffset > mRecordedGraph->mCurrentMaxXRange) { rightPos = mRecordedGraph->mCurrentMaxXRange - calcPosInCoord(3); }
        else {rightPos = tempOffset; }

        setIntervalPosition(2, 3, leftPos, rightPos);
    }
}


void CurrentGraphsArea::colorInterval()
{
    qDebug() << "interval count" << mIntervalsCount;
    if (mIntervalsCount < 3)
    {
        mRecordedGraph->addInterval(mIntervalsCount, QColor(Qt::magenta));// проверить
    }
    else
    {
        mRecordedGraph->addInterval(mIntervalsCount, QColor(Qt::cyan));
    }
}

//void CurrentGraphsArea::addDataOnTrendGraph()
//{
//    // Если не установлен контроллер
//    if (!mController) {
//        return;
//    }

//    // Узнаём среднее значение
//    const auto &value = mController->getLastAverageValue();

//    // Если пришло самое первое показание
//    if (mDateTimeOfFirstData.isNull()) {
//        // Сбрасываем информацию о графике
//        resetTrendGraph();

//        // Сохраняем время первого пришедшего значения и
//        // приравниваем предыдущее показание первому значению
//        mPrevTimeOfSensorReadingMs = value.timestamp;
//        mDateTimeOfFirstData = QDateTime::fromMSecsSinceEpoch(mPrevTimeOfSensorReadingMs);

//        // Устанавливаем диапазон по оси X
//        nextXRange();
//    } else {
//        // Суммируем хранимое время
//        mTotalTimeOfStoredReadingsMs += value.timestamp - mPrevTimeOfSensorReadingMs;
//    }

//    // Если переполнился допустимый буфер времени отображения показаний,
//    // то стираем старые значения в размере mDeleteIntervalMs
//    if (mTotalTimeOfStoredReadingsMs > mMaxStoredTimeMs) {
//        mTrendGraph->data()->removeBefore(mTrendGraph->data()->begin()->sortKey() + mDeleteIntervalMs / 1000);
//        mTotalTimeOfStoredReadingsMs -= mDeleteIntervalMs;
//    }

//    // Время показания в секундах
//    const auto timestampInSeconds = value.timestamp / 1000;

//    // Добавляем в контейнер данные
//    mTrendGraph->data()->add(QCPGraphData(timestampInSeconds, value.value));

//    // Сохраняем время пришедшего значения
//    mPrevTimeOfSensorReadingMs = value.timestamp;

//    // Если отображение графика переполнилось относительно последнего выставленного диапазона
//    // и пользователь не касался экрана, то сдвигаем его в центр
//    const auto &currentXRange = mTrendGraph->xAxis->range();
//    if (currentXRange.upper < timestampInSeconds && !mUserTouchOrMouseEventOnTrendGraph) {
//        const auto &shiftSeconds = mXRangesOfSecondsWithIcons[mCurrentXRangeIndex].interval * mXAxisShiftInPercent;
//        mLastXRange = mTrendGraph->setXRange(currentXRange.lower + shiftSeconds, timestampInSeconds + shiftSeconds);
//    }
//}


//QPair<double, double> CurrentGraphsArea::setTrendGraphToTheCenter(int intervalSeconds, const QDateTime &timeOfLastReading)
//{
//    // Сбрасываем флаг нажатия на экран
//    mUserTouchOrMouseEventOnTrendGraph = false;

//    // Меняем диапазон
//    const int halfInterval = intervalSeconds / 2;

//    // Возвращаем установленный диапазон
//    return mTrendGraph->setXRange(QCPAxisTickerDateTime::dateTimeToKey(timeOfLastReading.addSecs(-1 * halfInterval)),
//                                  QCPAxisTickerDateTime::dateTimeToKey(timeOfLastReading.addSecs(halfInterval)));
//}

//void CurrentGraphsArea::addLabelOnTrendGraph()
//{
//    // Если нет графика тренда или контроллера
//    if (!mTrendGraph || !mController) {
//        return;
//    }

//    // Менеджер меток
//    const LabelManager* const labelManager = mController->getLabelManager();

//    // Если нет менеджера меток
//    if (!labelManager) {
//        return;
//    }

//    // Последняя созданная метка
//    const std::shared_ptr<Label> label = labelManager->getLastCreatedLabel();

//    // Если нет последней созданной метки
//    if (!label) {
//        return;
//    }

//    // Создаём метку
//    LabelMarkItem *labelItem = new LabelMarkItem(mTrendGraph, label, mFontForLabelItems,
//                                                 TopMarginForLabel, LabelOrientation::moVerticalTop);
//    labelItem->setPositionMark(label->getTimeStartLabelMS() / 1000); // Установка позиции метки
//    labelItem->addLine(); // Добавление вертикальной пунктирной линии

//    // Добавляем элемент для оптимизации
//    mTrendGraph->addOptimizationLabelItem(labelItem);

//    // Добавляем в контейнер меток
//    mLabelItemsContainer.append(labelItem);
//}

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
//            !qFuzzyCompare(mWaveGraph->yAxis->range().lower, range.first) ||
//            !qFuzzyCompare(mTrendGraph->yAxis->range().upper, range.second)||
//            !qFuzzyCompare(mTrendGraph->yAxis->range().lower, range.first)) {
//        mWaveGraph->yAxis->setRange(range.first, range.second);
//        mTrendGraph->yAxis->setRange(range.first, range.second);
//        return;
//    }

//    // Новый диапазон по оси Y
//    ++mCurrentYRangeIndex;
//    mCurrentYRangeIndex %= YRanges.size();
//    const auto &newYRange = YRanges[mCurrentYRangeIndex];

//    // Здесь всем графикам меняем приближение
//    mWaveGraph->setYRange(newYRange.first, newYRange.second);
//    mTrendGraph->setYRange(newYRange.first, newYRange.second);
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

//    // Обновление текущего диапазона, если были перемещения по оси X
//    if (!qFuzzyCompare(mTrendGraph->xAxis->range().lower, mLastXRange.first) ||
//            !qFuzzyCompare(mTrendGraph->xAxis->range().upper, mLastXRange.second)) {
//        mLastXRange = setTrendGraphToTheCenter(mXRangesOfSecondsWithIcons[mCurrentXRangeIndex].interval, prevTimeOfSensorReadingMs);
//        return true;
//    }

    // Получаем объект
    ++mCurrentXRangeIndex;
    mCurrentXRangeIndex %= countIntervals;
    const auto &intervalAndIcon = mXRangesOfSecondsWithIcons[mCurrentXRangeIndex];

    // Устанавливаем картинку кнопке
    AbstractGraphAreaWidget::ui->xRangeGraphToolButton->setIcon(intervalAndIcon.defaultButtonIcon, intervalAndIcon.pressedButtonIcon);

    // Меняем диапазон
    //mLastXRange = setTrendGraphToTheCenter(intervalAndIcon.interval, prevTimeOfSensorReadingMs);

    return true;
}
