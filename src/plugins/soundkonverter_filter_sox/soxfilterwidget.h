
#ifndef SOXFILTERWIDGET_H
#define SOXFILTERWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;
class QDoubleSpinBox;
class KComboBox;
class KPushButton;
class QHBoxLayout;


class SoxEffectWidget : public QWidget
{
    Q_OBJECT
public:
    SoxEffectWidget( QWidget *parent = 0 );
    ~SoxEffectWidget();

private:
    KComboBox *cEffect;
    QHBoxLayout *widgetsBox;
    QList<QWidget*> widgets;
    KPushButton *pRemove;
    KPushButton *pAdd;

private slots:
    void effectChanged( int index );

    void normalizeVolumeChanged( double value );

};


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
//     QCheckBox *cNormalize;
//     QDoubleSpinBox *dNormalizeVolume;

signals:
    void somethingChanged();
};

#endif // SOXFILTERWIDGET_H
