
#ifndef NORMALIZEFILTERWIDGET_H
#define NORMALIZEFILTERWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;

class NormalizeFilterWidget : public FilterWidget
{
    Q_OBJECT
public:
    NormalizeFilterWidget();
    ~NormalizeFilterWidget();

    FilterOptions *currentFilterOptions();
    bool setCurrentFilterOptions( FilterOptions *_options );

private:
    QCheckBox *cNormalize;
};

#endif // NORMALIZEFILTERWIDGET_H
