
#include "tagengine.h"

#include <QFile>
// #include <KDebug>


#include <fileref.h>
#include <id3v1genres.h> //used to load genre list
#include <mpegfile.h>
#include <tag.h>
#include <tstring.h>
#include <id3v2tag.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <xiphcomment.h>
#include <mpegfile.h>
#include <oggfile.h>
#include <vorbisfile.h>
#include <flacfile.h>
#include <textidentificationframe.h>
#include <xiphcomment.h>
#include <mpcfile.h>


/*//#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v1genres.h> //used to load genre list
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/apetag.h>
#include <taglib/xiphcomment.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/flacfile.h>
#include <taglib/textidentificationframe.h>
#include <taglib/xiphcomment.h>
#include <taglib/mpcfile.h>
// #include "wavpack/wvfile.h"
// // #include "trueaudio/ttafile.h"
*/
/*#ifdef HAVE_MP4V2
#include "metadata/mp4/mp4file.h"
#include "metadata/mp4/mp4tag.h"
#else
#include "metadata/m4a/mp4file.h"
#include "metadata/m4a/mp4itunestag.h"
#endif*/

// TODO COMPILATION tag
// FIXME BPM tag

//using namespace std;
//using namespace MediaInfoLib;

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


TagEngine::TagEngine()
{
    TagLib::StringList genres = TagLib::ID3v1::genreList();
    for( TagLib::StringList::ConstIterator it = genres.begin(), end = genres.end(); it != end; ++it )
        genreList += TStringToQString( (*it) );

    genreList.sort();

    //MediaInfo::Options("Info_Version", "0.7.9;soundKonverter;1.0");
    //MediaInfo::Options("Internet", "No");
//     MI = new MediaInfoLib::MediaInfo;
    //MI->Option("Info_Version", "0.7.9;soundKonverter;1.0");
    //MI->Option("Internet", "No");
}

TagEngine::~TagEngine()
{}

// TagData* TagEngine::readTags( const QString& file ) // LibMediaInfo
// {
//     return 0;
// }

TagData* TagEngine::readTags( const KUrl& fileName ) // TagLib
{
    TagData *tagData = new TagData();

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit() );
    
    if( !fileref.isNull() )
    {
        TagLib::Tag *tag = fileref.tag();

        tagData->track = 0;
        tagData->year = 0;
        tagData->disc = 0;
        tagData->track_gain = 210588; // 0 is a valid value
        tagData->album_gain = 210588;

        if( tag )
        {
            tagData->title = TStringToQString( tag->title() );
            tagData->artist = TStringToQString( tag->artist() );
            tagData->album = TStringToQString( tag->album() );
            tagData->genre = TStringToQString( tag->genre() );
            tagData->comment = TStringToQString( tag->comment() );
            tagData->track = tag->track();
            tagData->year = tag->year();
        }

        TagLib::AudioProperties *audioProperties = fileref.audioProperties();

        if( audioProperties )
        {
            tagData->length = audioProperties->length();
            // TODO read all information
            //tagData->fileSize = ;
            // = audioProperties->channels();
            //tagData->bitrate = audioProperties->bitrate();
            tagData->samplingRate = audioProperties->sampleRate();
        }

        QString disc;
        QString track_gain;
        QString album_gain;
        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            if ( file->ID3v2Tag() )
            {
                if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                    disc = TStringToQString( file->ID3v2Tag()->frameListMap()["TPOS"].front()->toString() );

                if ( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                    tagData->composer = TStringToQString( file->ID3v2Tag()->frameListMap()["TCOM"].front()->toString() );
            }
            if ( file->APETag() )
            {
                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );

                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
            }
        }
        else if ( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ) )
        {
            if ( file->tag() )
            {
                if ( !file->tag()->fieldListMap()[ "COMPOSER" ].isEmpty() )
                    tagData->composer = TStringToQString( file->tag()->fieldListMap()["COMPOSER"].front() );

                if ( !file->tag()->fieldListMap()[ "DISCNUMBER" ].isEmpty() )
                    disc = TStringToQString( file->tag()->fieldListMap()["DISCNUMBER"].front() );

                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );

                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
            }
        }
        else if ( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ) )
        {
            if ( file->xiphComment() )
            {
                if ( !file->xiphComment()->fieldListMap()[ "COMPOSER" ].isEmpty() )
                    tagData->composer = TStringToQString( file->xiphComment()->fieldListMap()["COMPOSER"].front() );

                if ( !file->xiphComment()->fieldListMap()[ "DISCNUMBER" ].isEmpty() )
                    disc = TStringToQString( file->xiphComment()->fieldListMap()["DISCNUMBER"].front() );

                if ( !file->xiphComment()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->xiphComment()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );

                if ( !file->xiphComment()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->xiphComment()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
            }

            /*if ( file->tag() )
            {
                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );

                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
            }*/
        }
        /*else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                tagData->composer = TStringToQString( mp4tag->composer() );

                disc = QString::number( mp4tag->disk() );
            }
        }*/
        /*else if ( TagLib::MPC::File *file = dynamic_cast<TagLib::MPC::File *>( fileref.file() ) )
        {
            if ( file->APETag() )
            {
                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );

                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
            }
        }*/
/*        else if ( TagLib::WavPack::File *file = dynamic_cast<TagLib::WavPack::File *>( fileref.file() ) )
        {
            if ( file->APETag() )
            {
                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );

                if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
            }
        }*/
        /*else if ( TagLib::TTA::File *file = dynamic_cast<TagLib::TTA::File *>( fileref.file() ) ) // NOTE writing works, but reading not
        {
            if ( file->ID3v2Tag() )
            {
                if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                    disc = TStringToQString( file->ID3v2Tag()->frameListMap()["TPOS"].front()->toString() );

                if ( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                    tagData->composer = TStringToQString( file->ID3v2Tag()->frameListMap()["TCOM"].front()->toString() );
            }
        }*/

        if( !disc.isEmpty() )
        {
            int i = disc.indexOf('/');
            if( i != -1 )
                // disc.right( i ).toInt() is total number of discs, we don't use this at the moment
                tagData->disc = disc.left( i ).toInt();
            else
                tagData->disc = disc.toInt();
        }

        if( !track_gain.isEmpty() )
        {
            int i = track_gain.indexOf(' ');
            if( i != -1 )
                tagData->track_gain = track_gain.left( i ).toFloat();
            else
                tagData->track_gain = track_gain.toFloat();
        }

        if( !album_gain.isEmpty() )
        {
            int i = album_gain.indexOf(' ');
            if( i != -1 )
                tagData->album_gain = album_gain.left( i ).toFloat();
            else
                tagData->album_gain = album_gain.toFloat();
        }

        return tagData;
    }

    return 0;
}

