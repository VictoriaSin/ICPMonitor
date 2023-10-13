#include "recordedplot.h"
#include "plots/labelmarkitem.h"
#include "gui/gui_funcs.h"
#include "gui/dialogWindows/texteditdialog.h"
#include "controller/labels/label.h"
#include "global_define.h"

#include "gui/mainpage.h"
#include "unistd.h"
#include "tavgfilter.h"
//LabelManager *mLabelManagerRecorded {nullptr};

float mCurrentMaxYRange;
float mRecordedMaxXRange {60};
float mRecordedMaxYRange {60};

RecordedPlot::RecordedPlot(QWidget *parent):
    AbstractCustomPlot(GraphType::RecordedGraph, parent),
    mTextEditDialog(new TextEditDialog(this))
{
    // Размер окна ввода
    mTextEditDialog->setMinimumSize(QSize(480, 360));
    isLabelCreating = false;
    isIntervalCreating = false;
    mICPSettings->getCurrentPressureIndex() == 0 ? mCurrentMaxYRange = 60 : mCurrentMaxYRange = 815.7;

    mRecordedData.clear();
    // Настраиваем приближение и движение
    axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // Включаем оптимизацию отображения меток
    setAdaptiveSamplingLabelItems(true);
    mMainGraph = addGraph();
    mMainGraph->setAntialiased(true);

    mMainGraph->setAdaptiveSampling(true);//&


    // Настраиваем ручку для отрисовки графика
    QColor color(51, 242, 67, 200); // зеленый

    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(mThicknessOfMainGraph);

    // Устанавливаем браш и ручку для отрисовки основного графика
    mMainGraph->setPen(pen);
    mMainGraph->setBrush(brush);

    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);


//    // Разрешаем зумить и двигать по осям
//    axisRect()->setRangeDragAxes(QList<QCPAxis *>({xAxis, xAxis2, yAxis}));
//    // Связываем изменение интервала оси X (нижней) и оси X2 (верхней)
//    connect(xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), xAxis2, QOverload<const QCPRange&>::of(&QCPAxis::setRange));

    // Связываем изменение интервала оси X (нижней) c оптимизацией отображения меток
    connect(xAxis, QOverload<const QCPRange&, const QCPRange&>::of(&QCPAxis::rangeChanged), this, [this](const QCPRange &newRange, const QCPRange &oldRange) {
        // Если ось приблизили
        if (newRange.size() != oldRange.size())
        {
            AbstractCustomPlot::labelItemsOptimization();
        }
    });

    // Обработка клика по элементам
    connect(this, &RecordedPlot::itemClick, this, &RecordedPlot::itemClicked);

    retranslate();
    addLayer("intervalLayer", nullptr, limAbove);

}

RecordedPlot::~RecordedPlot()
{
    qDebug() << "exit";
    mRecordedData.clear();
}

