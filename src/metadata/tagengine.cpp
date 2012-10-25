
#include "tagengine.h"
#include "MetaReplayGain.h"
#include "config.h"

#include <QFile>
#include <QDir>
#include <QBuffer>

#include <KLocale>

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
#include <asffile.h>
#include <textidentificationframe.h>
#include <attachedpictureframe.h>
#include <xiphcomment.h>
#include <mpcfile.h>
#include <mp4tag.h>
#include <mp4file.h>

// TODO COMPILATION tag
// TODO BPM tag


// Taglib added support for FLAC pictures in 1.7.0
#if (TAGLIB_MAJOR_VERSION > 1) || (TAGLIB_MAJOR_VERSION == 1 && TAGLIB_MINOR_VERSION >= 7)
# define TAGLIB_HAS_FLAC_PICTURELIST
# define TAGLIB_HAS_ASF_PICTURE
#endif


CoverData::CoverData( const QByteArray& _data, const QString& _mimyType, Role _role, const QString& _description, QObject *parent )
        : QObject( parent )
{
    data = _data;
    mimeType = _mimyType;
    role = _role;
    description = _description;
}

CoverData::~CoverData()
{}

QString CoverData::roleName( Role role )
{
    switch( role )
    {
        case Other:
            return i18nc("cover type","Unclassified");
        case FileIcon:
            return i18nc("cover type","32x32 PNG file icon");
        case OtherFileIcon:
            return i18nc("cover type","File icon");
        case FrontCover:
            return i18nc("cover type","Front cover");
        case BackCover:
            return i18nc("cover type","Back cover");
        case LeafletPage:
            return i18nc("cover type","Leaflet page");
        case Media:
            return i18nc("cover type","CD photo");
        case LeadArtist:
            return i18nc("cover type","Lead artist picture");
        case Artist:
            return i18nc("cover type","Artist picture");
        case Conductor:
            return i18nc("cover type","Conductor picture");
        case Band:
            return i18nc("cover type","Band picture");
        case Composer:
            return i18nc("cover type","Composer picture");
        case Lyricist:
            return i18nc("cover type","Lyricist picture");
        case RecordingLocation:
            return i18nc("cover type","Picture of the recording location");
        case DuringRecording:
            return i18nc("cover type","Picture of the artists during recording");
        case DuringPerformance:
            return i18nc("cover type","Picture of the artists during performance");
        case MovieScreenCapture:
            return i18nc("cover type","Picture from a video");
        case ColouredFish:
            return i18nc("cover type","Picture of a large, coloured fish");
        case Illustration:
            return i18nc("cover type","Illustration related to the track");
        case BandLogo:
            return i18nc("cover type","Band logo");
        case PublisherLogo:
            return i18nc("cover type","Publisher logo");
    }

    return QString();
}


// TagData::TagData( const QString& _artist, const QString& _composer,
//              const QString& _album, const QString& _title,
//              const QString& _genre, const QString& _comment,
//              int _track, int _disc, int _discTotal, int _year,
//              int _length, int _fileSize, int _bitrate, int _samplingRate )
// {
//     artist = _artist;
//     composer = _composer;
//     album = _album;
//     title = _title;
//     genre = _genre;
//     comment = _comment;
//     track = _track;
//     disc = _disc;
//     discTotal = _discTotal;
//     year = _year;
//     length = _length;
//     fileSize = _fileSize;
//     bitrate = _bitrate;
//     samplingRate = _samplingRate;
//     coversRead = false;
// }

TagData::TagData()
{
    artist = QString();
    composer = QString();
    album = QString();
    title = QString();
    genre = QString();
    comment = QString();
    track = 0;
    disc = 0;
    discTotal = 0;
    year = 0;
    length = 0;
    fileSize = 0;
    bitrate = 0;
    samplingRate = 0;
    coversRead = false;
}

TagData::~TagData()
{
    qDeleteAll( covers );
    covers.clear();
}