bool TagEngine::writeTags( const KUrl& fileName, TagData *tagData )
{
    if( !tagData ) tagData = new TagData();

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit(), false );

    //Set default codec to UTF-8 (see bugs 111246 and 111232)
    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding( TagLib::String::UTF8 );

    if ( !fileref.isNull() )
    {
        TagLib::Tag* tag = fileref.tag();
        if ( tag )
        {
            tag->setTitle( TagLib::String(tagData->title.toUtf8().data(), TagLib::String::UTF8) );
            tag->setArtist( TagLib::String(tagData->artist.toUtf8().data(), TagLib::String::UTF8) );
            tag->setAlbum( TagLib::String(tagData->album.toUtf8().data(), TagLib::String::UTF8) );
            tag->setTrack( tagData->track );
            tag->setYear( tagData->year );
            tag->setComment( TagLib::String(tagData->comment.toUtf8().data(), TagLib::String::UTF8) );
            tag->setGenre( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
        }
        else
        {
            return false;
        }

        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            if ( file->ID3v2Tag() )
            {
                if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TPOS" ].front()->setText( TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TPOS", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8) );
                    file->ID3v2Tag()->addFrame( frame );
                }

                if ( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TCOM" ].front()->setText( TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TCOM", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
                    file->ID3v2Tag()->addFrame( frame );
                }

                // TODO check if hacks are neccessary for taglib 1.5
                // HACK sets the id3v2 genre tag as string
                if ( !file->ID3v2Tag()->frameListMap()[ "TCON" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TCON" ].front()->setText( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TCON", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
                    file->ID3v2Tag()->addFrame( frame );
                }

                // HACK sets the id3v2 year tag
                if ( !file->ID3v2Tag()->frameListMap()[ "TYER" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TYER" ].front()->setText( TagLib::String(QString::number(tagData->year).toUtf8().data(), TagLib::String::UTF8) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TYER", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( TagLib::String(QString::number(tagData->year).toUtf8().data(), TagLib::String::UTF8) );
                    file->ID3v2Tag()->addFrame( frame );
                }
            }
        }
        else if ( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ) )
        {
            if ( file->tag() )
            {
                file->tag()->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );

                file->tag()->addField( "DISCNUMBER", TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8), true );
            }
        }
        else if ( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ) )
        {
            if ( file->xiphComment() )
            {
                file->xiphComment()->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );

                file->xiphComment()->addField( "DISCNUMBER", TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8), true );
            }
        }
        /*else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                mp4tag->setComposer( QStringToTString( tagData->composer ) );

                mp4tag->setDisk( QString::number(tagData->disc) );
            }
        }
        /*if ( TagLib::TTA::File *file = dynamic_cast<TagLib::TTA::File *>( fileref.file() ) ) // NOTE writing works, but reading not
        {
            if ( file->ID3v2Tag() )
            {
                if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TPOS" ].front()->setText( QStringToTString( QString::number(tagData->disc) ) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TPOS", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( QStringToTString( QString::number(tagData->disc) ) );
                    file->ID3v2Tag()->addFrame( frame );
                }

                if ( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TCOM" ].front()->setText( QStringToTString( tagData->composer ) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TCOM", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( QStringToTString( tagData->composer ) );
                    file->ID3v2Tag()->addFrame( frame );
                }
            }
        }*/

        return fileref.save();
    }
    return false;
}

// bool TagEngine::canWrite( QString format )
// {
//     format = format.lower();
// 
//     if( format == "ogg" ||
//         format == "flac" || format == "fla" ||
//         format == "mp3" || // TODO mp2 ?
//         format == "mpc" ||
//         format == "aac" ||
//         format == "ape" || format == "mac" ||
//         format == "aa" ||
//         format == "m4a" || format == "m4b" || format == "m4p" || format == "mp4" || format == "m4v" || format == "mp4v" ||
//         format == "ra" || format == "rv" || format == "rm" || format == "rmj" || format == "rmvb" ||
//         format == "wma" || format == "asf" )
//     {
//         return true;
//     }
//     else {
//         return false;
//     }
// }