QPair<int, int> RecordedPlot::addInterval(uint8_t num, QColor color)
{
    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(mThicknessOfMainGraph);

    // Устанавливаем браш и ручку для отрисовки основного графика
    if (num == 2)
    {
        mIntervalFirst = addGraph();
        mIntervalFirst->setAntialiased(true);
        mIntervalFirst->setAdaptiveSampling(true);//?
        mIntervalFirst->setPen(pen);
        mIntervalFirst->setBrush(brush);
    }
    else
    {
        mIntervalSecond = addGraph();
        mIntervalSecond->setAntialiased(true);
        mIntervalSecond->setAdaptiveSampling(true);//?
        mIntervalSecond->setPen(pen);
        mIntervalSecond->setBrush(brush);
    }

    layer("intervalLayer")->setMode(QCPLayer::lmBuffered);

    QVector<float> temp;
    //for (int i=0; i<mRecordedData.count(); i++)
//    for (uint i=0; i<mSizeAllRecordedData; i++)
    for (int i=0; i<mMainGraph->data()->size(); i++)
    {
        //temp.push_back(mRecordedData[i].first);
        //qDebug() << "mRecordedData[i].first" << mRecordedData[i].first;
        //temp.push_back((float)mAllRecordedDataBuffer[i].timeStamp/1000);
        qDebug() << mMainGraph->data()->at(i)->key;
        temp.push_back(mMainGraph->data()->at(i)->key);
    }

    float t1 = (float)mIntervalsContainer[num-2]->mIntervalPos/1000.0;
    float t2 = (float)mIntervalsContainer[num-1]->mIntervalPos/1000.0;
qDebug() << "t1" << t1;
qDebug() << "t2" << t2;
    float first = 0.0;
    float second = 0.0;

    //for (int i=floor(t1)*25; i<=ceil(t2)*25; i++) //25 показаний в секунду
    for (int i=0; i<=temp.count(); i++) //25 показаний в секунду
    {
        //qDebug() << "i" << i << "x" << temp[i];//mRecordedData[i].first;
        if (temp[i] <= t1)//if (mRecordedData[i].first <= t1)
        {
            first = temp[i];//mRecordedData[i].first;
        }
        else if (temp[i] >= t2)//mRecordedData[i].first >= t2)
        {
            second = temp[i];//mRecordedData[i].first;
            break;
        }

    }
    qDebug() << "first" << first;
    qDebug() << "second" << second;

    int indexStart = temp.indexOf(first);
    int indexStop = temp.indexOf(second);
    qDebug() << "t1" << t1 << "indexStart" << indexStart;
    qDebug() << "t2" << t2 << "indexStop" << indexStop;
    //QVector<QPair<float, float>> intervalVec;
    for (int i=indexStart; i<=indexStop; i++)
    {
//        if (mIntervalsContainer[num-1]->maxIntervalValue < mAllRecordedDataBuffer[i].data)
//        {
//            mIntervalsContainer[num-1]->maxIntervalValue =  mAllRecordedDataBuffer[i].data;
//        }
//        mIntervalsContainer[num-1]->averageIntervalValue += mAllRecordedDataBuffer[i].data;
//        intervalVec.append(qMakePair((float)mAllRecordedDataBuffer[i].timeStamp/1000, mAllRecordedDataBuffer[i].data));
//        num == 2 ? mIntervalFirst->addData(intervalVec.back().first, intervalVec.back().second)
//                 : mIntervalSecond->addData(intervalVec.back().first, intervalVec.back().second);

        if (mIntervalsContainer[num-1]->maxIntervalValue < mMainGraph->data()->at(i)->value)
        {
            mIntervalsContainer[num-1]->maxIntervalValue =  mMainGraph->data()->at(i)->value;
        }
        mIntervalsContainer[num-1]->averageIntervalValue += mMainGraph->data()->at(i)->value;
        //intervalVec.append(qMakePair(mMainGraph->data()->at(i)->key, mMainGraph->data()->at(i)->value));
        num == 2 ? mIntervalFirst->addData(mMainGraph->data()->at(i)->key, mMainGraph->data()->at(i)->value)
                 : mIntervalSecond->addData(mMainGraph->data()->at(i)->key, mMainGraph->data()->at(i)->value);
    }
//    QVector<QPair<float, float>> tttt = mRecordedData.mid(indexStart, (indexStop-indexStart+1));
    qDebug() << "indexStart" << indexStart << "indexStop" << indexStop;
    mIntervalsContainer[num-1]->averageIntervalValue /= (indexStop - indexStart + 1);
    return qMakePair(first*1000, second*1000);
    //return qMakePair(indexStart, indexStop); // индексы записанного гррафика, каждые 40 мс
}