TagEngine::TagEngine( Config *_config )
    : config( _config )
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

        Meta::ReplayGainTagMap replayGainTags = Meta::readReplayGainTags( fileref );
        if( replayGainTags.contains(Meta::ReplayGain_Track_Gain) )
            tagData->track_gain = replayGainTags[ Meta::ReplayGain_Track_Gain ];
        if( replayGainTags.contains(Meta::ReplayGain_Album_Gain) )
            tagData->album_gain = replayGainTags[ Meta::ReplayGain_Album_Gain ];

        QString disc;
//         QString track_gain;
//         QString album_gain;
        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            // TXXX : TagLib::ID3v2::UserTextIdentificationFrame
            // TBPM : BPM
            // TPE2 : Album artist
            // TCMP : Compilation (true,1 vs. false,0)
            // POPM : rating, playcount
            // APIC : TagLib::ID3v2::AttachedPictureFrame
            // UFID : TagLib::ID3v2::UniqueFileIdentifierFrame

            if ( file->ID3v2Tag() )
            {
                if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                    disc = TStringToQString( file->ID3v2Tag()->frameListMap()["TPOS"].front()->toString() );

                if ( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                    tagData->composer = TStringToQString( file->ID3v2Tag()->frameListMap()["TCOM"].front()->toString() );

//                 TagLib::ID3v2::FrameList apic_frames = file->ID3v2Tag()->frameListMap()["APIC"];
//                 if( !apic_frames.isEmpty() )
//                 {
//                     TagLib::ID3v2::AttachedPictureFrame* pic = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(apic_frames.front());
//
//                     tagData->cover.loadFromData( (const uchar*) pic->picture().data(), pic->picture().size() );
//                 }
            }
//             if ( file->APETag() )
//             {
//                 if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
//                     track_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );
//
//                 if ( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
//                     album_gain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
//             }
        }
        else if ( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ) )
        {
            // ALBUMARTIST
            // BPM
            // COMPILATION (1 vs. 0)

            if ( file->tag() )
            {
                if ( !file->tag()->fieldListMap()[ "COMPOSER" ].isEmpty() )
                    tagData->composer = TStringToQString( file->tag()->fieldListMap()["COMPOSER"].front() );

                if ( !file->tag()->fieldListMap()[ "DISCNUMBER" ].isEmpty() )
                    disc = TStringToQString( file->tag()->fieldListMap()["DISCNUMBER"].front() );

//                 if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
//                     track_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );
//
//                 if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
//                     album_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
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

//                 if ( !file->xiphComment()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
//                     track_gain = TStringToQString( file->xiphComment()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );
//
//                 if ( !file->xiphComment()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
//                     album_gain = TStringToQString( file->xiphComment()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
            }

            /*if ( file->tag() )
            {
                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    track_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_TRACK_GAIN"].front() );

                if ( !file->tag()->fieldListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    album_gain = TStringToQString( file->tag()->fieldListMap()["REPLAYGAIN_ALBUM_GAIN"].front() );
            }*/
        }
        else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            // \xA9wrt : Composer
            // aART : Album artist
            // tmpo : BPM
            // disk
            // cpil : Compilation (true vs. false)

            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                TagLib::MP4::ItemListMap map = mp4tag->itemListMap();
                for( TagLib::MP4::ItemListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( it->first == "\xA9wrt" )
                    {
                        tagData->composer = TStringToQString( it->second.toStringList().front() );
                    }
                    else if( it->first == "disk" )
                    {
                        tagData->disc = it->second.toIntPair().first;
                        tagData->discTotal = it->second.toIntPair().second;
                    }
//                     else if( it->first == "\xA9lyr" )
//                     {
//                         tagData->comment = TStringToQString( it->second.toStringList().front() );
//                     }
                }
            }
        }
        else if ( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File *>( fileref.file() ) )
        {
            // WM/Composer : Composer
            // WM/AlbumTitle : Album artist
            // WM/BeatsPerMinute : BPM

            TagLib::ASF::Tag *asftag = dynamic_cast< TagLib::ASF::Tag * >( file->tag() );
            if( asftag )
            {
                TagLib::ASF::AttributeListMap map = asftag->attributeListMap();
                for( TagLib::ASF::AttributeListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( !it->second.size() )
                        continue;

                    if( it->first == "WM/Composer" )
                    {
                        tagData->composer = TStringToQString( it->second[0].toString() );
                    }
                }
            }
        }
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
            {
                tagData->disc = disc.left( i ).toInt();
                tagData->discTotal = disc.right( disc.count() - i - 1 ).toInt();
            }
            else
            {
                tagData->disc = disc.toInt();
                tagData->discTotal = 0;
            }
        }

