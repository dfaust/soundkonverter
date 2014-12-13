
#ifndef SOXEFFECTWIDGET_H
#define SOXEFFECTWIDGET_H

#include "../../core/codecwidget.h"
#include "soxfilteroptions.h"

class QComboBox;
class QPushButton;
class QHBoxLayout;


class SoxEffectWidget : public QWidget
{
    Q_OBJECT
public:
    SoxEffectWidget( QWidget *parent = 0 );
    ~SoxEffectWidget();

    void setRemoveButtonShown( bool shown );
    void setAddButtonShown( bool shown );

    SoxFilterOptions::EffectData currentEffectOptions();
    bool setEffectOptions( SoxFilterOptions::EffectData effectData );

private:
    QComboBox *cEffect;
    QHBoxLayout *widgetsBox;
    QList<QWidget*> widgets;
    QPushButton *pRemove;
    QPushButton *pAdd;

private slots:
    void removeClicked();
    void effectChanged( int index );

    void normalizeVolumeChanged( double value );

signals:
    void addEffectWidgetClicked();
    void removeEffectWidgetClicked( SoxEffectWidget *widget );

    void optionsChanged();
};


#endif // SOXEFFECTWIDGET_H
