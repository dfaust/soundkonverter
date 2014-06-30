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


#include "filterplugin.h"
#include "codecwidget.h"


//
// class FilterPluginItem
//
////////////////////

FilterPluginItem::FilterPluginItem( QObject *parent )
    : CodecPluginItem( parent )
{}

FilterPluginItem::~FilterPluginItem()
{}


//
// class FilterPlugin
//
////////////////////

FilterPlugin::FilterPlugin( QObject *parent )
    : CodecPlugin( parent )
{
    lastUsedFilterOptions = 0;
}

FilterPlugin::~FilterPlugin()
{}

QString FilterPlugin::type()
{
    return "filter";
}

FilterWidget *FilterPlugin::deleteFilterWidget( FilterWidget *filterWidget )
{
    if( !filterWidget )
        return 0;

    if( lastUsedFilterOptions )
        delete lastUsedFilterOptions;

    lastUsedFilterOptions = filterWidget->currentFilterOptions();
    delete filterWidget;

    return 0;
}

FilterOptions *FilterPlugin::filterOptionsFromXml( QDomElement filterOptions )
{
    FilterOptions *options = new FilterOptions();
    options->fromXml( filterOptions );
    return options;
}

