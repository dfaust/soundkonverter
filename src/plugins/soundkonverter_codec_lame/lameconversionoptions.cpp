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


#include "lamecodecglobal.h"

#include "lameconversionoptions.h"


LameConversionOptions::LameConversionOptions()
    : ConversionOptions()
{
    pluginName = global_plugin_name;
}

LameConversionOptions::~LameConversionOptions()
{}

bool LameConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    LameConversionOptions *other = dynamic_cast<LameConversionOptions*>(_other);

    if( data.preset==other->data.preset && data.preset==Data::UserDefined )
    {
        return ConversionOptions::equals(_other);
    }
    else if( data.preset==other->data.preset && data.preset==Data::SpecifyBitrate )
    {
        return ( equalsBasics(_other) && equalsFilters(_other) && data.presetBitrate==other->data.presetBitrate && data.presetBitrateCbr==other->data.presetBitrateCbr && data.presetFast==other->data.presetFast );
    }
    else if( data.preset==other->data.preset )
    {
        return ( equalsBasics(_other) && equalsFilters(_other) && data.presetFast==other->data.presetFast );
    }
    else
    {
        return false;
    }
}

QDomElement LameConversionOptions::toXml( QDomDocument document )
{
    QDomElement conversionOptions = ConversionOptions::toXml( document );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = document.createElement("data");
    data.setAttribute("preset",LameConversionOptions::data.preset);
    data.setAttribute("presetBitrate",LameConversionOptions::data.presetBitrate);
    data.setAttribute("presetBitrateCbr",LameConversionOptions::data.presetBitrateCbr);
    data.setAttribute("presetFast",LameConversionOptions::data.presetFast);
    encodingOptions.appendChild(data);

    return conversionOptions;
}

bool LameConversionOptions::fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions::fromXml( conversionOptions, filterOptionsElements );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    LameConversionOptions::data.preset = (Data::Preset)data.attribute("preset").toInt();
    LameConversionOptions::data.presetBitrate = data.attribute("presetBitrate").toInt();
    LameConversionOptions::data.presetBitrateCbr = data.attribute("presetBitrateCbr").toInt();
    LameConversionOptions::data.presetFast = data.attribute("presetFast").toInt();

    return true;
}