void RecordedPlot::saveDataForGraphic(unsigned int  x, unsigned int  y)//const ComplexValue &complexVal)
{
    float temp_x = (float) x/1000;
    float temp_y = (float) y;
    mRecordedData.push_back(qMakePair(temp_x, temp_y));
    //qDebug() << "x" << temp_x;
//    // Суммирование общего времени пришедших данных с датчика
//    // для ограничения отображения данных в диапазоне допустимых
//    // значение времени на оси Х графика

//    if (mPreviousSensorDataTime == 0)
//    {
//        mPreviousSensorDataTime = complexVal.timestamp;
//    }
//    else
//    {
//        mSummarySensorDataTimePerXRange += (complexVal.timestamp - mPreviousSensorDataTime) / 1000.0;
//    }

////    //#ifdef QT_DEBUG
////    if (avgBenchTime > 1000) {
////        //qDebug() << "Gui update period, ms" << avgBenchTime / (float)benchCount;
////        avgBenchTime = 0;
////        benchCount = 0;
////    }
////    //#endif
//    mRecordedData.push_back(qMakePair(mSummarySensorDataTimePerXRange, complexVal.value));
//    mPreviousSensorDataTime = complexVal.timestamp;
//    qDebug() << mRecordedData.size();
}

void RecordedPlot::downloadData(QByteArray *temp)
{
//    mMainGraph->data().clear();
//    _mSPIData ttt;
//    for (int i=0; i< temp->size()/6; i++)
//    {
//        ttt
//    }
}

//#define timerDelay 5
#define axisMoveRange 0.03

void RecordedPlot::animateGraphic(int timerDelaySec)
{
    QTimer *replayDataTimer = new QTimer(this);
    //int index = 0;

    if (isPlayRecord == true)
    {
        qDebug("stop");
        replayDataTimer->stop();
    }
    else
    {
        qDebug("start");
        replayDataTimer->start(float(timerDelaySec/100));//мс
    }
    connect(replayDataTimer, &QTimer::timeout, [this, replayDataTimer, timerDelaySec]()
    {
        if (isPlayRecord == true) // если нажата кнопка паузы
        {
            replayDataTimer->stop();
        }
        else
        {
            if (xAxis->range().upper + axisMoveRange < mRecordedMaxXRange)
            {
                xAxis->moveRange(axisMoveRange);
                replayDataTimer->start(float(timerDelaySec/100));//мс
            }
            else
            {
                xAxis->setRangeUpper(mRecordedMaxXRange);
                replayDataTimer->stop();
                isPlayRecord = true;
                emit(changeBtnIcon());
            }
        }
    });
}

void RecordedPlot::addDataOnGraphic()
{
    //float mNewUpperXValue = (float)mRecordedData.count()/25;// показаний в файле в секунду 50, а мы берем каждое второе
    //qDebug() << mSizeAllRecordedData;
    //qDebug() << "rr" <<mAllRecordedDataBuffer[mSizeAllRecordedData-1].timeStamp;
    //float mNewUpperXValue = (float)mAllRecordedDataBuffer[mSizeAllRecordedData-1].timeStamp/1000;
    QFile *currFile;
    if (isDownloadGraph)
    {
        mMainGraph->data()->clear();
        currFile = &mTestData;
    }
    else
    {
        currFile = &mRawDataFile;
    }
    float mNewUpperXValue;

    //for (int i = 0; i < mRecordedData.count(); i++)

    currFile->open(QIODevice::ReadOnly);
    _mSPIData temp;

    uint recordDataCount = currFile->size();
    float tempTimeOffset = 0;
    float param = 1.0;
    if (mICPSettings->getCurrentPressureIndex() == 1)
    {
        param = indexPressureH2O;
    }
    QVector<float> filterData;
    //for (uint i=0; i< mRawDataFile.size()/6; i+=10) // 40/4=10
    for (uint i=0; i < recordDataCount; i+=60)
    {

        //mRawDataFile.seek(i*sizeof(_mSPIData));
        currFile->seek(i);        
        currFile->read((char*)&temp, sizeof(_mSPIData));
        tempTimeOffset = (float)temp.timeStamp/1000;
        mMainGraph->addData(tempTimeOffset, (float)temp.data*param);
//        currFile->seek(i);
//        for (int j=0; j< 10; j++)
//        {
//            currFile->seek(i+j);
//            currFile->read((char*)&temp, sizeof(_mSPIData));
//            filterData.append((float)temp.data);
//        }
        //qDebug() << "temp.timeStamp" <<temp.timeStamp;
    }
    qDebug() << "vector size" << filterData.size();
    currFile->close();

//    TAVGFilter *filterGraph = new TAVGFilter(50, 15);
//    float filter_y;
//    uint N = filterData.size();
//    QVector<float> maxY;
//    QVector<float> minY;
//    float lastY = -1;
//    for (uint i = 0; i < N; i++)
//    {
//        filter_y = filterGraph->Filtrate(filterData[i]);
//        filterData[i] = filter_y;
//        if (i == 1)
//        {
//            if (filterData[0] < filterData[1])
//            {
//                minY.push_back(filterData[0]);
//            }
//            else
//            {
//                maxY.push_back(filterData[0]);
//            }
//            lastY = filterData[1];
//        }
//        else
//        {
//            if (filter_y > lastY)
//            {

//            }
//        }
//    }
//    delete filterGraph;
    mNewUpperXValue = tempTimeOffset;
    qDebug() <<"x max"<< mNewUpperXValue;
    if (mNewUpperXValue < xAxis->range().upper)
    {
        xAxis->setRangeUpper(mNewUpperXValue);
        //setInteraction(QCP::iRangeDrag, false);
    }
    mRecordedMaxXRange = mNewUpperXValue;
//    for (uint i=0; i<mSizeAllRecordedData; i++)
//    {
//        //mMainGraph->addData(mRecordedData[i].first, mRecordedData[i].second);
//        mMainGraph->addData((float)mAllRecordedDataBuffer[i].timeStamp/1000, mAllRecordedDataBuffer[i].data);
//        qDebug() << "data" << mAllRecordedDataBuffer[i].data;
//    }
}