//         if( !track_gain.isEmpty() )
//         {
//             int i = track_gain.indexOf(' ');
//             if( i != -1 )
//                 tagData->track_gain = track_gain.left( i ).toFloat();
//             else
//                 tagData->track_gain = track_gain.toFloat();
//         }
//
//         if( !album_gain.isEmpty() )
//         {
//             int i = album_gain.indexOf(' ');
//             if( i != -1 )
//                 tagData->album_gain = album_gain.left( i ).toFloat();
//             else
//                 tagData->album_gain = album_gain.toFloat();
//         }

        return tagData;
    }

    return 0;
}

bool TagEngine::writeTags( const KUrl& fileName, TagData *tagData )
{
    if( !tagData )
        tagData = new TagData();

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

        QString disc;
        if( tagData->disc > 0 )
        {
            disc = QString::number(tagData->disc);
            if( tagData->discTotal > 0 )
            {
                disc += "/" + QString::number(tagData->discTotal);
            }
        }

        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            if ( file->ID3v2Tag() )
            {
                if( !disc.isEmpty() )
                {
                    if ( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                    {
                        file->ID3v2Tag()->frameListMap()[ "TPOS" ].front()->setText( TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8) );
                    }
                    else
                    {
                        TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TPOS", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                        frame->setText( TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8) );
                        file->ID3v2Tag()->addFrame( frame );
                    }
                }

                if( !tagData->composer.isEmpty() )
                {
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
                }

                // TODO check if hacks are neccessary for taglib 1.5
                // HACK sets the id3v2 genre tag as string
                if( !tagData->genre.isEmpty() )
                {
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
                }

                // HACK sets the id3v2 year tag
                if( tagData->year > 0 )
                {
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
        }
        else if ( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ) )
        {
            if ( file->tag() )
            {
                if( !tagData->composer.isEmpty() )
                    file->tag()->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );

                if( !disc.isEmpty() )
                    file->tag()->addField( "DISCNUMBER", TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8), true );
            }
        }
        else if ( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ) )
        {
            if ( file->xiphComment() )
            {
                if( !tagData->composer.isEmpty() )
                    file->xiphComment()->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );

                if( !disc.isEmpty() )
                    file->xiphComment()->addField( "DISCNUMBER", TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8), true );
            }
        }
        else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                if( !tagData->composer.isEmpty() )
                    mp4tag->itemListMap()["\xA9wrt"] = TagLib::StringList(TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8));

                if( tagData->disc > 0 )
                    mp4tag->itemListMap()["disk"] = TagLib::MP4::Item( tagData->disc, tagData->discTotal );
            }
        }
        else if ( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File *>( fileref.file() ) )
        {
            // WM/Composer : Composer
            // WM/AlbumTitle : Album artist
            // WM/BeatsPerMinute : BPM

            TagLib::ASF::Tag *asftag = dynamic_cast< TagLib::ASF::Tag * >( file->tag() );
            if( asftag )
            {
                if( !tagData->composer.isEmpty() )
                    asftag->addAttribute( TagLib::String("WM/Composer"), TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
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

QList<CoverData*> TagEngine::readCovers( const KUrl& fileName ) // TagLib
{
    QList<CoverData*> covers;

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit() );

    if( !fileref.isNull() )
    {
        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            // TXXX : TagLib::ID3v2::UserTextIdentificationFrame
            // TBPM : BPM
            // TPE2 : Album artist
            // TCMP : Compilation (true,1 vs. false,0)
            // POPM : rating, playcount
            // APIC : TagLib::ID3v2::AttachedPictureFrame
            // UFID : TagLib::ID3v2::UniqueFileIdentifierFrame

            if ( file->ID3v2Tag() )
            {
                TagLib::ID3v2::FrameList apic_frames = file->ID3v2Tag()->frameListMap()["APIC"];

                for( TagLib::ID3v2::FrameList::ConstIterator iter = apic_frames.begin(); iter != apic_frames.end(); ++iter )
                {
                    TagLib::ID3v2::AttachedPictureFrame *pic =
                            dynamic_cast< TagLib::ID3v2::AttachedPictureFrame * >( *iter );

                    QByteArray image_data( pic->picture().data(), pic->picture().size() );
                    CoverData *newCover = new CoverData( image_data, TStringToQString(pic->mimeType()), CoverData::Role(pic->type()), TStringToQString(pic->description()) );
                    covers.append( newCover );
                }
            }
        }
        else if ( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ) )
        {
            // ALBUMARTIST
            // BPM
            // COMPILATION (1 vs. 0)

            if ( file->tag() )
            {
                TagLib::Ogg::FieldListMap map = file->tag()->fieldListMap();

                // Ogg lacks a definitive standard for embedding cover art, but it seems
                // b64 encoding a field called COVERART is the general convention

                TagLib::StringList coverArtValue;
                TagLib::StringList mimeTypeValue;
                TagLib::StringList descriptionValue;

                for( TagLib::Ogg::FieldListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    const TagLib::String key = it->first;
                    const TagLib::StringList value = it->second;

                    if( key == TagLib::String("COVERART") )
                    {
                        coverArtValue = value;
                    }
                    else if( key == TagLib::String("COVERARTMIME") )
                    {
                        mimeTypeValue = value;
                    }
                    else if( key == TagLib::String("COVERARTDESCRIPTION") )
                    {
                        descriptionValue = value;
                    }
                }

                for( uint i=0; i<coverArtValue.size(); i++ )
                {
                    QByteArray image_data_b64(coverArtValue[i].toCString());
                    QByteArray image_data = QByteArray::fromBase64(image_data_b64);

                    const TagLib::String mimeType = ( mimeTypeValue.size() > i ) ? mimeTypeValue[i] : "";
                    const TagLib::String description = ( descriptionValue.size() > i ) ? descriptionValue[i] : "";

                    CoverData *newCover = new CoverData( image_data, TStringToQString(mimeType), CoverData::FrontCover, TStringToQString(description) );
                    covers.append( newCover );
                }
            }
        }
        else if ( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ) )
        {
            if ( file->xiphComment() )
            {
                #ifdef TAGLIB_HAS_FLAC_PICTURELIST
                const TagLib::List<TagLib::FLAC::Picture*> picturelist = file->pictureList();
                for( TagLib::List<TagLib::FLAC::Picture*>::ConstIterator it = picturelist.begin(); it != picturelist.end(); it++ )
                {
                    const TagLib::FLAC::Picture *picture = *it;

                    QByteArray image_data( picture->data().data(), picture->data().size() );
                    CoverData *newCover = new CoverData( image_data, TStringToQString(picture->mimeType()), CoverData::Role(picture->type()), TStringToQString(picture->description()) );
                    covers.append( newCover );
                }
                #endif // TAGLIB_HAS_FLAC_PICTURELIST
            }
        }
        else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                TagLib::MP4::ItemListMap map = mp4tag->itemListMap();
                for( TagLib::MP4::ItemListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( it->first == "covr" )
                    {
                        TagLib::MP4::CoverArtList coverList = it->second.toCoverArtList();
                        for( TagLib::MP4::CoverArtList::Iterator cover = coverList.begin(); cover != coverList.end(); ++cover )
                        {
                            QByteArray image_data( cover->data().data(), cover->data().size() );
                            const QString mimeType = cover->format() == TagLib::MP4::CoverArt::PNG ? "image/png" : "image/jpeg";
                            CoverData *newCover = new CoverData( image_data, mimeType, CoverData::FrontCover );
                            covers.append( newCover );
                        }
                    }
                }
            }
        }
        else if ( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File *>( fileref.file() ) )
        {
            TagLib::ASF::Tag *asftag = dynamic_cast< TagLib::ASF::Tag * >( file->tag() );
            if( asftag )
            {
                #ifdef TAGLIB_HAS_ASF_PICTURE
                TagLib::ASF::AttributeListMap map = asftag->attributeListMap();
                for( TagLib::ASF::AttributeListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( !it->second.size() )
                        continue;

                    if( it->first == "WM/Picture" )
                    {
                        TagLib::ASF::AttributeList coverList = it->second;
                        for( TagLib::ASF::AttributeList::ConstIterator cover = coverList.begin(); cover != coverList.end(); ++cover )
                        {
                            if( cover->type() != TagLib::ASF::Attribute::BytesType )
                                continue;

                            TagLib::ASF::Picture pic = cover->toPicture();
                            QByteArray image_data( pic.picture().data(), pic.picture().size() );
                            CoverData *newCover = new CoverData( image_data, TStringToQString(pic.mimeType()), CoverData::Role(pic.type()), TStringToQString(pic.description()) );
                            covers.append( newCover );
                        }
                    }
                }
                #endif // TAGLIB_HAS_ASF_PICTURE
            }
        }
    }

    return covers;
}

