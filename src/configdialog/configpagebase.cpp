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
// C++ Implementation: configpagebase
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configpagebase.h"

#include <QApplication>


ConfigPageBase::ConfigPageBase( QWidget *parent )
 : QWidget( parent )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    spacingOffset = 2 * fontHeight;
    spacingSmall  = 0.5*fontHeight;
    spacingMedium =     fontHeight;
    spacingBig    = 2 * fontHeight;
}

ConfigPageBase::~ConfigPageBase()
{}

void ConfigPageBase::resetDefaults()
{}

void ConfigPageBase::saveSettings()
{}
