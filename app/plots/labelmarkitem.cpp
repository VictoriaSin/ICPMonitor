#include "labelmarkitem.h"
#include "controller/labels/label.h"
#include "global_define.h"

LabelMarkItem::LabelMarkItem(QCustomPlot *parentPlot, const std::shared_ptr<Label> &label, const QFont &font, float level, LabelOrientation orientation) :
    MarkItem(parentPlot, "", font, level, orientation),
    mLabel(label)
{
    if (mLabel) {
        setText(QString::number(label->getNumberLabel()));
    }
}

LabelMarkItem::~LabelMarkItem()
{

}

Label *LabelMarkItem::getLabel() const
{
    return mLabel.get();
}
