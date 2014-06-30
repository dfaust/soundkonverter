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
#ifndef CONFIGCOVERARTPAGE_H
#define CONFIGCOVERARTPAGE_H

#include "configpagebase.h"

class Config;
class QRadioButton;
class QLabel;
class QAbstractButton;
class KLineEdit;

/**
	@author Daniel Faust <hessijames@gmail.com>
 */
class ConfigCoverArtPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    ConfigCoverArtPage( Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    ~ConfigCoverArtPage();

private:
    QRadioButton *rWriteCoversAlways;
    QRadioButton *rWriteCoversAuto;
    QRadioButton *rWriteCoversNever;

    QLabel       *lWriteCoverName;
    QRadioButton *rWriteCoverNameTitle;
    QRadioButton *rWriteCoverNameDefault;
    QLabel       *lWriteCoverNameDefaultLabel;
    KLineEdit    *lWriteCoverNameDefaultEdit;

//     QCheckBox *cCopyCover;
//     QCheckBox *cEmbedCover;
//     QListView *lCoverList;

    Config *config;

private slots:
    void somethingChanged();
    void writeCoversChanged( QAbstractButton *button );

public slots:
    void resetDefaults();
    void saveSettings();

};

#endif
