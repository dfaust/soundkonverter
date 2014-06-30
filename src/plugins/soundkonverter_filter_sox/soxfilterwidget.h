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
