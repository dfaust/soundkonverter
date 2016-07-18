/*************************************************************************************
 *  Copyright (C) 2016 Leslie Zhai <xiang.zhai@i-soft.com.cn>                        *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#ifndef CONVERSIONOPTIONSMANAGER_H
#define CONVERSIONOPTIONSMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>

class ConversionOptions;
class PluginLoader;

class ConversionOptionsManager : public QObject
{
    Q_OBJECT

public:
    struct ConversionOptionsElement {
        int id;
        int references;
        ConversionOptions *conversionOptions;
    };

    ConversionOptionsManager( PluginLoader *_pluginLoader, QObject *parent );
    ~ConversionOptionsManager();

    int addConversionOptions( ConversionOptions *conversionOptions );
    int increaseReferences( int id );
    const ConversionOptions *getConversionOptions( int id ) const;
    void removeConversionOptions( int id );
    int updateConversionOptions( int id, ConversionOptions *conversionOptions );
    
    QList<int> getAllIds() const;

private:
    PluginLoader *pluginLoader;

    QHash<int, ConversionOptionsElement> elements;
    int idCounter;
};

#endif
