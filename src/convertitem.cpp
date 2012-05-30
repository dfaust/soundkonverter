
#include "convertitem.h"
#include "filelistitem.h"

#include <KStandardDirs>
#include <QFile>


ConvertItem::ConvertItem( FileListItem *item )
{
    fileListItem = item;
    getTime = convertTime = decodeTime = encodeTime = replaygainTime = 0.0f;
    encodePlugin = 0;
    convertID = -1;
    replaygainID = -1;
    take = 0;
    lastTake = 0;
    filterNumber = 0;
    killed = false;
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
    } while( QFile::exists(tempUrl) );

    return KUrl(tempUrl);
}

void ConvertItem::updateTimes()
{
    getTime = ( mode & ConvertItem::get ) ? 0.8f : 0.0f;            // TODO file size? connection speed?
    convertTime = ( mode & ConvertItem::convert ) ? 1.4f : 0.0f;    // NOTE either convert OR decode & encode is used --- or only replay gain
    if( fileListItem && fileListItem->track == -1 )
    {
        decodeTime = ( mode & ConvertItem::decode ) ? 0.4f : 0.0f;
        encodeTime = ( mode & ConvertItem::encode ) ? 1.0f : 0.0f;
    }
    else
    {
        decodeTime = ( mode & ConvertItem::decode ) ? 1.0f : 0.0f;  // TODO drive speed?
        encodeTime = ( mode & ConvertItem::encode ) ? 0.4f : 0.0f;
    }
    float filterSum = 0.0f;
    for( int i=0; i<filterCount; i++ )
    {
        filterTimes.append( 0.6f );
        filterSum += 0.6f;
    }
    replaygainTime = ( mode & ConvertItem::replaygain ) ? 0.2f : 0.0f;

    const float sum = getTime + convertTime + decodeTime + encodeTime + replaygainTime + filterSum;

    const float length = fileListItem ? fileListItem->length : 0;
    getTime *= length/sum;
    convertTime *= length/sum;
    decodeTime *= length/sum;
    encodeTime *= length/sum;
    replaygainTime *= length/sum;
    for( int i=0; i<filterTimes.count(); i++ )
    {
        filterTimes[i] *= length/sum;
    }

    finishedTime = 0.0f;
    switch( state )
    {
        case ConvertItem::convert:
        {
            if( mode & ConvertItem::get )
            {
                finishedTime += getTime;
            }
            break;
        }
        case ConvertItem::decode:
        {
            if( mode & ConvertItem::get )
            {
                finishedTime += getTime;
            }
            break;
        }
        case ConvertItem::filter:
        {
            if( mode & ConvertItem::get )
            {
                finishedTime += getTime;
            }
            if( mode & ConvertItem::decode )
            {
                finishedTime += decodeTime;
            }
            break;
        }
        case ConvertItem::encode:
        {
            if( mode & ConvertItem::get )
            {
                finishedTime += getTime;
            }
            if( mode & ConvertItem::decode )
            {
                finishedTime += decodeTime;
            }
            if( mode & ConvertItem::filter )
            {
                for( int i=0; i<filterNumber; i++ )
                {
                    finishedTime += filterTimes.at(i);
                }
            }
            break;
        }
        case ConvertItem::replaygain:
        {
            if( mode & ConvertItem::get )
            {
                finishedTime += getTime;
            }
            if( mode & ConvertItem::convert )
            {
                finishedTime += convertTime;
            }
            if( mode & ConvertItem::decode )
            {
                finishedTime += decodeTime;
            }
            if( mode & ConvertItem::filter )
            {
                for( int i=0; i<filterNumber; i++ )
                {
                    finishedTime += filterTimes.at(i);
                }
            }
            if( mode & ConvertItem::encode )
            {
                finishedTime += encodeTime;
            }
            break;
        }
        default:
            break;
    }
}

