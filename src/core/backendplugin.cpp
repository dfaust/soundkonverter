
#include "backendplugin.h"

#include <QFile>
#include <KStandardDirs>


BackendPluginItem::BackendPluginItem( QObject *parent )
    : QObject( parent )
{
    process = 0;
    id = -1;
    progress = -1;
}

BackendPluginItem::~BackendPluginItem()
{}


BackendPlugin::BackendPlugin( QObject *parent )
    : QObject( parent )
{
    lastId = 0;
}

BackendPlugin::~BackendPlugin()
{}

BackendPlugin::FormatInfo BackendPlugin::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "wav" )
    {
        info.lossless = true;
        info.description = i18n("Wave won't compress the audio stream.");
        info.mimeTypes.append( "audio/x-wav" );
        info.mimeTypes.append( "audio/wav" );
        info.extensions.append( "wav" );
    }
    else if( codecName == "aac" )
    {
        info.lossless = false;
        info.description = i18n("Advanced Audio Coding is a lossy and popular audio format. It is widely used by Apple Inc."); // http://en.wikipedia.org/wiki/Advanced_Audio_Coding
        info.mimeTypes.append( "audio/aac" );
        info.extensions.append( "aac" );
    }
    else if( codecName == "m4a" )
    {
        info.lossless = false;
        info.description = i18n("m4a files are encoded with the aac codec. It is widely used by Apple Inc.");
        info.mimeTypes.append( "audio/mp4" );
        info.mimeTypes.append( "audio/x-m4a" );
        info.extensions.append( "m4a" );
        info.extensions.append( "f4a" );
        info.extensions.append( "aac" );
    }
    else if( codecName == "mp4" )
    {
        info.lossless = false;
//         info.description = i18n("mp4 files are video files with an aac encoded audio stream. It is widely used by Apple Inc.");
        info.mimeTypes.append( "video/mp4" );
        info.extensions.append( "mp4" );
        info.extensions.append( "m4v" );
        info.extensions.append( "f4v" );
    }
    else if( codecName == "ogg vorbis" )
    {
        info.lossless = false;
        info.description = i18n("Ogg Vorbis is a free and lossy high quality audio codec.\nFor more information see: http://www.xiph.org/vorbis/");
        info.mimeTypes.append( "application/ogg" );
        info.mimeTypes.append( "audio/vorbis" );
        info.mimeTypes.append( "application/x-ogg" );
        info.mimeTypes.append( "audio/ogg" );
        info.mimeTypes.append( "audio/x-vorbis+ogg" );
        info.extensions.append( "ogg" );
        info.extensions.append( "oga" );
    }
    else if( codecName == "mp3" )
    {
        info.lossless = false;
        info.description = i18n("MP3 is the most popular lossy audio codec. It's supported by all audio players.");
        info.mimeTypes.append( "audio/x-mp3" );
        info.mimeTypes.append( "audio/mpeg" );
        info.mimeTypes.append( "audio/mp3" );
        info.extensions.append( "mp3" );
    }
    else if( codecName == "flac" )
    {
        info.lossless = true;
        info.description = i18n("Flac is the free lossless audio codec.\nAs it's name says, it compresses without any loss of quality.");
        info.mimeTypes.append( "audio/x-flac" );
        info.mimeTypes.append( "audio/x-flac+ogg" );
        info.mimeTypes.append( "audio/x-oggflac" );
        info.extensions.append( "flac" );
        info.extensions.append( "fla" );
    }
    else if( codecName == "wma" )
    {
        info.lossless = false;
        info.description = i18n("Windows Media Audio is a propritary audio codec from Microsoft.");
        info.mimeTypes.append( "audio/x-ms-wma" );
        info.extensions.append( "wma" );
    }
    else if( codecName == "ac3" ) // TODO description
    {
        info.lossless = false;
        info.description = i18n("Dolby Digital-Audio"); // http://en.wikipedia.org/wiki/Ac3
        info.mimeTypes.append( "audio/ac3" );
        info.extensions.append( "ac3" );
    }
//     else if( codecName == "e-ac3" ) // TODO mimtypes, extensions
//     {
//         info.lossless = false;
//         info.description = i18n("Dolby Digital Plus (Enhanced AC-3) is an advanced version of AC-3 for use on Blu-Ray discs.");
//         info.mimeTypes.append( "audio/eac3" );
//         info.extensions.append( "eac3" );
//     }
    else if( codecName == "alac" )
    {
        info.lossless = true;
        info.description = i18n("Apple Lossless Audio Codec is a lossless audio format from Apple Inc.."); // http://en.wikipedia.org/wiki/Alac
        info.mimeTypes.append( "audio/mp4" );
        info.mimeTypes.append( "audio/x-m4a" );
        info.extensions.append( "m4a" );
        info.extensions.append( "f4a" );
    }
    else if( codecName == "mp2" )
    {
        info.lossless = false;
        info.description = i18n("MPEG-1 Audio Layer II is an old lossy audio format."); // http://en.wikipedia.org/wiki/MPEG-1_Audio_Layer_II
        info.mimeTypes.append( "audio/mp2" );
        info.mimeTypes.append( "audio/x-mp2" );
        info.extensions.append( "mp2" );
    }
