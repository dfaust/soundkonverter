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
#ifndef CONFIGADVANCEDPAGE_H
#define CONFIGADVANCEDPAGE_H

#include "configpagebase.h"

class Config;
class QCheckBox;
class KIntSpinBox;
class KComboBox;

/**
	@author Daniel Faust <hessijames@gmail.com>
 */
class ConfigAdvancedPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    ConfigAdvancedPage( Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    ~ConfigAdvancedPage();

private:
    KComboBox *cPreferredOggVorbisExtension;
    KComboBox *cPreferredVorbisCommentCommentTag;
    KComboBox *cPreferredVorbisCommentTrackTotalTag;
    KComboBox *cPreferredVorbisCommentDiscTotalTag;
    QCheckBox *cUseVFATNames;
    QCheckBox *cEjectCdAfterRip;
    QCheckBox *cWriteLogFiles;
    QCheckBox *cUseSharedMemoryForTempFiles;
    KIntSpinBox *iMaxSizeForSharedMemoryTempFiles;
    QCheckBox *cUsePipes;

    Config *config;

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();

};

#endif
