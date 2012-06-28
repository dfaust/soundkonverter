
#ifndef SOXFILTERWIDGET_H
#define SOXFILTERWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;
class QDoubleSpinBox;
class KComboBox;
class KPushButton;
class QHBoxLayout;
class QVBoxLayout;


class SoxEffectWidget : public QWidget
{
    Q_OBJECT
public:
    SoxEffectWidget( QWidget *parent = 0 );
    ~SoxEffectWidget();

    void setRemoveButtonShown( bool shown );
    void setAddButtonShown( bool shown );

private:
    KComboBox *cEffect;
    QHBoxLayout *widgetsBox;
    QList<QWidget*> widgets;
    KPushButton *pRemove;
    KPushButton *pAdd;

private slots:
    void removeClicked();
    void effectChanged( int index );

    void normalizeVolumeChanged( double value );

signals:
    void addEffectWidgetClicked();
    void removeEffectWidgetClicked( SoxEffectWidget *widget );

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
    QVBoxLayout *effectWidgetsBox;
    QList<SoxEffectWidget*> effectWidgets;

private slots:
    void addEffectWidgetClicked();
    void removeEffectWidgetClicked( SoxEffectWidget *widget );

signals:
    void somethingChanged();

};

#endif // SOXFILTERWIDGET_H
