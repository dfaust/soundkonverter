
#ifndef TAGDATA_H
#define TAGDATA_H

#include <QString>

/**
 * @short All metainformation can be stored in this class
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class TagData
{
public:
    /**
     * Constructor
     */
    TagData( const QString& _artist = QString::null, const QString& _composer = QString::null,
             const QString& _album = QString::null, const QString& _title = QString::null,
             const QString& _genre = QString::null, const QString& _comment = QString::null,
             int _track = 0, int _disc = 0, int _year = 0,
             int _length = 0, int _fileSize = 0, int _bitrate = 0, int _samplingRate = 0 );

    /**
     * Destructor
     */
    virtual ~TagData();

    /** The tags */
    QString artist;
    QString composer;
    QString album;
    QString title;
    QString genre;
    QString comment;
    int track;
    int disc;
    int year;
    float track_gain;
    float album_gain;

    /** The technical information */
    int length;
    int fileSize;
    int bitrate;
    int samplingRate;
};

#endif // TAGDATA_H
