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

//
// C++ Interface: configgeneralpage
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGGENERALPAGE_H
#define CONFIGGENERALPAGE_H

#include "configpagebase.h"

class Config;
class QCheckBox;
class KComboBox;
class KIntSpinBox;
class KLineEdit;
class KPushButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigGeneralPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /** Default Constructor */
    ConfigGeneralPage( Config *_config, QWidget *parent=0 );

    /** Default Destructor */
    ~ConfigGeneralPage();

private:
    KComboBox *cStartTab;
    KComboBox *cDefaultProfile;
    KComboBox *cDefaultFormat;
//     KComboBox *cPriority;
//     QStringList sPriority;
    KComboBox *cConflictHandling;
    KIntSpinBox *iNumFiles;
    QCheckBox *cWaitForAlbumGain;
    KComboBox *cReplayGainGrouping;
    KIntSpinBox *iNumReplayGainFiles;

    Config *config;

//     int profileIndex( const QString& string );
//     int formatIndex( const QString& string );

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();
//     void selectDir();
    void profileChanged();


};

#endif
