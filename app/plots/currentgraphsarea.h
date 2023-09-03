#ifndef CURRENTGRAPHAREA_H
#define CURRENTGRAPHAREA_H

#include "plots/abstractmultiplegraphsareaswidget.h"
#include "controller/controller_enums.h"
#include "global_define.h"

namespace Ui {
class CurrentGraphsArea;
}

class WaveFormPlot;
class TrendPlot;
class LabelMarkItem;
class RecordedPlot;


extern QFile mHeadFile;
extern QFile mIntervalsFile;
extern QFile mMarksFile;
extern QFile mRawDataFile;

#define MAX_CNT_RECORD 300

//#pragma pack(1)
typedef struct
{
  uint32_t timeOffset;
  uint16_t rawData;
}_recordFields;

typedef struct
{
   _recordFields field[MAX_CNT_RECORD];
   uint16_t currentPos;
}_bufferRecord;
//#pragma pack(4)

extern uint8_t currentBufferRecord;
extern _bufferRecord bufferRecord_1, bufferRecord_2;


class CurrentGraphsArea : public AbstractMultipleGraphsAreasWidget
{
    Q_OBJECT

    /*################################################################################
                                Текущие показания датчика
    ################################################################################*/
    //! График текущих значений
    WaveFormPlot *mWaveGraph {nullptr};

    /*! Отрисовка каждой {устанавливаемое число} точки на графике текущих значений */
    int mThinningSensorReadings {2};

    /*! Счётчик пришедших точек */
    int mCounterSensorReadings {0};

    /*################################################################################
                                Записанные показания датчика
    ################################################################################*/
    //! График записанных значений
    RecordedPlot *mRecordedGraph {nullptr};



    /*################################################################################
                                Средние показания датчика
    ################################################################################*/
//    //! График средних значений
//    TrendPlot *mTrendGraph {nullptr};

    /*! Последний установленный диапазон X */
    QPair<double, double> mLastXRange {qMakePair(0, 0)};

    /*! Максимальное хранимое в RAM время показаний в миллисекундах (12 часов) */
    const int64_t mMaxStoredTimeMs {43200000};

    /*! Суммарное время хранимых показаний */
    int64_t mTotalTimeOfStoredReadingsMs {0};

    /*! Интервал смещения графика при полном заполнении в (процент сдвига / 100) */
    double mXAxisShiftInPercent {0.05};

    /*! Диапазон удаления старых значений в миллисекундах (1 минута)*/
    const int mDeleteIntervalMs {60000};

    /*! Время первого прихода данных (для возможности задать правильный диапазон оси Х) */
    QDateTime mDateTimeOfFirstData {};

    /*! Время прихода предыдущего показания */
    int64_t mPrevTimeOfSensorReadingMs {0};

    /*! Было ли касание пользователя на экране графика тренда */
    bool mUserTouchOrMouseEventOnRecordedGraph {false};



    /*! Шрифт для меток */
    QFont mFontForLabelItems {"Sans Serif", 16};

#ifdef PC_BUILD
    /*! Отступ метки от верхней границы графика */
    const double TopMarginForLabel {0.1};
#else
    const double TopMarginForLabel {0.055};
#endif

public:

    explicit CurrentGraphsArea(QWidget *parent = nullptr);
    ~CurrentGraphsArea();

    //! Состояние записи
    bool isRecord {false};

    /*! Устанавливает интервал на график записанных показаний */
    void addIntervalOnRecordedGraph();
private:

    /*################################################################################
                                Текущие показания датчика
    ################################################################################*/
//    /*! Сброс графика с текущими значениями */
//    void resetGraphOfCurrentValues();
    /*################################################################################
                                Записанные показания датчика
    ################################################################################*/
    /*! Сброс графика с записанными значениями */
    void resetRecordedGraph();

    /*! Устанавливает метку на график записанных показаний */
    void addLabelOnRecordedGraph();



    /*################################################################################
                                Средние показания датчика
    ################################################################################*/
    /*! Установка текущего значения графика тренда в центр по оси X.
        Возвращает установленный диапазон оси X.
        intervalSeconds - необходимый отображаемый интервал в секундах;
        timeOfLastReading - время последнего пришедшего показания
    */
    //QPair<double, double> setTrendGraphToTheCenter(int intervalSeconds, const QDateTime &timeOfLastReading);

    /*! Устанавливает метку на график Тренда */
    //void addLabelOnTrendGraph();

    /*! Сброс графика со средними значениями */
    //void resetTrendGraph();

    /*! Сброс всех меток */
    void resetAllLabelItems();

private slots:

    /*################################################################################
                                        Общее
    ################################################################################*/
    /*! Обработка событий контроллера */
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

    /*! Обновление отображения уровней тревоги на виджетах */
    void updateAlarmLevelsOnWidgets();

    /*! Обовление отображения состояний (вкл/выкл) на виджетах */
    void updateAlarmStatesOnWidgets();

    /*! Обновление параметров осей на виджетах */
    void updateIntervalsOnGraphs();
    void updateTicksOnGraphs();
    /*################################################################################
                                Текущие показания датчика
    ################################################################################*/
    /*! Устанавливаем данные на график внутричерепного давления */
    void addDataOnWavePlot();

    /*! Перерисовывает график текущего внутричерепного давления */
    void replotWaveGraph();

    /*################################################################################
                                Записанные показания датчика
    ################################################################################*/
    void replotRecordedGraph();


    /*################################################################################
                                Средние показания датчика
    ################################################################################*/
    /*! Добавляем данные для графика тренда */
    //void addDataOnTrendGraph();

private:
    Ui::CurrentGraphsArea *ui;

    // AbstractPlotAreaWidget interface

public:
    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;
    void retranslate() override;

public slots:
    void addDataOnRecordedPlot();

    /*! Сброс графика с текущими значениями */
    void resetGraphOfCurrentValues();

    void addOrDeleteNewItem(bool state);

    void changeXInterval(bool interval);
    void goToLabel(bool direction);
protected slots:
    void nextYRange() override;

    /*! Обновление диапазона отображения данных. Устанавливает график по центру
        в зависимости от текущего интервала mXRangesSecondsWithIcons[mCurrentXRange].interval
        и  времени последнего пришедшего показания mPrevTimeOfSensorReadingMs.
        false - нет диапазонов либо не приходило первое значение;
        true - успешно.
    */
    bool nextXRange() override;

    // AbstractGraphAreaWidget interface

    void replotDisplayedGraph() override;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void writeRawData(_bufferRecord *buffer);
    void addRawData(_bufferRecord *buffer);
    void colorInterval();
};

#endif // CURRENTGRAPHAREA_H
