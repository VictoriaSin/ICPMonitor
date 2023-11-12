#include "abstractmultiplegraphsareaswidget.h"
#include "ui_abstractgraphareawidget.h"
#include "plots/waveformplot.h"
#include "gui/gui_funcs.h"
#include "animatedtoolbutton.h"

#include "global_define.h"
#include "unistd.h"

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
    case AbstractCustomPlot::GraphType::WaveFormGraph: {
        //AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("СИГНАЛ ВЧД"));
        break;
    }
//    case AbstractCustomPlot::GraphType::BoxGraph: {
//        AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("ЯЩИК С УСАМИ"));
//        break;
//    }
    //case AbstractCustomPlot::GraphType::HistGraph: {
    //    AbstractGraphAreaWidget::ui->nameGraphLabel->setText(tr("ГИСТОГРАММА"));
    //}
    case AbstractCustomPlot::GraphType::RecordedGraph: {
        //AbstractGraphAreaWidget::ui->nameGraphLabel->setText(" ");//("ЗАПИСЬ"));
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

void AbstractMultipleGraphsAreasWidget::changeGraph(uint8_t number)
{
    // Всего графиков
    const int countGraph = mGraphContainer.size();

    // Если графиков меньше двух
    if (countGraph < 2 || number >= countGraph) { return; }

    mGraphContainer[mCurrentGraphIndex]->hide();
    mCurrentGraphIndex = number;

    // Берём график
    const auto graph = mGraphContainer[mCurrentGraphIndex];

    isLabelCreating = false;
    isIntervalCreating = false;
    isFluidIntervalCreating = false;

    // Устанавливаем тип отображаемого графика
    mDisplayedGraph = graph->type();
    if (number == 4)
    {
        mGraphContainer[0]->show();
    }
    else if (number != 0)
    {
        mGraphContainer[0]->hide();
    }

    graph->show();
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
