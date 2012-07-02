
#ifndef SOXEFFECTWIDGET_H
#define SOXEFFECTWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class KPushButton;
class QHBoxLayout;


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


#endif // SOXEFFECTWIDGET_H
