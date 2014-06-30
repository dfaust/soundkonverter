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
// C++ Interface: conversionoptionsmanager
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONVERSIONOPTIONSMANAGER_H
#define CONVERSIONOPTIONSMANAGER_H

#include <QList>

class ConversionOptions;
class PluginLoader;

/** @author Daniel Faust <hessijames@gmail.com> */
class ConversionOptionsManager
{
public:
    struct ConversionOptionsElement {
        int id;
        int references;
        ConversionOptions *conversionOptions;
    };

    ConversionOptionsManager( PluginLoader *_pluginLoader );
    ~ConversionOptionsManager();

    int addConversionOptions( ConversionOptions *conversionOptions );
    int increaseReferences( int id );
    ConversionOptions *getConversionOptions( int id );
    void removeConversionOptions( int id );
    int updateConversionOptions( int id, ConversionOptions *conversionOptions );
    
    QList<int> getAllIds();

private:
    PluginLoader *pluginLoader;

    QList<ConversionOptionsElement> elements;
    int idCounter;
};

#endif