//     else if( codecName == "sonic" )
//     {
//         info.lossless = false;
//         info.description = i18n("Sonic");
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
//     else if( codecName == "sonicls" )
//     {
//         info.lossless = true;
//         info.description = i18n("Sonic Lossless");
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
    else if( codecName == "als" ) // TODO mime type, etc.
    {
        info.lossless = true;
        info.description = i18n("MPEG-4 Audio Lossless Coding"); // http://en.wikipedia.org/wiki/Audio_Lossless_Coding
        info.mimeTypes.append( "audio/mp4" );
        info.extensions.append( "mp4" );
    }
    else if( codecName == "amr nb" )
    {
        info.lossless = false;
        info.description = i18n("Adaptive Multi-Rate Narrow-Band is based on 3gp and mainly used for speech compression in mobile communication."); // http://en.wikipedia.org/wiki/Adaptive_Multi-Rate_audio_codec
        info.mimeTypes.append( "audio/amr" );
        info.mimeTypes.append( "audio/3gpp" );
        info.mimeTypes.append( "audio/3gpp2" );
        info.extensions.append( "amr" );
    }
    else if( codecName == "amr wb" )
    {
        info.lossless = false;
        info.description = i18n("Adaptive Multi-Rate Wide-Band is an advanced version of amr nb which uses a higher data rate resulting in a higher quality."); // http://en.wikipedia.org/wiki/Adaptive_Multi-Rate_Wideband
        info.mimeTypes.append( "audio/amr-wb" );
        info.mimeTypes.append( "audio/3gpp" );
        info.extensions.append( "awb" );
    }
    else if( codecName == "ape" )
    {
        info.lossless = true;
        info.description = i18n("Monkey’s Audio is a fast and efficient but propritary lossless audio format.");
        info.mimeTypes.append( "audio/x-ape" );
        info.extensions.append( "ape" );
        info.extensions.append( "mac" );
        info.extensions.append( "apl" );
    }
    else if( codecName == "speex" )
    {
        info.lossless = false;
        info.description = i18n("Speex is a free and lossy audio codec designed for low quality speech encoding.\nFor more information see: http://www.speex.org");
        info.mimeTypes.append( "audio/x-speex" );
        info.mimeTypes.append( "audio/x-speex+ogg" );
        info.extensions.append( "spx" );
    }
    else if( codecName == "mp1" )
    {
        info.lossless = false;
        info.description = i18n("MPEG-1 Audio Layer I very old and lossy file format."); // http://en.wikipedia.org/wiki/MP1
//         info.mimeTypes.append( "audio/mpeg" );
        info.extensions.append( "mp1" );
    }
    else if( codecName == "musepack" )
    {
        info.lossless = false;
        info.description = i18n("Musepack is a free and lossy file format based on mp2 and optimized for high quality."); // http://en.wikipedia.org/wiki/Musepack
        info.mimeTypes.append( "audio/x-musepack" );
        info.mimeTypes.append( "audio/musepack" );
        info.extensions.append( "mpc" );
        info.extensions.append( "mp+" );
        info.extensions.append( "mpp" );
    }
    else if( codecName == "shorten" )
    {
        info.lossless = true;
        info.description = i18n("Shorten is a free and lossless audio codec.\nFor more information see: http://etree.org/shnutils/shorten/");
        info.mimeTypes.append( "application/x-shorten" );
        info.extensions.append( "shn" );
    }
