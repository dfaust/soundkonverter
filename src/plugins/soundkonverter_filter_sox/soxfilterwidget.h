
#ifndef SOXFILTERWIDGET_H
#define SOXFILTERWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;
class QDoubleSpinBox;
class KComboBox;

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
    QCheckBox *cNormalize;
    QDoubleSpinBox *dNormalizeVolume;

private slots:
    void normalizeVolumeChanged( double value );

signals:
    void somethingChanged();
};

#endif // SOXFILTERWIDGET_H