//void RecordedPlot::setLabelManager(LabelManager *labelManager)
//{
//    //mLabelManager = labelManager;
//    //mLabelManagerRecorded = labelManager;
//}

void RecordedPlot::itemClicked(QCPAbstractItem *item, QMouseEvent *event)
{
    qDebug() << item;
    const LabelMarkItem *labelItem = dynamic_cast<LabelMarkItem*>(item);

    // Если преобразование прошло успешно и событие отпускания левой мышки
    if (labelItem && event->type() == QEvent::Type::MouseButtonRelease && event->button() == Qt::MouseButton::LeftButton)
    {
        // Метка, связанная с отображение на графике
        Label *label = labelItem->getLabel();

        // Если метки нет
        if (!label) { return; }

        // Время начала и окончания метки
        const int64_t &timeStartLabelMs = label->getTimeStartLabelMS();
        const int64_t &timeEndLabelMs = label->getTimeEndLabelMS();

        // Устанавливаем id сессии, к которой относится метка,
        // номер метки, время начала метки и окончание
        mTextEditDialog->setInfoLabelText(TemplateInfoAboutLabel.arg(tr("ID сессии")).arg(label->getBelongIdSession()).
                                          arg(tr("Номер метки")).arg(label->getNumberLabel()).
                                          arg(tr("Время начала метки")).arg(QDateTime::fromMSecsSinceEpoch(timeStartLabelMs).toString("dd.MM.yyyy hh:mm:ss.zzz")).
                                          arg(tr("Время окончания метки")).arg(timeEndLabelMs > timeStartLabelMs ? QDateTime::fromMSecsSinceEpoch(timeEndLabelMs).toString("dd.MM.yyyy hh:mm:ss.zzz") : "-").
                                          arg(tr("Информация")));

        // Информация о метке
        const QString &labelInfo = label->getInfoLabel();

        // Устанавливаем информацию о метке
        mTextEditDialog->setText(labelInfo);

        // Если пользователь подтвердил изменения
        if (mTextEditDialog->exec() == QDialog::Accepted) {
            // Запоминаем текст из поля ввода
            const QString newText = mTextEditDialog->getText();

            // Если изменения были, обновляем информацию о метке
            if (labelInfo != newText) {
                label->setLabelInfo(newText);
            }
        }
    }
}

void RecordedPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    mICPSettings->getCurrentPressureIndex() == 0 ? yAxis->setLabel(tr("мм рт ст")) : yAxis->setLabel(tr("мм вод ст"));

    mTextEditDialog->retranslate();
    mTextEditDialog->setWindowTitle(tr("Информация о метке"));
}

void RecordedPlot::scaleFont(float scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
    mTextEditDialog->scaleFont(scaleFactor);
}

void RecordedPlot::resetGraph()
{
    mPreviousSensorDataTime = 0;
    mSummarySensorDataTimePerXRange = 0;
    mRecordedData.clear();
    setInteraction(QCP::iRangeDrag, true);
    mRecordedMaxXRange = 60;
    //mCurrentMaxYRange = 700; //60
    mMainGraph->data()->clear();
    //mIntervalFirst->data().clear();
    //mIntervalSecond->data().clear();
    qDebug() << "RecordedPlot reset";
}
#ifdef PC_BUILD

bool RecordedPlot::editLabel(QMouseEvent *mouseEvent)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                pointStart = pointStop;
                mCoordLabelX += deltaX;
                mLabelItemsContainer.back()->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                mCoordLabelX = pointStart;
                mLabelItemsContainer.back()->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}
bool RecordedPlot::editInterval(QMouseEvent *mouseEvent)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                auto ttt = (mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX);
                if (mIntervalsCount % 2 == 0)
                {
                    if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                    if (mIntervalsCount == 3)
                    {
                        if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                    }
                pointStart = pointStop;
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mIntervalsCount % 2 == 0)
                {
                    if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                if (mIntervalsCount == 3)
                {
                    if (pointStart < (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}
bool RecordedPlot::editAxisRange(QMouseEvent *mouseEvent)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isAxisMoving = false;
    static float pointStartX, pointStartY;
    static float pointStopX, pointStopY;

    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) || (typeOfEvent == QEvent::Wheel))
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            isAxisMoving = true;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStartY = yAxis->pixelToCoord(mouseEvent->pos().y());
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (isAxisMoving))
        {
            pointStopX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStopY = yAxis->pixelToCoord(mouseEvent->pos().y());
            float deltaX = pointStopX - pointStartX;
            float deltaY = pointStopY - pointStartY;

            if (!((xAxis->range().lower - deltaX < 0) || (xAxis->range().upper - deltaX > mRecordedMaxXRange)))
            {
                xAxis->setRange(xAxis->range().lower - deltaX, xAxis->range().upper - deltaX);
            }
            if (!((yAxis->range().lower - deltaY < 0) || (yAxis->range().upper - deltaY > mCurrentMaxYRange)))
            {
                yAxis->setRange(yAxis->range().lower - deltaY, yAxis->range().upper - deltaY);
            }

            pointStartX = pointStopX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStartY = pointStopY = yAxis->pixelToCoord(mouseEvent->pos().y());
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            isAxisMoving = false;
            return true;
        }

        if (typeOfEvent == QEvent::Wheel)
        {
            #define DEF_ZOOM_IN_X  (float)0.8
            #define DEF_ZOOM_OUT_X (float)(1.0/DEF_ZOOM_IN_X)
            QWheelEvent *eventWheel = (QWheelEvent*)mouseEvent;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());

            if (eventWheel->delta() < 0)
            {
              //qDebug("Zoom-");
              //qDebug() << "pointStartX:" << pointStartX << " (pointStartX *  DEF_ZOOM_IN_X)" << (pointStartX *  DEF_ZOOM_IN_X) << " xAxis->range().lower" << xAxis->range().lower;
              float startPlot;
              float endPlot;
              bool customPlot = false;
              if ((pointStartX - (pointStartX * DEF_ZOOM_IN_X)) > xAxis->range().lower) // < 0
              {
                customPlot = true;
                startPlot = 0;
              }

              if ((xAxis->range().upper + (xAxis->range().upper *  DEF_ZOOM_IN_X)) > mRecordedMaxXRange) // > MAX
              {
                customPlot = true;
                startPlot = xAxis->range().lower * DEF_ZOOM_IN_X;
                endPlot = mRecordedMaxXRange;
              }
              else
              {
                endPlot = xAxis->range().size() / DEF_ZOOM_IN_X;
              }

              if (customPlot)
              {
                xAxis->setRange(startPlot, endPlot);
                return true;
              }
              xAxis->scaleRange(DEF_ZOOM_OUT_X, pointStartX);
              return true;
            }
            else
            {
              //qDebug("Zoom+");
              xAxis->scaleRange(DEF_ZOOM_IN_X, pointStartX);
            }
            return true;
        }
    }
    return false;
}