//     else if( codecName == "mlp" )
//     {
//         info.lossless = true;
//         info.description = i18n("Meridian Lossless Packing is an old propritary lossless audio format."); // http://en.wikipedia.org/wiki/Meridian_Lossless_Packing
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
//     else if( codecName == "truehd" )
//     {
//         info.lossless = true;
//         info.description = i18n("Dolby TrueHD is a lossless audio format based on mlp for use on Blu-Ray discs."); // http://en.wikipedia.org/wiki/Dolby_TrueHD
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
//     else if( codecName == "truespeech" )
//     {
//         info.lossless = false;
//         info.description = i18n("Truespeech is a propritary speech codec for low bitrates."); // http://en.wikipedia.org/wiki/Truespeech
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
    else if( codecName == "tta" )
    {
        info.lossless = true;
        info.description = i18n("True Audio is a free lossless audio format."); // http://en.wikipedia.org/wiki/TTA_(codec)
        info.mimeTypes.append( "audio/x-tta" );
        info.extensions.append( "tta" );
    }
    else if( codecName == "wavpack" )
    {
        info.lossless = true;
        info.description = i18n("WavPack is a free and lossless audio codec.\nFor more information see: http://www.wavpack.com");
        info.mimeTypes.append( "audio/x-wavpack" );
        info.extensions.append( "wv" );
        info.extensions.append( "wvp" );
    }
    else if( codecName == "ra" )
    {
        info.lossless = false;
        info.description = i18n("Real Media Audio is a propritary and lossy codec.");
        info.mimeTypes.append( "audio/vnd.rn-realaudio" );
        info.extensions.append( "ra" );
        info.extensions.append( "rax" );
    }
    else if( codecName == "3gp" )
    {
        info.lossless = false;
        info.description = i18n("3GP is a audio/video container format for mobile devices."); // http://de.wikipedia.org/wiki/3gp
        info.mimeTypes.append( "video/3gpp" );
        info.mimeTypes.append( "audio/3gpp" );
        info.mimeTypes.append( "video/3gpp2" );
        info.mimeTypes.append( "audio/3gpp2" );
        info.extensions.append( "3gp" );
        info.extensions.append( "3g2" );
        info.extensions.append( "3gpp" );
        info.extensions.append( "3ga" );
        info.extensions.append( "3gp2" );
        info.extensions.append( "3gpp2" );
    }
    else if( codecName == "rm" )
    {
        info.lossless = false;
        info.description = i18n("Real Media is a propritary and lossy codec.");
        info.mimeTypes.append( "application/vnd.rn-realmedia" );
        info.extensions.append( "rm" );
        info.extensions.append( "rmj" );
        info.extensions.append( "rmm" );
        info.extensions.append( "rms" );
        info.extensions.append( "rmvb" );
        info.extensions.append( "rmx" );
    }
    else if( codecName == "avi" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/x-msvideo" );
        info.extensions.append( "avi" );
        info.extensions.append( "divx" );
    }
    else if( codecName == "mkv" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/x-matroska" );
        info.extensions.append( "mkv" );
    }
    else if( codecName == "ogv" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/ogg" );
        info.extensions.append( "ogv" );
    }
    else if( codecName == "mpeg" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/mpeg" );
        info.extensions.append( "mpg" );
        info.extensions.append( "mpeg" );
        info.extensions.append( "m2t" );
        info.extensions.append( "m2ts" );
        info.extensions.append( "mod" );
        info.extensions.append( "mp2" );
        info.extensions.append( "mpe" );
        info.extensions.append( "mts" );
        info.extensions.append( "ts" );
        info.extensions.append( "vob" );
    }
    else if( codecName == "mov" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/quicktime" );
        info.extensions.append( "mov" );
        info.extensions.append( "moov" );
        info.extensions.append( "qt" );
        info.extensions.append( "qtvr" );
    }
    else if( codecName == "flv" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/x-flv" );
        info.mimeTypes.append( "video/flv" );
        info.extensions.append( "flv" );
    }
    else if( codecName == "wmv" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/x-ms-wmv" );
        info.mimeTypes.append( "video/x-ms-asf" );
        info.extensions.append( "wmv" );
        info.extensions.append( "asf" );
    }
    else if( codecName == "rv" )
    {
        info.lossless = false;
        info.mimeTypes.append( "video/vnd.rn-realvideo" );
        info.extensions.append( "rv" );
        info.extensions.append( "rvx" );
    }
    else if( codecName == "midi" )
    {
        info.lossless = false;
        info.description = i18n("midi is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
        info.mimeTypes.append( "audio/midi" );
        info.extensions.append( "midi" );
        info.extensions.append( "mid" );
        info.extensions.append( "kar" );
    }
    else if( codecName == "mod" )
    {
        info.lossless = false;
        info.description = i18n("Amiga-SoundTracker-Audio is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
        info.mimeTypes.append( "audio/x-mod" );
        info.extensions.append( "669" );
        info.extensions.append( "m15" );
        info.extensions.append( "med" );
        info.extensions.append( "mod" );
        info.extensions.append( "mtm" );
        info.extensions.append( "ult" );
        info.extensions.append( "uni" );
    }
    else if( codecName == "sad" )
    {
        info.lossless = false;
//         info.description = i18n("Black & White audio files where used for the game Black & White and contain an MP2 stream.");
        info.extensions.append( "sad" );
    }

    return info;
}

void BackendPlugin::scanForBackends( const QStringList& directoryList )
{
    for( QMap<QString, QString>::Iterator a = binaries.begin(); a != binaries.end(); ++a )
    {
        a.value() = KStandardDirs::findExe( a.key() );
        if( a.value().isEmpty() )
        {
            for( QList<QString>::const_iterator b = directoryList.begin(); b != directoryList.end(); ++b )
            {
                if( QFile::exists((*b) + "/" + a.key()) )
                {
                    a.value() = (*b) + "/" + a.key();
                    break;
                }
            }
        }
    }
}

QString BackendPlugin::getCodecFromFile( const KUrl& filename, const QString& mimeType )
{
    for( int i=0; i<allCodecs.count(); i++ )
    {
        if( formatInfo(allCodecs.at(i)).mimeTypes.indexOf(mimeType) != -1 )
        {
            return allCodecs.at(i);
        }
    }

    const QString extension = filename.url().mid( filename.url().lastIndexOf(".") + 1 );

    for( int i=0; i<allCodecs.count(); i++ )
    {
        if( formatInfo(allCodecs.at(i)).extensions.indexOf(extension) != -1 )
        {
            return allCodecs.at(i);
        }
    }

    return "";
}

bool BackendPlugin::pause( int id )
{
    Q_UNUSED(id)

    return false;
}

bool BackendPlugin::resume( int id )
{
    Q_UNUSED(id)

    return false;
}

bool BackendPlugin::kill( int id )
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->id == id && backendItems.at(i)->process != 0 )
        {
            backendItems.at(i)->process->kill();
            emit log( id, i18n("Killing process on user request") );
            return true;
        }
    }
    return false;
}

