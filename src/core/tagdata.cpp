
#include "tagdata.h"


TagData::TagData( const QString& _artist, const QString& _composer,
             const QString& _album, const QString& _title,
             const QString& _genre, const QString& _comment,
             int _track, int _disc, int _year,
             int _length, int _fileSize, int _bitrate, int _samplingRate )
{
    artist = _artist;
    composer = _composer;
    album = _album;
    title = _title;
    genre = _genre;
    comment = _comment;
    track = _track;
    disc = _disc;
    year = _year;
    length = _length;
    fileSize = _fileSize;
    bitrate = _bitrate;
    samplingRate = _samplingRate;
}

TagData::~TagData()
{}
