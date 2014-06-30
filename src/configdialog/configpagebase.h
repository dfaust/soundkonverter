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
// C++ Interface: configpagebase
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGPAGEBASE_H
#define CONFIGPAGEBASE_H

#include <QWidget>

class Config;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigPageBase : public QWidget
{
    Q_OBJECT
public:
    ConfigPageBase( QWidget *parent=0 );
    virtual ~ConfigPageBase();

    int spacingOffset;
    int spacingSmall;
    int spacingMedium;
    int spacingBig;

public slots:
    virtual void resetDefaults();
    virtual void saveSettings();

signals:
    void configChanged( bool state = true );

};

#endif
