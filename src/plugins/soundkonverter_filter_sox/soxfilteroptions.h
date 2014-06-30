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


#ifndef SOXFILTEROPTIONS_H
#define SOXFILTEROPTIONS_H

#include "../../core/conversionoptions.h"


class SoxFilterOptions : public FilterOptions
{
public:
    SoxFilterOptions();
    ~SoxFilterOptions();

    bool equals( FilterOptions *_other );
    QDomElement toXml( QDomDocument document, const QString elementName );
    bool fromXml( QDomElement filterOptions );

    struct EffectData {
        QString effectName;
        QVariantList data;
    };

    struct Data {
        int sampleRate; // 0 if disabled
        int sampleSize; // 0 if disabled
        short channels; // 0 if disabled
        QList<EffectData> effects;
    } data;
};

#endif // SOXFILTEROPTIONS_H