#else
// Для тача
bool RecordedPlot::editLabel(QTouchEvent *touchEvent)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) )
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                pointStart = pointStop;
                mCoordLabelX += deltaX;
                mLabelItemsContainer.back()->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                mCoordLabelX = pointStart;
                mLabelItemsContainer.back()->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool RecordedPlot::editInterval(QTouchEvent *touchEvent)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) )
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                auto ttt = (mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX);
                if (mIntervalsCount % 2 == 0)
                {
                    if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                    if (mIntervalsCount == 3)
                    {
                        if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                    }
                pointStart = pointStop;
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mIntervalsCount % 2 == 0)
                {
                    if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                if (mIntervalsCount == 3)
                {
                    if (pointStart < (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool RecordedPlot::editAxisRange(QTouchEvent *touchEvent)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isAxisMoving = false;
    static float pointStartX, pointStartY;
    static float pointStopX, pointStopY;

    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate))// || (typeOfEvent == QEvent::Wheel))
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            isAxisMoving = true;
            pointStartX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStartY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (isAxisMoving))
        {
            pointStopX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStopY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            float deltaX = pointStopX - pointStartX;
            float deltaY = pointStopY - pointStartY;

            if (!((xAxis->range().lower - deltaX < 0) || (xAxis->range().upper - deltaX > mRecordedMaxXRange)))//mCurrentMaxXRange)))
            {
                xAxis->setRange(xAxis->range().lower - deltaX, xAxis->range().upper - deltaX);
            }
            if (!((yAxis->range().lower - deltaY < 0) || (yAxis->range().upper - deltaY > mCurrentMaxYRange)))
            {
                yAxis->setRange(yAxis->range().lower - deltaY, yAxis->range().upper - deltaY);
            }

            pointStartX = pointStopX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStartY = pointStopY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            isAxisMoving = false;
            return true;
        }

        /*if (typeOfEvent == QEvent::Wheel)
        {
            #define DEF_ZOOM_IN_X  (float)0.8
            #define DEF_ZOOM_OUT_X (float)(1.0/DEF_ZOOM_IN_X)
            QWheelEvent *eventWheel = (QWheelEvent*)mouseEvent;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());

            if (eventWheel->delta() < 0)
            {
              //qDebug("Zoom-");
              //qDebug() << "pointStartX:" << pointStartX << " (pointStartX *  DEF_ZOOM_IN_X)" << (pointStartX *  DEF_ZOOM_IN_X) << " xAxis->range().lower" << xAxis->range().lower;
              float startPlot;
              float endPlot;
              bool customPlot = false;
              if ((pointStartX - (pointStartX * DEF_ZOOM_IN_X)) > xAxis->range().lower) // < 0
              {
                customPlot = true;
                startPlot = 0;
              }

              if ((xAxis->range().upper + (xAxis->range().upper *  DEF_ZOOM_IN_X)) > mRecordedMaxXRange) //mCurrentMaxXRange) // > MAX
              {
                customPlot = true;
                startPlot = xAxis->range().lower * DEF_ZOOM_IN_X;
                endPlot = mRecordedMaxXRange;//mCurrentMaxXRange;
              }
              else
              {
                endPlot = xAxis->range().size() / DEF_ZOOM_IN_X;
              }

              if (customPlot)
              {
                xAxis->setRange(startPlot, endPlot);
                return true;
              }
              xAxis->scaleRange(DEF_ZOOM_OUT_X, pointStartX);
              return true;
            }
            else
            {
              //qDebug("Zoom+");
              xAxis->scaleRange(DEF_ZOOM_IN_X, pointStartX);
            }
            return true;
        }*/
    }
    return false;
}

