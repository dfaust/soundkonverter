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


#include "timiditycodecglobal.h"

#include "timiditycodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>


TimidityCodecWidget::TimidityCodecWidget()
    : CodecWidget(),
    currentFormat( "midi" )
{}

TimidityCodecWidget::~TimidityCodecWidget()
{}

ConversionOptions *TimidityCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    return options;
}

bool TimidityCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    return true;
}

void TimidityCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString TimidityCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool TimidityCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int TimidityCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }

    return dataRate;
}

