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


#ifndef OPUSTOOLSCONVERSIONOPTIONS_H
#define OPUSTOOLSCONVERSIONOPTIONS_H

#include "../../core/conversionoptions.h"

class OpusToolsConversionOptions : public ConversionOptions
{
public:
    OpusToolsConversionOptions();
    ~OpusToolsConversionOptions();

    bool equals( ConversionOptions *_other );
    QDomElement toXml( QDomDocument document );
    bool fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

    struct Data {
        float floatBitrate;
    } data;
};

#endif // OPUSTOOLSCONVERSIONOPTIONS_H