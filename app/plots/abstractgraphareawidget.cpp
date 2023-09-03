#include "abstractgraphareawidget.h"
#include "ui_abstractgraphareawidget.h"
#include "gui/gui_funcs.h"
#include <QDebug>

AbstractGraphAreaWidget::AbstractGraphAreaWidget(QWidget *parent) :
    QWidget(parent),
    mGuiUpdateTimer(new QTimer(this)),
    ui(new Ui::AbstractGraphAreaWidget)
{
    ui->setupUi(this);

    // Настройка кнопок
    setupButtons();

    // Настраиваем шрифт для имён графиков
    QFont mGraphNameFont;
    mGraphNameFont.setBold(true);
    mGraphNameFont.setFamily("Sans Serif");
    mGraphNameFont.setPointSize(18);

    // Устанавливаем шрифт имени графика
    ui->nameGraphLabel->setFont(mGraphNameFont);

    // Настройка перерисовки графиков
    mGuiUpdateTimer->setInterval(mGuiUpdate);
    mGuiUpdateTimer->setTimerType(Qt::PreciseTimer);
    connect(mGuiUpdateTimer, &QTimer::timeout, this, &AbstractGraphAreaWidget::replotDisplayedGraph);

}

AbstractGraphAreaWidget::~AbstractGraphAreaWidget()
{
    delete ui;
}

void AbstractGraphAreaWidget::installController(MonitorController *controller)
{
    mController = controller;
}

void AbstractGraphAreaWidget::retranslate()
{
    ui->retranslateUi(this);

    // Обновление имени графика
    updateDisplayedGraphName();
}

void AbstractGraphAreaWidget::scaleFont(float scaleFactor)
{
    // Скейлим имя графика
    WFontScaling(ui->nameGraphLabel, scaleFactor);
}

bool AbstractGraphAreaWidget::addWidgetOnLeftVerticalLayout(QWidget *widget)
{
    // Если пришёл nullptr
    if (!widget) {
        return false;
    }

    // Добавляем виджет
    ui->leftVerticalLayout->insertWidget(1, widget);

    // Добавляем разделитель
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->leftVerticalLayout->insertItem(1, spacer);

    return true;
}

bool AbstractGraphAreaWidget::initializeXRangeToolButton()
{
    // Если не заполнили интервалы с изображениями кнопок
    if (mXRangesOfSecondsWithIcons.isEmpty()) {
        return false;
    }

    // Устанавливаем изображение первого диапазона
    ui->xRangeGraphToolButton->setIcon(mXRangesOfSecondsWithIcons[mCurrentXRangeIndex].defaultButtonIcon, mXRangesOfSecondsWithIcons[mCurrentXRangeIndex].pressedButtonIcon);
    ui->xRangeGraphToolButton->setIconSize(QSize(90, 90));
    ui->xRangeGraphToolButton->setStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss"));
    return true;
}

void AbstractGraphAreaWidget::setupButtons()
{
    // Настройка анимаций кнопок
    ui->yRangeGraphToolButton->setIcon(QIcon(":/icons/scaleGraph.svg"), QIcon(":/icons/scaleGraph_pressed.svg"));
    ui->yRangeGraphToolButton->setIconSize(QSize(90, 90));
    ui->yRangeGraphToolButton->setStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss"));
    ui->yRangeGraphToolButton->hide();
}

bool AbstractGraphAreaWidget::startPlotting()
{
    if (!mGuiUpdateTimer->isActive()) {
        mGuiUpdateTimer->start();
        return true;
    }
    return false;
}

bool AbstractGraphAreaWidget::stopPlotting()
{
    if (mGuiUpdateTimer->isActive()) {
        mGuiUpdateTimer->stop();
        return true;
    }
    return false;
}

void AbstractGraphAreaWidget::showEvent(QShowEvent */*event*/)
{
    //startPlotting();
}

void AbstractGraphAreaWidget::hideEvent(QHideEvent */*event*/)
{
    //stopPlotting();
}
