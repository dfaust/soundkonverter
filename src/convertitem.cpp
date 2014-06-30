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


#include "convertitem.h"
#include "filelistitem.h"

#include <KStandardDirs>
#include <QFile>


ConvertItem::ConvertItem( FileListItem *item )
    : fileListItem( item )
{
    getTime = replaygainTime = 0.0f;

    backendPlugin = 0;
    backendID = -1;

    take = 0;
    lastTake = 0;

    conversionPipesStep = -1;

    killed = false;
    internalReplayGainUsed = false;
}

ConvertItem::~ConvertItem()
{}

KUrl ConvertItem::generateTempUrl( const QString& trunk, const QString& extension, bool useSharedMemory )
{
    QString tempUrl;
    int i=0;
    do {
        if( useSharedMemory )
        {
            tempUrl = "/dev/shm/" + QString("soundkonverter_temp_%1_%2_%3.%4").arg(trunk).arg(logID).arg(i).arg(extension);
        }
        else
        {
            tempUrl = KStandardDirs::locateLocal( "tmp", QString("soundkonverter_temp_%1_%2_%3.%4").arg(trunk).arg(logID).arg(i).arg(extension) );
        }
        i++;
    } while( tempConvertUrls.contains(KUrl(tempUrl)) || QFile::exists(tempUrl) );

    return KUrl(tempUrl);
}

void ConvertItem::updateTimes()
{
    float totalTime = 0.0f;
    getTime = ( mode & ConvertItem::get ) ? 0.8f : 0.0f;                        // TODO file size? connection speed?
    totalTime += getTime;
    if( conversionPipes.count() > take )
    {
        foreach( const ConversionPipeTrunk trunk, conversionPipes.at(take).trunks )
        {
            float time = 0.0f;

            if( trunk.codecFrom == "audio cd" )
                time += 1.0f;
            else if( trunk.codecFrom != "wav" )
                time += 0.4f;

            if( trunk.codecTo != "wav" )
                time += 1.0f;

            if( trunk.codecFrom == "wav" && trunk.codecTo == "wav" )
                time += 0.4f;

            totalTime += time;

            convertTimes.append( time );
        }
    }
    replaygainTime = ( mode & ConvertItem::replaygain ) ? 0.2f : 0.0f;
    totalTime += replaygainTime;

    const float length = fileListItem ? fileListItem->length : 200.0f;

    getTime *= length/totalTime;
    for( int i=0; i<convertTimes.count(); i++ )
    {
        convertTimes[i] *= length/totalTime;
    }
    replaygainTime *= length/totalTime;

    finishedTime = 0.0f;
//     switch( state )
//     {
//         case ConvertItem::get:
//             break;
//         case ConvertItem::convert:
//             finishedTime += getTime;
//             break;
//         case ConvertItem::rip:
//             finishedTime += getTime;
//             break;
//         case ConvertItem::decode:
//             finishedTime += getTime;
//             break;
//         case ConvertItem::filter:
//             finishedTime += getTime;
//             finishedTime += convertTimes.at(0);
//             break;
//         case ConvertItem::encode:
//         case ConvertItem::replaygain:
//     }
}

