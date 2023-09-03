#include "abstractmultiplegraphsareaswidget.h"
#include "ui_abstractgraphareawidget.h"
#include "plots/waveformplot.h"
#include "gui/gui_funcs.h"
#include "animatedtoolbutton.h"

#include "global_define.h"

AbstractMultipleGraphsAreasWidget::AbstractMultipleGraphsAreasWidget(QWidget *parent) :
    AbstractGraphAreaWidget(parent)
{
    // Настройка кнопки смены графиков
    setupChangeGraphToolButton();
}

AbstractMultipleGraphsAreasWidget::~AbstractMultipleGraphsAreasWidget()
{

}

void AbstractMultipleGraphsAreasWidget::updateDisplayedGraphName()
{
    switch (mDisplayedGraph)
    {
//    case AbstractCustomPlot::GraphType::TrendGraph: {
//        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("ТРЕНД"));
//        break;
//    }
    case AbstractCustomPlot::GraphType::WaveFormGraph: {
        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("СИГНАЛ ВЧД"));
        break;
    }
    case AbstractCustomPlot::GraphType::BoxGraph: {
        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("ЯЩИК С УСАМИ"));
        break;
    }
    case AbstractCustomPlot::GraphType::HistGraph: {
        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("ГИСТОГРАММА"));
    }
    case AbstractCustomPlot::GraphType::RecordedGraph: {
        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr(" "));//("ЗАПИСЬ"));
    }
    default: break;
    }
}

void AbstractMultipleGraphsAreasWidget::setupChangeGraphToolButton()
{
    // Если кнопка уже создана
    if (mChangeGraphToolButton) {
        return;
    }

    // Настраиваем кнопку для переключения между статистическими графиками
    mChangeGraphToolButton = new AnimatedToolButton(this);
    mChangeGraphToolButton->setIcon(QIcon(":/icons/otherDisplay.svg"), QIcon(":/icons/otherDisplay_pressed.svg"));
    mChangeGraphToolButton->setIconSize(QSize(90, 90));
    mChangeGraphToolButton->setStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss"));
    mChangeGraphToolButton->setEnabled(false);


    // Добавляем кнопку для переключения между статистическими графиками в левый layout
    addWidgetOnLeftVerticalLayout(mChangeGraphToolButton);

    mChangeGraphToolButton->hide();
//    // Смена графика
//    connect(mChangeGraphToolButton, &QToolButton::clicked, this, &AbstractMultipleGraphsAreasWidget::changeGraph);

}

void AbstractMultipleGraphsAreasWidget::changeGraph()
{
    // Всего графиков
    const int countGraph = mGraphContainer.size();

    // Если графиков меньше двух
    if (countGraph < 2) {
        return;
    }

    // Скрываем текущий график
    mGraphContainer[mCurrentGraphIndex]->hide();

    // Узнаём индекс следующего графика
    ++mCurrentGraphIndex;
    mCurrentGraphIndex %= countGraph;

    // Берём график
    const auto graph = mGraphContainer[mCurrentGraphIndex];

    isLabelCreating = false;
    isIntervalCreating = false;
    // Устанавливаем тип отображаемого графика
    mDisplayedGraph = graph->type();

//#ifdef QT_DEBUG // Для замера ФПС
    // Если график текущих значений
    if (mDisplayedGraph == AbstractCustomPlot::GraphType::WaveFormGraph) {
        // Если удалось скастовать
        if (auto WaveGraph = dynamic_cast<WaveFormPlot *>(graph)) {
            WaveGraph->benchTime = QDateTime::currentMSecsSinceEpoch();
        }
    }
//#endif    
    // Если есть кнопка для изменения диапазона оси X графика тренда
    //if (ui->xRangeGraphToolButton) {
    //    // Если установленный график не Тренд, то скрываем кнопку
    //    if (mDisplayedGraph != AbstractCustomPlot::GraphType::TrendGraph)
    //    {
    //        if (!ui->xRangeGraphToolButton->isHidden())
    //        {
    //            ui->xRangeGraphToolButton->hide();
    //        }
    //    }
    //    else
    //    {
    //        ui->xRangeGraphToolButton->show();
    //    }
    //}

    // Показываем график
    graph->show();

    // Обновляем имя текущего графика
    updateDisplayedGraphName();
}

void AbstractMultipleGraphsAreasWidget::retranslate()
{
    AbstractGraphAreaWidget::retranslate();
}

void AbstractMultipleGraphsAreasWidget::scaleFont(float scaleFactor)
{
    AbstractGraphAreaWidget::scaleFont(scaleFactor);
}

//void AbstractMultipleGraphsAreasWidget::changeButtonStatus()
//{
//    if (mChangeGraphToolButton->isEnabled())
//    {
//        mChangeGraphToolButton->setEnabled(false);
//    }
//    else
//    {
//        mChangeGraphToolButton->setEnabled(true);
//    }
//}