bool TagEngine::writeCovers( const KUrl& fileName, QList<CoverData*> covers )
{
    if( covers.isEmpty() )
        return true;

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit(), false );

    if ( !fileref.isNull() )
    {
        if ( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ) )
        {
            if ( file->ID3v2Tag() )
            {
                foreach( CoverData *cover, covers )
                {
                    TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame( "APIC" );
                    frame->setPicture( TagLib::ByteVector( cover->data.data(), cover->data.size() ) );
                    frame->setType( TagLib::ID3v2::AttachedPictureFrame::Type( cover->role ) );
                    if( !cover->mimeType.isEmpty() )
                        frame->setMimeType( TagLib::ByteVector(cover->mimeType.toUtf8().data()) );
                    if( !cover->description.isEmpty() )
                        frame->setDescription( TagLib::ByteVector(cover->description.toUtf8().data()) );

                    file->ID3v2Tag()->addFrame( frame );
                }
            }

            return fileref.save();
        }
        else if ( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ) )
        {
            if ( file->xiphComment() )
            {
                #ifdef TAGLIB_HAS_FLAC_PICTURELIST
                foreach( CoverData *cover, covers )
                {
                    TagLib::FLAC::Picture *newPicture = new TagLib::FLAC::Picture();
                    newPicture->setData( TagLib::ByteVector( cover->data.data(), cover->data.size() ) );
                    newPicture->setType( TagLib::FLAC::Picture::Type( cover->role ) );
                    if( !cover->mimeType.isEmpty() )
                        newPicture->setMimeType( TagLib::ByteVector(cover->mimeType.toUtf8().data()) );
                    if( !cover->description.isEmpty() )
                        newPicture->setDescription( TagLib::ByteVector(cover->description.toUtf8().data()) );

                    file->addPicture( newPicture );
                }
                #endif // TAGLIB_HAS_FLAC_PICTURELIST
            }

            return fileref.save();
        }
        else if ( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ) )
        {
            TagLib::MP4::Tag *mp4tag = dynamic_cast<TagLib::MP4::Tag *>( file->tag() );
            if( mp4tag )
            {
                TagLib::MP4::CoverArtList coversList;
                foreach( CoverData *cover, covers )
                {
                    const TagLib::MP4::CoverArt::Format format = cover->mimeType == "image/png" ? TagLib::MP4::CoverArt::PNG : TagLib::MP4::CoverArt::JPEG;

                    coversList.append( TagLib::MP4::CoverArt( format, TagLib::ByteVector( cover->data.data(), cover->data.size() ) ) );
                }
                mp4tag->itemListMap()["covr"] = TagLib::MP4::Item( coversList );
            }

            return fileref.save();
        }
        else if ( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File *>( fileref.file() ) )
        {
            TagLib::ASF::Tag *asftag = dynamic_cast< TagLib::ASF::Tag * >( file->tag() );
            if( asftag )
            {
                #ifdef TAGLIB_HAS_ASF_PICTURE
                foreach( CoverData *cover, covers )
                {
                    TagLib::ASF::Picture *newPicture = new TagLib::ASF::Picture();
                    newPicture->setPicture( TagLib::ByteVector( cover->data.data(), cover->data.size() ) );
                    newPicture->setType( TagLib::ASF::Picture::Type( cover->role ) );
                    if( !cover->mimeType.isEmpty() )
                        newPicture->setMimeType( TagLib::ByteVector(cover->mimeType.toUtf8().data()) );
                    if( !cover->description.isEmpty() )
                        newPicture->setDescription( TagLib::ByteVector(cover->description.toUtf8().data()) );

                    asftag->addAttribute( TagLib::String("WM/Picture"), TagLib::ASF::Attribute( newPicture->render() ) );
                }
                #endif // TAGLIB_HAS_ASF_PICTURE
            }

            return fileref.save();
        }
    }

    return false;
}

