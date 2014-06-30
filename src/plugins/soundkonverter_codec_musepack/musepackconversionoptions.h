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


#ifndef MUSEPACKCONVERSIONOPTIONS_H
#define MUSEPACKCONVERSIONOPTIONS_H

#include "../../core/conversionoptions.h"

class MusePackConversionOptions : public ConversionOptions
{
public:
    MusePackConversionOptions();
    ~MusePackConversionOptions();

    bool equals( ConversionOptions *_other );
    QDomElement toXml( QDomDocument document );
    bool fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

    struct Data {
        enum Preset {
            Telephone = 0,
            Thumb = 1,
            Radio = 2,
            Standard = 3,
            Extreme = 4,
            Insane = 5,
            Braindead = 6,
            UserDefined = 7
        } preset;
    } data;
};

#endif // MUSEPACKCONVERSIONOPTIONS_H