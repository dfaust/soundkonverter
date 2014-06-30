/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


#ifndef SOXEFFECTWIDGET_H
#define SOXEFFECTWIDGET_H

#include "../../core/codecwidget.h"
#include "soxfilteroptions.h"

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

    SoxFilterOptions::EffectData currentEffectOptions();
    bool setEffectOptions( SoxFilterOptions::EffectData effectData );

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

    void optionsChanged();
};


#endif // SOXEFFECTWIDGET_H
