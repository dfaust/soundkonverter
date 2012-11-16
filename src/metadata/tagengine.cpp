
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
#include <uniquefileidentifierframe.h>
#include <xiphcomment.h>
#include <mpcfile.h>
#include <mp4tag.h>
#include <mp4file.h>


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


TagData::TagData()
{
    track = 0;
    trackTotal = 0;
    disc = 0;
    discTotal = 0;
    year = 0;
    trackGain = 0;
    albumGain = 0;

    tagsRead = TagsRead(0);

    length = 0;
    samplingRate = 0;
}

TagData::~TagData()
{
    qDeleteAll( covers );
}


TagEngine::TagEngine( Config *_config )
    : config( _config )
{
    TagLib::StringList genres = TagLib::ID3v1::genreList();
    for( TagLib::StringList::ConstIterator it = genres.begin(), end = genres.end(); it != end; ++it )
        genreList += TStringToQString( (*it) );

    genreList.sort();
}

TagEngine::~TagEngine()
{}

TagData* TagEngine::readTags( const KUrl& fileName )
{
    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit() );

    if( !fileref.isNull() )
    {
        TagData *tagData = new TagData();

        TagLib::Tag *tag = fileref.tag();
        if( tag )
        {
            tagData->title = TStringToQString( tag->title() );
            tagData->artist = TStringToQString( tag->artist() );
            tagData->album = TStringToQString( tag->album() );
            tagData->genre = TStringToQString( tag->genre() );
            if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
            {
                if( TagLib::Ogg::XiphComment *tag = file->tag() )
                {
                    const QString preferredOggVorbisCommentTag = config->data.general.preferredVorbisCommentCommentTag;
                    const QString otherOggVorbisCommentTag = ( preferredOggVorbisCommentTag == "COMMENT" ) ? "DESCRIPTION" : "COMMENT";
                    if( tag && tag->fieldListMap().contains(TagLib::String(preferredOggVorbisCommentTag.toUtf8().data(),TagLib::String::UTF8)) )
                        tagData->comment = TStringToQString( tag->fieldListMap()[TagLib::String(preferredOggVorbisCommentTag.toUtf8().data(), TagLib::String::UTF8)].front() );
                    else if( tag && tag->fieldListMap().contains(TagLib::String(otherOggVorbisCommentTag.toUtf8().data(),TagLib::String::UTF8)) )
                        tagData->comment = TStringToQString( tag->fieldListMap()[TagLib::String(otherOggVorbisCommentTag.toUtf8().data(), TagLib::String::UTF8)].front() );
                }
            }
            else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
            {
                if( TagLib::Ogg::XiphComment *tag = file->xiphComment() )
                {
                    const QString preferredOggVorbisCommentTag = config->data.general.preferredVorbisCommentCommentTag;
                    const QString otherOggVorbisCommentTag = ( preferredOggVorbisCommentTag == "COMMENT" ) ? "DESCRIPTION" : "COMMENT";
                    if( tag && tag->fieldListMap().contains(TagLib::String(preferredOggVorbisCommentTag.toUtf8().data(),TagLib::String::UTF8)) )
                        tagData->comment = TStringToQString( tag->fieldListMap()[TagLib::String(preferredOggVorbisCommentTag.toUtf8().data(), TagLib::String::UTF8)].front() );
                    else if( tag && tag->fieldListMap().contains(TagLib::String(otherOggVorbisCommentTag.toUtf8().data(),TagLib::String::UTF8)) )
                        tagData->comment = TStringToQString( tag->fieldListMap()[TagLib::String(otherOggVorbisCommentTag.toUtf8().data(), TagLib::String::UTF8)].front() );
                }
            }
            else
            {
                tagData->comment = TStringToQString( tag->comment() );
            }
            tagData->track = tag->track();
            tagData->year = tag->year();
        }

        TagLib::AudioProperties *audioProperties = fileref.audioProperties();
        if( audioProperties )
        {
            tagData->length = audioProperties->length();
            tagData->samplingRate = audioProperties->sampleRate();
        }

        Meta::ReplayGainTagMap replayGainTags = Meta::readReplayGainTags( fileref );
        if( replayGainTags.contains(Meta::ReplayGain_Track_Gain) )
        {
            tagData->trackGain = replayGainTags[ Meta::ReplayGain_Track_Gain ];
            tagData->tagsRead = TagData::TagsRead(tagData->tagsRead | TagData::TrackGain);
        }
        if( replayGainTags.contains(Meta::ReplayGain_Album_Gain) )
        {
            tagData->albumGain = replayGainTags[ Meta::ReplayGain_Album_Gain ];
            tagData->tagsRead = TagData::TagsRead(tagData->tagsRead | TagData::AlbumGain);
        }

        QString track;
        QString disc;
        if( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File*>(fileref.file()) )
        {
            // TCMP : Compilation (true,1 vs. false,0)
            // POPM : rating, playcount
            // TXXX : TagLib::ID3v2::UserTextIdentificationFrame
            // APIC : TagLib::ID3v2::AttachedPictureFrame
            // UFID : TagLib::ID3v2::UniqueFileIdentifierFrame

            if( TagLib::ID3v2::Tag *tag = file->ID3v2Tag() )
            {
                if( !tag->frameListMap()["TCOM"].isEmpty() )
                    tagData->composer = TStringToQString( tag->frameListMap()["TCOM"].front()->toString() );

                if( !tag->frameListMap()["TRCK"].isEmpty() )
                    track = TStringToQString( tag->frameListMap()["TRCK"].front()->toString() );

                if( !tag->frameListMap()["TPOS"].isEmpty() )
                    disc = TStringToQString( tag->frameListMap()["TPOS"].front()->toString() );

                TagLib::ID3v2::FrameList ufid = tag->frameListMap()["UFID"];
                if( !ufid.isEmpty() )
                {
                    for( TagLib::ID3v2::FrameList::Iterator it = ufid.begin(); it != ufid.end(); it++ )
                    {
                        TagLib::ID3v2::UniqueFileIdentifierFrame *frame = dynamic_cast<TagLib::ID3v2::UniqueFileIdentifierFrame*>(*it);
                        if( frame && frame->owner() == "http://musicbrainz.org" )
                        {
                            const TagLib::ByteVector id = frame->identifier();
                            tagData->musicBrainzTrackId = QString::fromAscii( id.data(), id.size() );
                        }
                    }
                }
                // Not tested - what about MusicBrainz Album Ids?
                // if( tagData->musicBrainzTrackId.isEmpty() )
                // {
                //     // foobar2k
                //     TagLib::ID3v2::UserTextIdentificationFrame *frame = TagLib::ID3v2::UserTextIdentificationFrame::find( tag, "MUSICBRAINZ_TRACKID" );
                //     if( frame )
                //     {
                //         const TagLib::StringList texts = frame->fieldList();
                //         if( texts.size() > 1)
                //         {
                //             tagData->musicBrainzTrackId = TStringToQString( texts[1] );
                //         }
                //     }
                // }

                TagLib::ID3v2::UserTextIdentificationFrame *frame = TagLib::ID3v2::UserTextIdentificationFrame::find( tag, "MusicBrainz Album Id" );
                if( frame )
                {
                    const TagLib::StringList texts = frame->fieldList();
                    if( texts.size() > 1)
                    {
                        tagData->musicBrainzReleaseId = TStringToQString( texts[1] );
                    }
                }
            }
        }
        else if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
        {
            // COMPILATION (1 vs. 0)

            if( TagLib::Ogg::XiphComment *tag = file->tag() )
            {
                if( !tag->fieldListMap()["COMPOSER"].isEmpty() )
                    tagData->composer = TStringToQString( tag->fieldListMap()["COMPOSER"].front() );

                if( !tag->fieldListMap()["TRACKTOTAL"].isEmpty() ) // used by EasyTag
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TRACKTOTAL"].front() ).toInt();
                else if( !tag->fieldListMap()["TOTALTRACKS"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TOTALTRACKS"].front() ).toInt();
                else if( !tag->fieldListMap()["TRACKNUMBER"].isEmpty() )
                    track = TStringToQString( tag->fieldListMap()["TRACKNUMBER"].front() );

                if( !tag->fieldListMap()["DISCTOTAL"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["DISCTOTAL"].front() ).toInt();
                else if( !tag->fieldListMap()["TOTALDISCS"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TOTALDISCS"].front() ).toInt();
                else if( !tag->fieldListMap()["DISCNUMBER"].isEmpty() )
                    disc = TStringToQString( tag->fieldListMap()["DISCNUMBER"].front() );

                if( !tag->fieldListMap()["MUSICBRAINZ_TRACKID"].isEmpty() )
                    tagData->musicBrainzTrackId = TStringToQString( tag->fieldListMap()["MUSICBRAINZ_TRACKID"].front() );

                if( !tag->fieldListMap()["MUSICBRAINZ_ALBUMID"].isEmpty() )
                    tagData->musicBrainzReleaseId = TStringToQString( tag->fieldListMap()["MUSICBRAINZ_ALBUMID"].front() );
            }
        }
        else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
        {
            if( TagLib::Ogg::XiphComment *tag = file->xiphComment() )
            {
                if( !tag->fieldListMap()["COMPOSER"].isEmpty() )
                    tagData->composer = TStringToQString( tag->fieldListMap()["COMPOSER"].front() );

                if( !tag->fieldListMap()["TRACKTOTAL"].isEmpty() ) // used by EasyTag
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TRACKTOTAL"].front() ).toInt();
                else if( !tag->fieldListMap()["TOTALTRACKS"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TOTALTRACKS"].front() ).toInt();
                else if( !tag->fieldListMap()["TRACKNUMBER"].isEmpty() ) // used by Kid3
                    track = TStringToQString( tag->fieldListMap()["TRACKNUMBER"].front() );

                if( !tag->fieldListMap()["DISCTOTAL"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["DISCTOTAL"].front() ).toInt();
                else if( !tag->fieldListMap()["TOTALDISCS"].isEmpty() )
                    tagData->trackTotal = TStringToQString( tag->fieldListMap()["TOTALDISCS"].front() ).toInt();
                else if( !tag->fieldListMap()["DISCNUMBER"].isEmpty() ) // used by Kid3, EasyTag
                    disc = TStringToQString( tag->fieldListMap()["DISCNUMBER"].front() );

                if( !tag->fieldListMap()["MUSICBRAINZ_TRACKID"].isEmpty() )
                    tagData->musicBrainzTrackId = TStringToQString( tag->fieldListMap()["MUSICBRAINZ_TRACKID"].front() );

                if( !tag->fieldListMap()["MUSICBRAINZ_ALBUMID"].isEmpty() )
                    tagData->musicBrainzReleaseId = TStringToQString( tag->fieldListMap()["MUSICBRAINZ_ALBUMID"].front() );
            }
        }
        else if( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File*>(fileref.file()) )
        {
            // cpil : Compilation (true vs. false)

            if( TagLib::MP4::Tag *tag = file->tag() )
            {
                TagLib::MP4::ItemListMap map = tag->itemListMap();
                for( TagLib::MP4::ItemListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( it->first == "\xA9wrt" )
                    {
                        tagData->composer = TStringToQString( it->second.toStringList().front() );
                    }
                    else if( it->first == "trkn" )
                    {
                        tagData->trackTotal = it->second.toIntPair().second;
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
                    else if( it->first == "----:com.apple.iTunes:MusicBrainz Track Id" )
                    {
                        tagData->musicBrainzTrackId = TStringToQString( it->second.toStringList().front() );
                    }
                    else if( it->first == "----:com.apple.iTunes:MusicBrainz Album Id" )
                    {
                        tagData->musicBrainzReleaseId = TStringToQString( it->second.toStringList().front() );
                    }
                }
            }
        }
        else if( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File*>(fileref.file()) )
        {
            if( TagLib::ASF::Tag *tag = file->tag() )
            {
                TagLib::ASF::AttributeListMap map = tag->attributeListMap();
                for( TagLib::ASF::AttributeListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( !it->second.size() )
                        continue;

                    if( it->first == "WM/Composer" )
                    {
                        tagData->composer = TStringToQString( it->second.front().toString() );
                    }
                    else if( it->first == "WM/TrackNumber" )
                    {
                        track = TStringToQString( it->second.front().toString() );
                    }
                    else if( it->first == "WM/PartOfSet" )
                    {
                        disc = TStringToQString( it->second.front().toString() );
                    }
                    else if( it->first == "MusicBrainz/Track Id" )
                    {
                        tagData->musicBrainzTrackId = TStringToQString( it->second.front().toString() );
                    }
                    else if( it->first == "MusicBrainz/Album Id" )
                    {
                        tagData->musicBrainzReleaseId = TStringToQString( it->second.front().toString() );
                    }
                }
            }
        }
        /*else if( TagLib::MPC::File *file = dynamic_cast<TagLib::MPC::File *>( fileref.file() ) )
        {
            if( file->APETag() )
            {
                if( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    trackGain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );

                if( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    albumGain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
            }
        }*/
/*        else if( TagLib::WavPack::File *file = dynamic_cast<TagLib::WavPack::File *>( fileref.file() ) )
        {
            if( file->APETag() )
            {
                if( !file->APETag()->itemListMap()[ "REPLAYGAIN_TRACK_GAIN" ].isEmpty() )
                    trackGain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_TRACK_GAIN"].toString() );

                if( !file->APETag()->itemListMap()[ "REPLAYGAIN_ALBUM_GAIN" ].isEmpty() )
                    albumGain = TStringToQString( file->APETag()->itemListMap()["REPLAYGAIN_ALBUM_GAIN"].toString() );
            }
        }*/
        /*else if( TagLib::TTA::File *file = dynamic_cast<TagLib::TTA::File *>( fileref.file() ) ) // NOTE writing works, but reading not
        {
            if( file->ID3v2Tag() )
            {
                if( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                    disc = TStringToQString( file->ID3v2Tag()->frameListMap()["TPOS"].front()->toString() );

                if( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
                    tagData->composer = TStringToQString( file->ID3v2Tag()->frameListMap()["TCOM"].front()->toString() );
            }
        }*/

        if( !track.isEmpty() )
        {
            const int i = track.indexOf('/');
            if( i != -1 )
            {
                tagData->trackTotal = track.right( track.count() - i - 1 ).toInt();
            }
        }

        if( !disc.isEmpty() )
        {
            const int i = disc.indexOf('/');
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

        return tagData;
    }

    return 0;
}

bool TagEngine::writeTags( const KUrl& fileName, TagData *tagData )
{
    if( !tagData )
        return false;

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit(), false );

    //Set default codec to UTF-8 (see bugs 111246 and 111232)
    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding( TagLib::String::UTF8 );

    if( !fileref.isNull() )
    {
        if( TagLib::Tag *tag = fileref.tag() )
        {
            tag->setTitle( TagLib::String(tagData->title.toUtf8().data(), TagLib::String::UTF8) );
            tag->setArtist( TagLib::String(tagData->artist.toUtf8().data(), TagLib::String::UTF8) );
            tag->setAlbum( TagLib::String(tagData->album.toUtf8().data(), TagLib::String::UTF8) );
            tag->setTrack( tagData->track );
            tag->setYear( tagData->year );
            if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
            {
                if( TagLib::Ogg::XiphComment *tag = file->tag() )
                {
                    if( !tagData->comment.isEmpty() )
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentCommentTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(tagData->comment.toUtf8().data(), TagLib::String::UTF8), true );

                }
            }
            else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
            {
                if( TagLib::Ogg::XiphComment *tag = file->xiphComment() )
                {
                    if( !tagData->comment.isEmpty() && file->xiphComment() )
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentCommentTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(tagData->comment.toUtf8().data(), TagLib::String::UTF8), true );
                }
            }
            else
            {
                tag->setComment( TagLib::String(tagData->comment.toUtf8().data(), TagLib::String::UTF8) );
            }
            tag->setGenre( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
        }
        else
        {
            return false;
        }

        QString track;
        if( tagData->trackTotal > 0 )
        {
            track = QString::number(tagData->track) + "/" + QString::number(tagData->trackTotal);
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

        if( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File*>(fileref.file()) )
        {
            if( TagLib::ID3v2::Tag *tag = file->ID3v2Tag() )
            {
                if( !tagData->composer.isEmpty() )
                {
                    if( !tag->frameListMap()["TCOM"].isEmpty() )
                    {
                        tag->frameListMap()["TCOM"].front()->setText( TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
                    }
                    else
                    {
                        TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TCOM", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                        frame->setText( TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
                        tag->addFrame( frame );
                    }
                }

                if( !track.isEmpty() )
                {
                    if( !tag->frameListMap()["TRCK"].isEmpty() )
                    {
                        tag->frameListMap()["TRCK"].front()->setText( TagLib::String(track.toUtf8().data(), TagLib::String::UTF8) );
                    }
                    else
                    {
                        TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TRCK", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                        frame->setText( TagLib::String(track.toUtf8().data(), TagLib::String::UTF8) );
                        tag->addFrame( frame );
                    }
                }

                if( !disc.isEmpty() )
                {
                    if( !tag->frameListMap()["TPOS"].isEmpty() )
                    {
                        tag->frameListMap()["TPOS"].front()->setText( TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8) );
                    }
                    else
                    {
                        TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TPOS", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                        frame->setText( TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8) );
                        tag->addFrame( frame );
                    }
                }

                if( !tagData->musicBrainzTrackId.isEmpty() )
                {
                    TagLib::ID3v2::UniqueFileIdentifierFrame *frame = new TagLib::ID3v2::UniqueFileIdentifierFrame( "http://musicbrainz.org", TagLib::ByteVector(tagData->musicBrainzTrackId.toUtf8().data(), TagLib::String::UTF8) );
                    tag->addFrame( frame );
                }

                if( !tagData->musicBrainzReleaseId.isEmpty() )
                {
                    TagLib::ID3v2::UserTextIdentificationFrame *frame = new TagLib::ID3v2::UserTextIdentificationFrame( TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setDescription( "MusicBrainz Album Id" );
                    frame->setText( TagLib::String(tagData->musicBrainzReleaseId.toUtf8().data(), TagLib::String::UTF8) );
                    tag->addFrame( frame );
                }

                // // HACK sets the id3v2 genre tag as string
                // if( !tagData->genre.isEmpty() )
                // {
                //     if( !tag->frameListMap()["TCON"].isEmpty() )
                //     {
                //         tag->frameListMap()["TCON"].front()->setText( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
                //     }
                //     else
                //     {
                //         TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TCON", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                //         frame->setText( TagLib::String(tagData->genre.toUtf8().data(), TagLib::String::UTF8) );
                //         tag->addFrame( frame );
                //     }
                // }

                // // HACK sets the id3 <= v2.3.0 year tag
                // if( tagData->year > 0 )
                // {
                //     if( !tag->frameListMap()["TYER"].isEmpty() )
                //     {
                //         tag->frameListMap()["TYER"].front()->setText( TagLib::String(QString::number(tagData->year).toUtf8().data(), TagLib::String::UTF8) );
                //     }
                //     else
                //     {
                //         TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TYER", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                //         frame->setText( TagLib::String(QString::number(tagData->year).toUtf8().data(), TagLib::String::UTF8) );
                //         tag->addFrame( frame );
                //     }
                // }
            }
        }
        else if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
        {
            if( TagLib::Ogg::XiphComment *tag = file->tag() )
            {
                if( !tagData->composer.isEmpty() )
                {
                    if( tag->contains("COMPOSER") )
                        tag->removeField("COMPOSER");

                    tag->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( tagData->trackTotal > 0 )
                {
                    if( config->data.general.preferredVorbisCommentTrackTotalTag == "TRACKNUMBER" )
                    {
                        if( tag->contains("TRACKNUMBER") )
                            tag->removeField("TRACKNUMBER");

                        tag->addField( "TRACKNUMBER", TagLib::String(track.toUtf8().data(), TagLib::String::UTF8), true );
                    }
                    else
                    {
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentTrackTotalTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(QString::number(tagData->trackTotal).toUtf8().data(), TagLib::String::UTF8), true );
                    }
                }

                if( tagData->disc > 0 )
                {
                    if( tag->contains("DISCNUMBER") )
                        tag->removeField("DISCNUMBER");

                    tag->addField( "DISCNUMBER", TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( tagData->discTotal > 0 )
                {
                    if( config->data.general.preferredVorbisCommentDiscTotalTag == "DISCNUMBER" )
                    {
                        if( tag->contains("DISCNUMBER") )
                            tag->removeField("DISCNUMBER");

                        tag->addField( "DISCNUMBER", TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8), true );
                    }
                    else
                    {
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentDiscTotalTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(QString::number(tagData->discTotal).toUtf8().data(), TagLib::String::UTF8), true );
                    }
                }

                if( !tagData->musicBrainzTrackId.isEmpty() )
                {
                    if( tag->contains("MUSICBRAINZ_TRACKID") )
                        tag->removeField("MUSICBRAINZ_TRACKID");

                    tag->addField( "MUSICBRAINZ_TRACKID", TagLib::String(tagData->musicBrainzTrackId.toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( !tagData->musicBrainzReleaseId.isEmpty() )
                {
                    if( tag->contains("MUSICBRAINZ_ALBUMID") )
                        tag->removeField("MUSICBRAINZ_ALBUMID");

                    tag->addField( "MUSICBRAINZ_ALBUMID", TagLib::String(tagData->musicBrainzReleaseId.toUtf8().data(), TagLib::String::UTF8), true );
                }
            }
        }
        else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
        {
            if( TagLib::Ogg::XiphComment *tag = file->xiphComment() )
            {
                if( !tagData->composer.isEmpty() )
                {
                    if( tag->contains("COMPOSER") )
                        tag->removeField("COMPOSER");

                    tag->addField( "COMPOSER", TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( tagData->trackTotal > 0 )
                {
                    if( config->data.general.preferredVorbisCommentTrackTotalTag == "TRACKNUMBER" )
                    {
                        if( tag->contains("TRACKNUMBER") )
                            tag->removeField("TRACKNUMBER");

                        tag->addField( "TRACKNUMBER", TagLib::String(track.toUtf8().data(), TagLib::String::UTF8), true );
                    }
                    else
                    {
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentTrackTotalTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(QString::number(tagData->trackTotal).toUtf8().data(), TagLib::String::UTF8), true );
                    }
                }

                if( tagData->disc > 0 )
                {
                    if( tag->contains("DISCNUMBER") )
                        tag->removeField("DISCNUMBER");

                    tag->addField( "DISCNUMBER", TagLib::String(QString::number(tagData->disc).toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( tagData->discTotal > 0 )
                {
                    if( config->data.general.preferredVorbisCommentDiscTotalTag == "DISCNUMBER" )
                    {
                        if( tag->contains("DISCNUMBER") )
                            tag->removeField("DISCNUMBER");

                        tag->addField( "DISCNUMBER", TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8), true );
                    }
                    else
                    {
                        tag->addField( TagLib::String(config->data.general.preferredVorbisCommentDiscTotalTag.toUtf8().data(), TagLib::String::UTF8), TagLib::String(QString::number(tagData->discTotal).toUtf8().data(), TagLib::String::UTF8), true );
                    }
                }

                if( !tagData->musicBrainzTrackId.isEmpty() )
                {
                    if( tag->contains("MUSICBRAINZ_TRACKID") )
                        tag->removeField("MUSICBRAINZ_TRACKID");

                    tag->addField( "MUSICBRAINZ_TRACKID", TagLib::String(tagData->musicBrainzTrackId.toUtf8().data(), TagLib::String::UTF8), true );
                }

                if( !tagData->musicBrainzReleaseId.isEmpty() )
                {
                    if( tag->contains("MUSICBRAINZ_ALBUMID") )
                        tag->removeField("MUSICBRAINZ_ALBUMID");

                    tag->addField( "MUSICBRAINZ_ALBUMID", TagLib::String(tagData->musicBrainzReleaseId.toUtf8().data(), TagLib::String::UTF8), true );
                }
            }
        }
        else if( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File*>(fileref.file()) )
        {
            if( TagLib::MP4::Tag *tag = file->tag() )
            {
                if( !tagData->composer.isEmpty() )
                    tag->itemListMap()["\xA9wrt"] = TagLib::StringList(TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8));

                if( tagData->trackTotal > 0 )
                    tag->itemListMap()["trkn"] = TagLib::MP4::Item( tagData->track, tagData->trackTotal );

                if( tagData->disc > 0 )
                {
                    if( tagData->discTotal > 0 )
                        tag->itemListMap()["disk"] = TagLib::MP4::Item( tagData->disc, tagData->discTotal );
                    else
                        tag->itemListMap()["disk"] = TagLib::MP4::Item( tagData->disc );
                }

                if( !tagData->musicBrainzTrackId.isEmpty() )
                    tag->itemListMap()["----:com.apple.iTunes:MusicBrainz Track Id"] = TagLib::StringList(TagLib::String(tagData->musicBrainzTrackId.toUtf8().data(), TagLib::String::UTF8));

                if( !tagData->musicBrainzReleaseId.isEmpty() )
                    tag->itemListMap()["----:com.apple.iTunes:MusicBrainz Album Id"] = TagLib::StringList(TagLib::String(tagData->musicBrainzReleaseId.toUtf8().data(), TagLib::String::UTF8));
            }
        }
        else if( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File*>(fileref.file()) )
        {
            if( TagLib::ASF::Tag *tag = file->tag() )
            {
                if( !tagData->composer.isEmpty() )
                {
                    if( !tag->attributeListMap()["WM/Composer"].isEmpty() )
                        tag->attributeListMap()["WM/Composer"].clear();

                    tag->addAttribute( TagLib::String("WM/Composer"), TagLib::String(tagData->composer.toUtf8().data(), TagLib::String::UTF8) );
                }

                if( !track.isEmpty() )
                {
                    if( !tag->attributeListMap()["WM/TrackNumber"].isEmpty() )
                        tag->attributeListMap()["WM/TrackNumber"].clear();

                    tag->addAttribute( TagLib::String("WM/TrackNumber"), TagLib::String(track.toUtf8().data(), TagLib::String::UTF8) );
                }

                if( !disc.isEmpty() )
                {
                    if( !tag->attributeListMap()["WM/PartOfSet"].isEmpty() )
                        tag->attributeListMap()["WM/PartOfSet"].clear();

                    tag->addAttribute( TagLib::String("WM/PartOfSet"), TagLib::String(disc.toUtf8().data(), TagLib::String::UTF8) );
                }

                if( !tagData->musicBrainzTrackId.isEmpty() )
                {
                    if( !tag->attributeListMap()["MusicBrainz/Track Id"].isEmpty() )
                        tag->attributeListMap()["MusicBrainz/Track Id"].clear();

                    tag->addAttribute( TagLib::String("MusicBrainz/Track Id"), TagLib::String(tagData->musicBrainzTrackId.toUtf8().data(), TagLib::String::UTF8) );
                }

                if( !tagData->musicBrainzReleaseId.isEmpty() )
                {
                    if( !tag->attributeListMap()["MusicBrainz/Album Id"].isEmpty() )
                        tag->attributeListMap()["MusicBrainz/Album Id"].clear();

                    tag->addAttribute( TagLib::String("MusicBrainz/Album Id"), TagLib::String(tagData->musicBrainzReleaseId.toUtf8().data(), TagLib::String::UTF8) );
                }
            }
        }
        /*if( TagLib::TTA::File *file = dynamic_cast<TagLib::TTA::File *>( fileref.file() ) ) // NOTE writing works, but reading not
        {
            if( file->ID3v2Tag() )
            {
                if( !file->ID3v2Tag()->frameListMap()[ "TPOS" ].isEmpty() )
                {
                    file->ID3v2Tag()->frameListMap()[ "TPOS" ].front()->setText( QStringToTString( QString::number(tagData->disc) ) );
                }
                else
                {
                    TagLib::ID3v2::TextIdentificationFrame *frame = new TagLib::ID3v2::TextIdentificationFrame( "TPOS", TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding() );
                    frame->setText( QStringToTString( QString::number(tagData->disc) ) );
                    file->ID3v2Tag()->addFrame( frame );
                }

                if( !file->ID3v2Tag()->frameListMap()[ "TCOM" ].isEmpty() )
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

QList<CoverData*> TagEngine::readCovers( const KUrl& fileName )
{
    QList<CoverData*> covers;

    TagLib::FileRef fileref( fileName.pathOrUrl().toLocal8Bit() );

    if( !fileref.isNull() )
    {
        if( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File*>(fileref.file()) )
        {
            if( TagLib::ID3v2::Tag *tag = file->ID3v2Tag() )
            {
                TagLib::ID3v2::FrameList apic_frames = tag->frameListMap()["APIC"];

                for( TagLib::ID3v2::FrameList::ConstIterator iter = apic_frames.begin(); iter != apic_frames.end(); ++iter )
                {
                    TagLib::ID3v2::AttachedPictureFrame *pic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*iter);

                    const QByteArray image_data( pic->picture().data(), pic->picture().size() );
                    CoverData *newCover = new CoverData( image_data, TStringToQString(pic->mimeType()), CoverData::Role(pic->type()), TStringToQString(pic->description()) );
                    covers.append( newCover );
                }
            }
        }
        else if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
        {
            // http://wiki.xiph.org/VorbisComment#Recommended_field_names

            if( TagLib::Ogg::XiphComment *tag = file->tag() )
            {
                #ifdef TAGLIB_HAS_FLAC_PICTURELIST
                const TagLib::StringList& block = tag->fieldListMap()["METADATA_BLOCK_PICTURE"];
                for( TagLib::StringList::ConstIterator i = block.begin(); i != block.end(); ++i )
                {
                    const QByteArray data( QByteArray::fromBase64( i->to8Bit().c_str() ) );
                    TagLib::ByteVector tdata( data.data(), data.size() );
                    TagLib::FLAC::Picture picture;

                    if(!picture.parse(tdata))
                        continue;

                    const QByteArray image_data( picture.data().data(), picture.data().size() );
                    CoverData *newCover = new CoverData( image_data, TStringToQString(picture.mimeType()), CoverData::Role(picture.type()), TStringToQString(picture.description()) );
                    covers.append( newCover );
                }
                #endif // TAGLIB_HAS_FLAC_PICTURELIST


                TagLib::Ogg::FieldListMap map = tag->fieldListMap();

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
                    const QByteArray image_data_b64(coverArtValue[i].toCString());
                    const QByteArray image_data = QByteArray::fromBase64(image_data_b64);

                    const TagLib::String mimeType = ( mimeTypeValue.size() > i ) ? mimeTypeValue[i] : "";
                    const TagLib::String description = ( descriptionValue.size() > i ) ? descriptionValue[i] : "";

                    CoverData *newCover = new CoverData( image_data, TStringToQString(mimeType), CoverData::FrontCover, TStringToQString(description) );
                    covers.append( newCover );
                }
            }
        }
        else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
        {
            #ifdef TAGLIB_HAS_FLAC_PICTURELIST
            const TagLib::List<TagLib::FLAC::Picture*> picturelist = file->pictureList();
            for( TagLib::List<TagLib::FLAC::Picture*>::ConstIterator it = picturelist.begin(); it != picturelist.end(); it++ )
            {
                const TagLib::FLAC::Picture *picture = *it;

                const QByteArray image_data( picture->data().data(), picture->data().size() );
                CoverData *newCover = new CoverData( image_data, TStringToQString(picture->mimeType()), CoverData::Role(picture->type()), TStringToQString(picture->description()) );
                covers.append( newCover );
            }
            #endif // TAGLIB_HAS_FLAC_PICTURELIST
        }
        else if( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File*>(fileref.file()) )
        {
            if( TagLib::MP4::Tag *tag = file->tag() )
            {
                TagLib::MP4::ItemListMap map = tag->itemListMap();
                for( TagLib::MP4::ItemListMap::ConstIterator it = map.begin(); it != map.end(); ++it )
                {
                    if( it->first == "covr" )
                    {
                        TagLib::MP4::CoverArtList coverList = it->second.toCoverArtList();
                        for( TagLib::MP4::CoverArtList::Iterator cover = coverList.begin(); cover != coverList.end(); ++cover )
                        {
                            const QByteArray image_data( cover->data().data(), cover->data().size() );
                            const QString mimeType = cover->format() == TagLib::MP4::CoverArt::PNG ? "image/png" : "image/jpeg";
                            CoverData *newCover = new CoverData( image_data, mimeType, CoverData::FrontCover );
                            covers.append( newCover );
                        }
                    }
                }
            }
        }
        else if( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File*>(fileref.file()) )
        {
            if( TagLib::ASF::Tag *tag = file->tag() )
            {
                #ifdef TAGLIB_HAS_ASF_PICTURE
                TagLib::ASF::AttributeListMap map = tag->attributeListMap();
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
                            const QByteArray image_data( pic.picture().data(), pic.picture().size() );
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

    if( !fileref.isNull() )
    {
        if( TagLib::MPEG::File *file = dynamic_cast<TagLib::MPEG::File*>(fileref.file()) )
        {
            if( TagLib::ID3v2::Tag *tag = file->ID3v2Tag() )
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

                    tag->addFrame( frame );
                }
            }

            return fileref.save();
        }
        else if( TagLib::Ogg::Vorbis::File *file = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileref.file()) )
        {
            if( TagLib::Ogg::XiphComment *tag = file->tag() )
            {
                #ifdef TAGLIB_HAS_FLAC_PICTURELIST
                foreach( CoverData *cover, covers )
                {
                    TagLib::FLAC::Picture newPicture;
                    newPicture.setData( TagLib::ByteVector( cover->data.data(), cover->data.size() ) );
                    newPicture.setType( TagLib::FLAC::Picture::Type( cover->role ) );
                    if( !cover->mimeType.isEmpty() )
                        newPicture.setMimeType( TagLib::ByteVector(cover->mimeType.toUtf8().data()) );
                    if( !cover->description.isEmpty() )
                        newPicture.setDescription( TagLib::ByteVector(cover->description.toUtf8().data()) );

                    TagLib::ByteVector t_block = newPicture.render();
                    const QByteArray q_block = QByteArray( t_block.data(), t_block.size() );
                    const QByteArray q_block_b64 = q_block.toBase64();
                    TagLib::ByteVector t_block_b64 = TagLib::ByteVector( q_block_b64.data(), q_block_b64.size() );
                    tag->addField( "METADATA_BLOCK_PICTURE", t_block_b64, false );
                }
                #endif // TAGLIB_HAS_FLAC_PICTURELIST
            }

            return fileref.save();
        }
        else if( TagLib::FLAC::File *file = dynamic_cast<TagLib::FLAC::File*>(fileref.file()) )
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

            return fileref.save();
        }
        else if( TagLib::MP4::File *file = dynamic_cast<TagLib::MP4::File*>(fileref.file()) )
        {
            if( TagLib::MP4::Tag *tag = file->tag() )
            {
                TagLib::MP4::CoverArtList coversList;
                foreach( CoverData *cover, covers )
                {
                    const TagLib::MP4::CoverArt::Format format = cover->mimeType == "image/png" ? TagLib::MP4::CoverArt::PNG : TagLib::MP4::CoverArt::JPEG;

                    coversList.append( TagLib::MP4::CoverArt( format, TagLib::ByteVector( cover->data.data(), cover->data.size() ) ) );
                }
                tag->itemListMap()["covr"] = TagLib::MP4::Item( coversList );
            }

            return fileref.save();
        }
        else if( TagLib::ASF::File *file = dynamic_cast<TagLib::ASF::File*>(fileref.file()) )
        {
            if( TagLib::ASF::Tag *tag = file->tag() )
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

                    tag->addAttribute( TagLib::String("WM/Picture"), TagLib::ASF::Attribute( newPicture->render() ) );
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
