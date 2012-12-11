
#include "backendplugin.h"

#include <QFile>
#include <KStandardDirs>


BackendPluginItem::BackendPluginItem( QObject *parent )
    : QObject( parent )
{
    process = 0;
    id = 0;
    progress = -1;
}

BackendPluginItem::~BackendPluginItem()
{}


BackendPlugin::BackendPlugin( QObject *parent )
    : QObject( parent )
{
    lastId = 100;
}

BackendPlugin::~BackendPlugin()
{}

BackendPlugin::FormatInfo BackendPlugin::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;
    info.priority = 100;
    info.lossless = false;
    info.inferiorQuality = false;

    if( codecName == "wav" )
    {
        info.lossless = true;
        info.description = i18n("Wave is a very simple audio format that doesn't compress the audio stream so it's lossless but the file size is very big and it doesn't support advanced features like tags.<br>It is supported by virtually any software or device.<br><br><a href=\"http://en.wikipedia.org/wiki/WAV\">http://en.wikipedia.org/wiki/WAV</a>");
        info.mimeTypes.append( "audio/x-wav" );
        info.mimeTypes.append( "audio/wav" );
        info.extensions.append( "wav" );
    }
    else if( codecName == "aac" )
    {
        info.description = i18n("Advanced Audio Coding (AAC) is a lossy and popular audio format. It is widely used by Apple Inc.<br>In soundKonverter \"aac\" refers to only the codec. In order to get a file with tagging capabilities use \"m4a/aac\".<br><br><a href=\"http://en.wikipedia.org/wiki/Advanced_Audio_Coding\">http://en.wikipedia.org/wiki/Advanced Audio Coding</a>");
        info.mimeTypes.append( "audio/aac" );
        info.extensions.append( "aac" );
    }
    else if( codecName == "m4a/aac" )
    {
        info.priority = 90;
        info.description = i18n("Advanced Audio Coding (AAC) is a lossy and popular audio format. It is widely used by Apple Inc.<br>In soundKonverter \"m4a/aac\" refers to a MPEG-4 audio file encoded with AAC.<br><br><a href=\"http://en.wikipedia.org/wiki/Advanced_Audio_Coding\">http://en.wikipedia.org/wiki/Advanced Audio Coding</a>");
        info.mimeTypes.append( "audio/mp4" );
        info.mimeTypes.append( "audio/x-m4a" );
        info.extensions.append( "m4a" );
        info.extensions.append( "f4a" );
        info.extensions.append( "aac" );
        info.extensions.append( "mp4" );
    }
    else if( codecName == "m4a/alac" )
    {
        info.priority = 80;
        info.lossless = true;
        info.description = i18n("Apple Lossless Audio Codec (ALAC) is a lossless audio format. It is widely used by Apple Inc.<br>In soundKonverter \"m4a/alac\" refers to a MPEG-4 audio file encoded with ALAC.<br><br><a href=\"http://en.wikipedia.org/wiki/Apple_Lossless\">http://en.wikipedia.org/wiki/Apple Lossless</a>");
        info.mimeTypes.append( "audio/mp4" );
        info.mimeTypes.append( "audio/x-m4a" );
        info.extensions.append( "m4a" );
        info.extensions.append( "f4a" );
        info.extensions.append( "alac" );
        info.extensions.append( "mp4" );
    }
    else if( codecName == "mp4" )
    {
        // info.description = i18n("mp4 files are video files with an aac encoded audio stream. It is widely used by Apple Inc.");
        info.mimeTypes.append( "video/mp4" );
        info.extensions.append( "mp4" );
        info.extensions.append( "m4v" );
        info.extensions.append( "f4v" );
    }
    else if( codecName == "ogg vorbis" )
    {
        info.description = i18n("Ogg Vorbis is a lossy and popular audio format.<br>It is free of any legal restrictions.<br><br><a href=\"http://en.wikipedia.org/wiki/Vorbis\">http://en.wikipedia.org/wiki/Vorbis</a>");
        info.mimeTypes.append( "application/ogg" );
        info.mimeTypes.append( "audio/vorbis" );
        info.mimeTypes.append( "application/x-ogg" );
        info.mimeTypes.append( "audio/ogg" );
        info.mimeTypes.append( "audio/x-vorbis+ogg" );
        info.extensions.append( "ogg" );
        info.extensions.append( "oga" );
    }
    else if( codecName == "opus" )
    {
        info.description = i18n("Opus is a lossy audio format designed for real time communication over the internet.<br>It can cover a wide range of bitrates with a high quality and it is free of any legal restrictions.<br><br><a href=\"http://en.wikipedia.org/wiki/Opus_(audio_format)\">http://en.wikipedia.org/wiki/Opus (audio format)</a>");
        info.mimeTypes.append( "audio/ogg" );
        info.mimeTypes.append( "audio/opus" );
        info.extensions.append( "opus" );
        info.extensions.append( "oga" );
    }
    else if( codecName == "mp3" )
    {
        info.description = i18n("MPEG-1 or MPEG-2 Audio Layer III (MP3) is the most popular lossy audio format.<br>It is supported by virtually all audio players even though it has some legal restrictions.<br><br><a href=\"http://en.wikipedia.org/wiki/MP3\">http://en.wikipedia.org/wiki/MP3</a>");
        info.mimeTypes.append( "audio/x-mp3" );
        info.mimeTypes.append( "audio/mpeg" );
        info.mimeTypes.append( "audio/mp3" );
        info.extensions.append( "mp3" );
    }
    else if( codecName == "flac" )
    {
        info.lossless = true;
        info.description = i18n("Free Lossless Audio Codec (FLAC) is a lossless audio format.<br>It is free of any legal restrictions.<br><br><a href=\"http://en.wikipedia.org/wiki/FLAC\">http://en.wikipedia.org/wiki/FLAC</a>");
        info.mimeTypes.append( "audio/flac" );
        info.mimeTypes.append( "audio/x-flac" );
        info.mimeTypes.append( "audio/x-flac+ogg" );
        info.mimeTypes.append( "audio/x-oggflac" );
        info.extensions.append( "flac" );
        info.extensions.append( "fla" );
    }
    else if( codecName == "wma" )
    {
        info.description = i18n("Windows Media Audio (WMA) is a lossy audio format developed by Microsoft.<br><br><a href=\"http://en.wikipedia.org/wiki/Windows_Media_Audio\">http://en.wikipedia.org/wiki/Windows Media Audio</a>");
        info.mimeTypes.append( "audio/x-ms-wma" );
        info.extensions.append( "wma" );
    }
    else if( codecName == "ac3" )
    {
        info.description = i18n("AC3 or AC-3 is part of the Dolby Digital format family.<br>It was designed to be used for movies (e.g. DVDs) and supports sourround sound.<br><br><a href=\"http://en.wikipedia.org/wiki/Dolby_Digital\">http://en.wikipedia.org/wiki/Dolby Digital</a>");
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
    else if( codecName == "mp2" )
    {
        info.description = i18n("MPEG-1 or MPEG-2 Audio Layer II (MP2) is an old lossy audio format.<br><br><a href=\"http://en.wikipedia.org/wiki/MPEG-1_Audio_Layer_II\">http://en.wikipedia.org/wiki/MPEG-1 Audio Layer II</a>");
        info.mimeTypes.append( "audio/mp2" );
        info.mimeTypes.append( "audio/x-mp2" );
        info.extensions.append( "mp2" );
    }
//     else if( codecName == "sonic" )
//     {
//         info.lossless = false;
//         info.description = i18n("Sonic");
//     }
//     else if( codecName == "sonicls" )
//     {
//         info.lossless = true;
//         info.description = i18n("Sonic Lossless");
//     }
    else if( codecName == "als" ) // TODO mime type, etc.
    {
        info.lossless = true;
        // info.description = i18n("MPEG-4 Audio Lossless Coding"); // http://en.wikipedia.org/wiki/Audio_Lossless_Coding
        info.mimeTypes.append( "audio/mp4" );
        info.extensions.append( "mp4" );
    }
    else if( codecName == "amr nb" )
    {
        info.inferiorQuality = true;
        info.description = i18n("Adaptive Multi-Rate Narrow-Band (AMR NB) is mainly used for speech compression for mobile communication.<br><br><a href=\"http://en.wikipedia.org/wiki/Adaptive_Multi-Rate_audio_codec\">http://en.wikipedia.org/wiki/Adaptive Multi-Rate audio codec</a>");
        info.mimeTypes.append( "audio/amr" );
        info.mimeTypes.append( "audio/3gpp" );
        info.mimeTypes.append( "audio/3gpp2" );
        info.extensions.append( "amr" );
    }
    else if( codecName == "amr wb" )
    {
        info.inferiorQuality = true;
        info.description = i18n("Adaptive Multi-Rate Wide-Band (AMR WB) is an advanced version of AMR NB which uses a higher data rate resulting in a higher quality.<br>Still it should be used only for speech compression.<br><br><a href=\"http://en.wikipedia.org/wiki/Adaptive_Multi-Rate_Wideband\">http://en.wikipedia.org/wiki/Adaptive Multi-Rate Wideband</a>");
        info.mimeTypes.append( "audio/amr-wb" );
        info.mimeTypes.append( "audio/3gpp" );
        info.extensions.append( "awb" );
    }
    else if( codecName == "ape" )
    {
        info.lossless = true;
        info.description = i18n("Monkey's Audio is a lossless audio format.<br><br><a href=\"http://en.wikipedia.org/wiki/Monkey's_Audio\">http://en.wikipedia.org/wiki/Monkey's Audio</a>");
        info.mimeTypes.append( "audio/x-ape" );
        info.extensions.append( "ape" );
        info.extensions.append( "mac" );
        info.extensions.append( "apl" );
    }
    else if( codecName == "speex" )
    {
        info.inferiorQuality = true;
        info.description = i18n("Speex is a lossy audio format designed for speech encoding.<br>It is free of any legal restrictions. Since the release of Opus, Speex is considered obsolete.<br><br><a href=\"http://en.wikipedia.org/wiki/Speex\">http://en.wikipedia.org/wiki/Speex<a/>");
        info.mimeTypes.append( "audio/x-speex" );
        info.mimeTypes.append( "audio/x-speex+ogg" );
        info.extensions.append( "spx" );
    }
    else if( codecName == "mp1" )
    {
        // info.description = i18n("MPEG-1 Audio Layer I very old and lossy file format."); // http://en.wikipedia.org/wiki/MP1
        info.mimeTypes.append( "audio/mpeg" );
        info.mimeTypes.append( "audio/mpa" );
        info.extensions.append( "mp1" );
    }
    else if( codecName == "musepack" )
    {
        info.description = i18n("Musepack (MPC) is a free and lossy file format based on mp2 and optimized for high quality.<br><br><a href=\"http://en.wikipedia.org/wiki/Musepack\">http://en.wikipedia.org/wiki/Musepack<a/>");
        info.mimeTypes.append( "audio/x-musepack" );
        info.mimeTypes.append( "audio/musepack" );
        info.extensions.append( "mpc" );
        info.extensions.append( "mp+" );
        info.extensions.append( "mpp" );
    }
    else if( codecName == "shorten" )
    {
        info.lossless = true;
        info.description = i18n("Shorten (SHN) is a free and lossless audio format.<br><br><a href=\"http://en.wikipedia.org/wiki/Shorten\">http://en.wikipedia.org/wiki/Shorten</a>");
        info.mimeTypes.append( "application/x-shorten" );
        info.extensions.append( "shn" );
    }
//     else if( codecName == "mlp" )
//     {
//         info.lossless = true;
//         info.description = i18n("Meridian Lossless Packing is an old propritary lossless audio format."); // http://en.wikipedia.org/wiki/Meridian_Lossless_Packing
//     }
//     else if( codecName == "truehd" )
//     {
//         info.lossless = true;
//         info.description = i18n("Dolby TrueHD is a lossless audio format based on mlp for use on Blu-Ray discs."); // http://en.wikipedia.org/wiki/Dolby_TrueHD
//     }
//     else if( codecName == "truespeech" )
//     {
//         info.lossless = false;
//         info.description = i18n("Truespeech is a propritary speech codec for low bitrates."); // http://en.wikipedia.org/wiki/Truespeech
//     }
    else if( codecName == "tta" )
    {
        info.lossless = true;
        info.description = i18n("True Audio (TTA) is a free and lossless audio format.<br><br><a href=\"http://en.wikipedia.org/wiki/TTA_(codec)\">http://en.wikipedia.org/wiki/TTA (codec)<a/>");
        info.mimeTypes.append( "audio/x-tta" );
        info.extensions.append( "tta" );
    }
    else if( codecName == "wavpack" )
    {
        info.lossless = true;
        info.description = i18n("WavPack (WV) is a free and lossless audio format.<br><br><a href=\"http://en.wikipedia.org/wiki/WavPack\">http://en.wikipedia.org/wiki/WavPack<a/>");
        info.mimeTypes.append( "audio/x-wavpack" );
        info.extensions.append( "wv" );
        info.extensions.append( "wvp" );
    }
    else if( codecName == "ra" )
    {
        // info.description = i18n("Real Media Audio is a proprietary and lossy codec.");
        info.mimeTypes.append( "audio/vnd.rn-realaudio" );
        info.extensions.append( "ra" );
        info.extensions.append( "rax" );
    }
    else if( codecName == "3gp" )
    {
        // info.description = i18n("3GP is a audio/video container format for mobile devices."); // http://de.wikipedia.org/wiki/3gp
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
        // info.description = i18n("Real Media is a proprietary and lossy codec.");
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
        info.mimeTypes.append( "video/x-msvideo" );
        info.extensions.append( "avi" );
        info.extensions.append( "divx" );
    }
    else if( codecName == "mkv" )
    {
        info.mimeTypes.append( "video/x-matroska" );
        info.extensions.append( "mkv" );
    }
    else if( codecName == "ogv" )
    {
        info.mimeTypes.append( "video/ogg" );
        info.extensions.append( "ogv" );
    }
    else if( codecName == "mpeg" )
    {
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
        info.mimeTypes.append( "video/quicktime" );
        info.extensions.append( "mov" );
        info.extensions.append( "moov" );
        info.extensions.append( "qt" );
        info.extensions.append( "qtvr" );
    }
    else if( codecName == "flv" )
    {
        info.mimeTypes.append( "video/x-flv" );
        info.mimeTypes.append( "video/flv" );
        info.extensions.append( "flv" );
    }
    else if( codecName == "wmv" )
    {
        info.mimeTypes.append( "video/x-ms-wmv" );
        info.mimeTypes.append( "video/x-ms-asf" );
        info.extensions.append( "wmv" );
        info.extensions.append( "asf" );
    }
    else if( codecName == "rv" )
    {
        info.mimeTypes.append( "video/vnd.rn-realvideo" );
        info.extensions.append( "rv" );
        info.extensions.append( "rvx" );
    }
    else if( codecName == "midi" )
    {
        info.inferiorQuality = true;
        // info.description = i18n("midi is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
        info.mimeTypes.append( "audio/midi" );
        info.extensions.append( "midi" );
        info.extensions.append( "mid" );
        info.extensions.append( "kar" );
    }
    else if( codecName == "mod" )
    {
        info.inferiorQuality = true;
        // info.description = i18n("Amiga-SoundTracker-Audio is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
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
        info.inferiorQuality = true;
        // info.description = i18n("Black & White audio files where used for the game Black & White and contain an MP2 stream.");
        info.extensions.append( "sad" );
    }
    else if( codecName == "8svx" )
    {
        info.inferiorQuality = true;
        info.description = i18n("8-Bit Sampled Voice (8SVX) is an audio file format standard developed by Electronic Arts for the Commodore-Amiga computer series.<br><br><a href=\"http://en.wikipedia.org/wiki/8SVX\">http://en.wikipedia.org/wiki/8SVX</a>");
        info.mimeTypes.append( "audio/x-8svx" );
        info.mimeTypes.append( "audio/8svx" );
        info.mimeTypes.append( "audio/x-aiff" );
        info.mimeTypes.append( "audio/aiff" );
        info.extensions.append( "8svx" );
        info.extensions.append( "iff" );
    }
    else if( codecName == "aiff" )
    {
        info.lossless = true;
        info.description = i18n("Audio Interchange File Format (AIFF) is a very simple audio format that doesn't compress the audio stream so it's lossless but the file size is very big and it doesn't support advanced features like tags.<br><br><a href=\"http://en.wikipedia.org/wiki/Audio_Interchange_File_Format\">http://en.wikipedia.org/wiki/Audio Interchange File Format</a>");
        info.mimeTypes.append( "audio/x-aiff" );
        info.mimeTypes.append( "audio/aiff" );
        info.extensions.append( "aiff" ); // aiff has meta data, aif not
        info.extensions.append( "aif" );
    }
//     else if( codecName == "aiff-c" ) // sox only supports uncompressed aiff-c
//     {
//         info.lossless = true;
//         info.description = i18n("AIFF-C is a compressed version of AIFF. There are different compression types available.");
//         info.mimeTypes.append( "audio/x-aiff" );
//         info.mimeTypes.append( "audio/aiff" );
//         info.extensions.append( "aifc" );
//         info.extensions.append( "aiffc" );
//     }

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

QString BackendPlugin::getCodecFromFile( const KUrl& filename, const QString& mimeType, short *rating )
{
    Q_UNUSED(filename)
    Q_UNUSED(mimeType)
    Q_UNUSED(rating)

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
            emit log( id, "<pre>\t" + i18n("Killing process on user request") + "</pre>" );
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
                logOutput( backendItems.at(i)->id, output );

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

        return i18n( "Since '%1' includes patented codecs, it may not be included in the default installation of your distribution. Many distributions offer '%1' in an additional software repository.", arguments.at(0) );
    }
    if( type == "install_website_backend,url" )
    {
        if( arguments.count() != 2 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        return i18n( "You can download '%1' at %2", arguments.at(0), arguments.at(1) );
    }
    if( type == "filter,backend" )
    {
        if( arguments.count() != 3 )
            return "BackendPlugin::standardMessage (type: '"+type+"') called with wrong 'arguments' count!";

        if( arguments.at(1) == "single" )
        {
            return i18n( "In order to use the filter %1, you need to install '%2'.", arguments.at(0), arguments.at(2) );
        }
        else
        {
            return i18n( "In order to use the filters %1, you need to install '%2'.", arguments.at(0), arguments.at(2) );
        }
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

void BackendPlugin::logOutput( int id, const QString& message )
{
    emit log( id, "<pre>\t<span style=\"color:#C00000\">" + message.trimmed().replace("\n","<br>\t") + "</span></pre>" );
}

void BackendPlugin::logCommand( int id, const QString& message )
{
    emit log( id, "<pre>\t<span style=\"color:#DC6300\">" + message.trimmed().replace("\n","<br>\t") + "</span></pre>" );
}