bool TagEngine::writeCoversToDirectory( const QString& directoryName, TagData *tags )
{
    if( !tags )
        return false;

    QList<CoverData*> covers = tags->covers;

    if( covers.isEmpty() )
        return true;

    QDir dir( directoryName );

    if( directoryName.isEmpty() || !dir.exists() )
        return false;


    int i = covers.count() > 1 ? 1 : 0;

    foreach( CoverData *cover, covers )
    {
        QString fileName = cover->description;
        if( fileName.isEmpty() || config->data.coverArt.writeCoverName == 1 )
        {
            fileName = config->data.coverArt.writeCoverDefaultName;

            fileName.replace( "%a", "$replace_by_artist$" );
            fileName.replace( "%b", "$replace_by_album$" );
            fileName.replace( "%d", "$replace_by_description$" );
            fileName.replace( "%r", "$replace_by_role$" );

            QString artist = tags->artist;
            artist.replace("/",",");
            fileName.replace( "$replace_by_artist$", artist );

            QString album = tags->album;
            album.replace("/",",");
            fileName.replace( "$replace_by_album$", album );

            QString description = cover->description;
            description.replace("/",",");
            fileName.replace( "$replace_by_description$", description );

            QString role = cover->roleName( cover->role );
            role.replace("/",",");
            fileName.replace( "$replace_by_role$", role );

            if( i > 0 )
                fileName += QString::number(i);
        }
        QString extension;
        if( cover->mimeType == "image/jpeg" )
        {
            extension = ".jpg";
            if( fileName.toLower().endsWith(".jpg") )
                fileName = fileName.left( fileName.count() - 4 );
            if( fileName.toLower().endsWith(".jpeg") )
                fileName = fileName.left( fileName.count() - 5 );
        }
        else if( cover->mimeType == "image/png" )
        {
            extension = ".png";
            if( fileName.toLower().endsWith(".png") )
                fileName = fileName.left( fileName.count() - 4 );
        }

        QFile file( directoryName + "/" + fileName + extension );
        if( !file.exists() )
        {
            file.open( QIODevice::WriteOnly );
            file.write( cover->data.data(), cover->data.size() );
            file.close();
        }

        i++;
    }

    return false;
}
