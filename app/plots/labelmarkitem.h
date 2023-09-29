#ifndef LABELMARKITEM_H
#define LABELMARKITEM_H

#include "markitem.h"

class LabelMarkItem : public MarkItem
{
   /*! Метка */
    std::shared_ptr<Label> mLabel;
public:
    LabelMarkItem(QCustomPlot *parentPlot, const std::shared_ptr<Label> &label, const QFont &font, float level = 0, LabelOrientation orientation = LabelOrientation::moVerticalBottom);
    ~LabelMarkItem();

    /*! Возвращает метку */
    Label *getLabel() const;
};

#endif // LABELMARKITEM_H
