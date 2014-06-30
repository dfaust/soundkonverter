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


#include "codecplugin.h"
#include "codecwidget.h"


CodecPluginItem::CodecPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{
    data.length = 0;
}

CodecPluginItem::~CodecPluginItem()
{}


CodecPlugin::CodecPlugin( QObject *parent )
    : BackendPlugin( parent )
{
    lastUsedConversionOptions = 0;
}

CodecPlugin::~CodecPlugin()
{}

QString CodecPlugin::type()
{
    return "codec";
}

CodecWidget *CodecPlugin::deleteCodecWidget( CodecWidget *codecWidget )
{
    if( !codecWidget )
        return 0;

    if( lastUsedConversionOptions )
        delete lastUsedConversionOptions;

    lastUsedConversionOptions = codecWidget->currentConversionOptions();
    if( lastUsedConversionOptions )
    {
        lastUsedConversionOptions->pluginName = name();
        lastUsedConversionOptions->profile = codecWidget->currentProfile();
    }

    delete codecWidget;

    return 0;
}

ConversionOptions *CodecPlugin::conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions *options = new ConversionOptions();
    options->fromXml( conversionOptions, filterOptionsElements );
    return options;
}

