
#ifndef SOXFILTERWIDGET_H
#define SOXFILTERWIDGET_H

#include "../../core/codecwidget.h"

class SoxEffectWidget;

class QCheckBox;
class QComboBox;
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
    QComboBox *cSampleSize;
    QCheckBox *chSampleRate;
    QComboBox *cSampleRate;
    QCheckBox *chChannels;
    QComboBox *cChannels;
    QVBoxLayout *effectWidgetsBox;
    QList<SoxEffectWidget*> effectWidgets;

private slots:
    void addEffectWidgetClicked();
    void removeEffectWidgetClicked( SoxEffectWidget *widget );
};

#endif // SOXFILTERWIDGET_H