#endif
//bool RecordedPlot::event(QEvent *event)
//{
//#ifdef PC_BUILD


//    if (isLabelCreating == true)
//    {
//        if (editLabel((QMouseEvent*)event)) return true;
//    }
//    else if (isIntervalCreating == true)
//    {
//        if (editInterval((QMouseEvent*)event)) return true;
//    }
//    else
//    {
//        if (editAxisRange((QMouseEvent*)event)) return true;
//    }

//#else

//    if (isLabelCreating == true)
//    {
//        if (editLabel((QTouchEvent*)event)) return true;
//    }
//    else if (isIntervalCreating == true)
//    {
//        if (editInterval((QTouchEvent*)event)) return true;
//    }
//    else
//    {
//        if (editAxisRange((QTouchEvent*)event)) return true;
//    }

////    // Если событие - взаимодействие с сенсором
////    if(typeOfEvent == QEvent::TouchBegin ||
////       typeOfEvent == QEvent::TouchEnd ||
////       typeOfEvent == QEvent::TouchUpdate ||
////       typeOfEvent == QEvent::TouchCancel)
////    {
////        // Кастуем
////        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

////        // Получаем список точек касания
////        auto& touchPoints = touchEvent->touchPoints();

////        // Если нажали одним пальцем после нажатия двумя пальцами
////        if(touchPoints.count() == 1 && mDoublePointsTouch) {
////            // Отправляем событие для отпускания тача
////            const auto &point = touchPoints.first();
////            auto ev = QMouseEvent(QEvent::MouseButtonRelease, point.lastPos(),
////                                  point.lastScenePos(), point.lastScreenPos(),
////                                  Qt::MouseButton::LeftButton, Qt::MouseButtons(),
////                                  Qt::KeyboardModifiers(Qt::KeyboardModifier::NoModifier),
////                                  Qt::MouseEventSource::MouseEventSynthesizedBySystem);
////            mouseReleaseEvent(&ev);

////            // Обнуляем флаг нажатия двумя пальцами
////            mDoublePointsTouch = false;

////            // Отдаём событие на дальнейшую обработку
////            event->ignore();

////            // Событие было распознано
////            return true;
////        }

////        // Если нажатие двумя пальцами
////        if (touchPoints.count() == 2)
////        {
////            // Устанавливаем флаг нажатия двумя пальцами
////            mDoublePointsTouch = true;

////            // Сохраняем позиции точек
////            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
////            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

////            // Отношение текущей линии к предыдущей
////            float scaleFactor =
////                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length() /
////                    QLineF(touchPoint0.lastPos(), touchPoint1.lastPos()).length();

////            // Центр прямоугольника (точка скейлинга)
////            QPointF scalingPointF((touchPoint0.pos().x() + touchPoint1.pos().x())/2,
////                                  (touchPoint0.pos().y() + touchPoint1.pos().y())/2);

////            // Если было отдаление, делаем скейл отрицательным
////            scaleFactor *= scaleFactor < 1 ? -1 : 1;

////            // Если не отпустили пальцы, отправляем событие колеса мышки
////            if(!touchEvent->touchPointStates().testFlag(Qt::TouchPointReleased)){
////                QWheelEvent ev(scalingPointF, scalingPointF, QPoint(0, 0), QPoint(0, scaleFactor * scaleSensitivity),
////                               Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier, Qt::ScrollPhase::NoScrollPhase, false);
////                wheelEvent(&ev);
////            }

////            // Событие не требует дальнейшей обработки
////            event->setAccepted(true);

////            // Событие было распознано
////            return true;
////        }
////    }
////    // Если не распознали или не хотим обрабатывать событие, то отдаём родителю
//#endif
//    return AbstractCustomPlot::event(event);
//}