float BackendPlugin::progress( int id )
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->id == id )
        {
            return backendItems.at(i)->progress;
        }
    }
    return 0.0f;
}

// void BackendPlugin::setPriority( int _priority )
// {
//     priority = _priority;
//
//     for( int i=0; i<backendItems.size(); i++ )
//     {
//         backendItems.at(i)->process->setPriority( priority );
//     }
// }

void BackendPlugin::processOutput()
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            const QString output = backendItems.at(i)->process->readAllStandardOutput().data();

            const float progress = parseOutput( output );

            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            if( progress == -1 )
                emit log( backendItems.at(i)->id, output );

            return;
        }
    }
}

void BackendPlugin::processExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)

    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            emit jobFinished( backendItems.at(i)->id, exitCode );

            backendItems.at(i)->deleteLater();
            backendItems.removeAt(i);

            return;
        }
    }
}

QString BackendPlugin::standardMessage( const QString& type, const QStringList& arguments )
{
    if( type == "encode_codec,backend" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "In order to encode %1 files, you need to install '%2'.", arguments.at(0), arguments.at(1) );
    }
    if( type == "decode_codec,backend" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "In order to decode %1 files, you need to install '%2'.", arguments.at(0), arguments.at(1) );
    }
    if( type == "transcode_codec,backend" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "In order to transcode %1 files, you need to install '%2'.", arguments.at(0), arguments.at(1) );
    }
    if( type == "replygain_codec,backend" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "In order to calculate Replay Gain tags for %1 files, you need to install '%2'.", arguments.at(0), arguments.at(1) );
    }
    if( type == "install_opensource_backend" )
    {
        if( arguments.count() != 1 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "'%1' should be shipped with your distribution.", arguments.at(0) );
    }
    if( type == "install_patented_backend" )
    {
        if( arguments.count() != 1 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "Since '%1' inludes patented codecs, it may not be included in the default installation of your distribution. Many distributions offer '%1' in an additional software repository.", arguments.at(0) );
    }
    if( type == "install_website_backend,url" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "You can download '%1' at %2", arguments.at(0), arguments.at(1) );
    }

    return QString();
}

QString BackendPlugin::standardMessage( const QString& type )
{
    return standardMessage( type, QStringList() );
}

QString BackendPlugin::standardMessage( const QString& type, const QString& argument1 )
{
    return standardMessage( type, QStringList(argument1) );
}

QString BackendPlugin::standardMessage(const QString& type, const QString& argument1, const QString& argument2)
{
    QStringList arguments;
    arguments.append( argument1 );
    arguments.append( argument2 );
    return standardMessage( type, arguments );
}

QString BackendPlugin::standardMessage(const QString& type, const QString& argument1, const QString& argument2, const QString& argument3)
{
    QStringList arguments;
    arguments.append( argument1 );
    arguments.append( argument2 );
    arguments.append( argument3 );
    return standardMessage( type, arguments );
}

/// see http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_03.html
QString BackendPlugin::escapeUrl( const KUrl& url )
{
    // if no file name is given, assume we are using pipes
    if( url.isEmpty() )
        return "-";

    return url.toLocalFile().replace("\"","\\\"").replace("$","\\$").replace("`","\\`");
}

