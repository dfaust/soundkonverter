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


#include "normalizefilterglobal.h"

#include "normalizefilteroptions.h"
#include "../../core/conversionoptions.h"


NormalizeFilterOptions::NormalizeFilterOptions()
{
    pluginName = global_plugin_name;
}

NormalizeFilterOptions::~NormalizeFilterOptions()
{}

bool NormalizeFilterOptions::equals( FilterOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    NormalizeFilterOptions *other = dynamic_cast<NormalizeFilterOptions*>(_other);

    return ( FilterOptions::equals( _other ) && data.normalize == other->data.normalize );
}

QDomElement NormalizeFilterOptions::toXml( QDomDocument document, const QString elementName )
{
    QDomElement filterOptions = FilterOptions::toXml( document,elementName );
    filterOptions.setAttribute("normalize",data.normalize);

    return filterOptions;
}

bool NormalizeFilterOptions::fromXml( QDomElement filterOptions )
{
    FilterOptions::fromXml( filterOptions );
    data.normalize = filterOptions.attribute("normalize").toInt();

    return true;
}
