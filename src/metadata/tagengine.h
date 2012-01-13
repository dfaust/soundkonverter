

#ifndef TAGENGINE_H
#define TAGENGINE_H

#include <QString>
#include <QStringList>
#include <KUrl>
#include <qimage.h>

// #include <MediaInfo/MediaInfo.h>

class CoverData : public QObject
{
public:
    /*!
    * This describes the function or content of the picture.
    * copyright: (C) 2002 - 2008 by Scott Wheeler <wheeler@kde.org>
    */
    enum Role {
        //! A type not enumerated below
        Other              = 0x00,
        //! 32x32 PNG image that should be used as the file icon
        FileIcon           = 0x01,
        //! File icon of a different size or format
        OtherFileIcon      = 0x02,
        //! Front cover image of the album
        FrontCover         = 0x03,
        //! Back cover image of the album
        BackCover          = 0x04,
        //! Inside leaflet page of the album
        LeafletPage        = 0x05,
        //! Image from the album itself
        Media              = 0x06,
        //! Picture of the lead artist or soloist
        LeadArtist         = 0x07,
        //! Picture of the artist or performer
        Artist             = 0x08,
        //! Picture of the conductor
        Conductor          = 0x09,
        //! Picture of the band or orchestra
        Band               = 0x0A,
        //! Picture of the composer
        Composer           = 0x0B,
        //! Picture of the lyricist or text writer
        Lyricist           = 0x0C,
        //! Picture of the recording location or studio
        RecordingLocation  = 0x0D,
        //! Picture of the artists during recording
        DuringRecording    = 0x0E,
        //! Picture of the artists during performance
        DuringPerformance  = 0x0F,
        //! Picture from a movie or video related to the track
        MovieScreenCapture = 0x10,
        //! Picture of a large, coloured fish
        ColouredFish       = 0x11,
        //! Illustration related to the track
        Illustration       = 0x12,
        //! Logo of the band or performer
        BandLogo           = 0x13,
        //! Logo of the publisher (record company)
        PublisherLogo      = 0x14
    };

    CoverData( const QByteArray& _data = QByteArray(), const QString& _mimyType = QString::null, Role _role = Other, const QString& _description = QString::null, QObject *parent = 0 );
    virtual ~CoverData();

    QByteArray data;
    QString mimeType;
    Role role;
    QString description;

    static QString roleName( Role role );
};

/**
 * @short All metainformation can be stored in this class
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class TagData : public QObject
{
    Q_OBJECT
public:
    /** Constructor */
//     TagData( const QString& _artist = QString::null, const QString& _composer = QString::null,
//              const QString& _album = QString::null, const QString& _title = QString::null,
//              const QString& _genre = QString::null, const QString& _comment = QString::null,
//              int _track = 0, int _disc = 0, int _discTotal = 0, int _year = 0,
//              int _length = 0, int _fileSize = 0, int _bitrate = 0, int _samplingRate = 0 );
    TagData();

    /** Destructor */
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
    int discTotal;
    int year;
    float track_gain;
    float album_gain;

    /** Covers */
    QList<CoverData*> covers;
    bool coversRead;

    /** The technical information */
    int length;
    int fileSize;
    int bitrate;
    int samplingRate;
};


/**
 * @short Manages everything that has something to do with tags
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class TagEngine
{
public:
    /** Constructor */
    TagEngine();

    /** Destructor */
    virtual ~TagEngine();

    /** A list of all genre */
    QStringList genreList;

//     MediaInfoLib::MediaInfo* MI;

    TagData* readTags( const KUrl& fileName );
    bool writeTags( const KUrl& fileName, TagData *tagData );

    QList<CoverData*> readCovers( const KUrl& fileName );
    bool writeCovers( const KUrl& fileName, QList<CoverData*> covers );
    bool writeCoversToDirectory( const QString& directoryName, QList<CoverData*> covers );

//     bool canWrite( QString format ); // NOTE no const because this string is being modyfied
};

#endif // TAGENGINE_H
