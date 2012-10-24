
#ifndef SOXFILTERWIDGET_H
#define SOXFILTERWIDGET_H

#include "../../core/codecwidget.h"

class SoxEffectWidget;

class QCheckBox;
class KComboBox;
class QVBoxLayout;


class SoxFilterWidget : public FilterWidget
{
    Q_OBJECT
public:
    SoxFilterWidget();
    ~SoxFilterWidget();

    FilterOptions *currentFilterOptions();
    bool setCurrentFilterOptions( FilterOptions *_options );

private:
    QCheckBox *chSampleSize;
    KComboBox *cSampleSize;
    QCheckBox *chSampleRate;
    KComboBox *cSampleRate;
    QCheckBox *chChannels;
    KComboBox *cChannels;
    QVBoxLayout *effectWidgetsBox;
    QList<SoxEffectWidget*> effectWidgets;

private slots:
    void addEffectWidgetClicked();
    void removeEffectWidgetClicked( SoxEffectWidget *widget );
};

#endif // SOXFILTERWIDGET_H
