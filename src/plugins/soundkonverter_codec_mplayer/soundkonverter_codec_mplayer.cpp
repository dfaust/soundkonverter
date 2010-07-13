
#include "mplayercodecglobal.h"

#include "soundkonverter_codec_mplayer.h"
#include "../../core/conversionoptions.h"
#include "mplayercodecwidget.h"


soundkonverter_codec_mplayer::soundkonverter_codec_mplayer( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    binaries["mplayer"] = "";
    
    // encoders
    codecMap["wav"] = "pcm_s16le";
    codecMap["ogg vorbis"] = "libvorbis"; // vorbis
    codecMap["mp3"] = "libmp3lame";
    codecMap["flac"] = "flac";
    codecMap["wma"] = "wmav2";
    codecMap["aac"] = "libfaac"; // aac
    codecMap["ac3"] = "ac3";
    codecMap["alac"] = "alac";
    codecMap["mp2"] = "mp2";
//     codecMap["sonic"] = "sonic";
//     codecMap["sonic lossless"] = "sonicls";
    codecMap["amr nb"] = "libopencore_amrnb";
}

soundkonverter_codec_mplayer::~soundkonverter_codec_mplayer()
{}

QString soundkonverter_codec_mplayer::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_mplayer::codecTable()
{
    QList<ConversionPipeTrunk> table;
    
    // encode
    fromCodecs += "wav";
    fromCodecs += "ogg vorbis";
    fromCodecs += "mp3";
    fromCodecs += "flac";
    fromCodecs += "wma";
    fromCodecs += "aac";
    fromCodecs += "ac3";
    fromCodecs += "alac";
    fromCodecs += "mp2";
//     fromCodecs += "sonic";
//     fromCodecs += "sonic lossless";
    fromCodecs += "als";
    fromCodecs += "amr nb";
    // decode
    fromCodecs += "amr wb";
    fromCodecs += "ape";
//     fromCodecs += "eac3";
    fromCodecs += "speex";
    fromCodecs += "mp1";
    fromCodecs += "mpc";
    fromCodecs += "shorten";
//     fromCodecs += "mlp";
//     fromCodecs += "truehd";
//     fromCodecs += "truespeech";
    fromCodecs += "tta";
    fromCodecs += "wavpack";
    // containers
    fromCodecs += "3gp";

    toCodecs += "wav";
    toCodecs += "ogg vorbis";
    toCodecs += "mp3";
    toCodecs += "flac";
    toCodecs += "wma";
    toCodecs += "aac";
    toCodecs += "ac3";
    toCodecs += "alac";
    toCodecs += "mp2";
//     toCodecs += "sonic";
//     toCodecs += "sonic lossless";
    toCodecs += "amr nb";
    
    for( int i=0; i<fromCodecs.count(); i++ )
    {
        for( int j=0; j<toCodecs.count(); j++ )
        {
            if( fromCodecs.at(i) == "wav" && toCodecs.at(j) == "wav" ) continue;
          
            ConversionPipeTrunk newTrunk;
            newTrunk.codecFrom = fromCodecs.at(i);
            newTrunk.codecTo = toCodecs.at(j);
            newTrunk.rating = 90;
            newTrunk.enabled = ( binaries["mplayer"] != "" );
            newTrunk.problemInfo = i18n("You need to install 'mplayer'. Since mplayer inludes many patented codecs, it may not be included in the default installation of your distribution. Many distributions offer mplayer in an additional software repository.");
            newTrunk.data.hasInternalReplayGain = false;
            table.append( newTrunk );
        }
    }

    QSet<QString> codecs;
    codecs += QSet<QString>::fromList(fromCodecs);
    codecs += QSet<QString>::fromList(toCodecs);
    allCodecs = codecs.toList();
    
    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_mplayer::formatInfo( const QString& codecName )
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
    }
    else if( codecName == "mp3" )
    {
        info.lossless = false;
        info.description = i18n("MP3 is a very popular lossy audio codec.");
        info.mimeTypes.append( "audio/x-mp3" );
        info.mimeTypes.append( "audio/mpeg" );
        info.mimeTypes.append( "audio/mp3" );
        info.extensions.append( "mp3" );
    }
    else if( codecName == "flac" )
    {
        info.lossless = true;
        info.description = i18n("Flac is the free lossless audio codec.\nAs it name says, it compresses without any loss.");
        info.mimeTypes.append( "audio/x-flac" );
        info.mimeTypes.append( "audio/x-flac+ogg" );
        info.mimeTypes.append( "audio/x-oggflac" );
        info.extensions.append( "flac" );
        info.extensions.append( "fla" );
        info.extensions.append( "ogg" );
    }
    else if( codecName == "wma" )
    {
        info.lossless = false;
        info.description = i18n("Windows Media Audio is a propritary audio codec from Microsoft.");
        info.mimeTypes.append( "audio/x-ms-wma" );
        info.extensions.append( "wma" );
    }
    else if( codecName == "aac" )
    {
        info.lossless = false;
        info.description = i18n("Advanced Audio Coding is a lossy and popular audio format."); // http://en.wikipedia.org/wiki/Advanced_Audio_Coding
        info.mimeTypes.append( "audio/aac" );
        info.mimeTypes.append( "audio/aacp" );
        info.mimeTypes.append( "audio/mp4" );
        info.extensions.append( "aac" );
        info.extensions.append( "3gp" );
        info.extensions.append( "mp4" );
        info.extensions.append( "m4a" );
    }
    else if( codecName == "ac3" ) // TODO description
    {
        info.lossless = false;
        info.description = i18n("Dolby Digital-Audio"); // http://en.wikipedia.org/wiki/Ac3
        info.mimeTypes.append( "audio/ac3" );
        info.extensions.append( "ac3" );
    }
    else if( codecName == "alac" )
    {
        info.lossless = true;
        info.description = i18n("Apple Lossless Audio Codec is a lossless audio format from Apple."); // http://en.wikipedia.org/wiki/Alac
//         info.mimeTypes.append( "audio/x-ms-wma" );
        info.extensions.append( "m41" );
    }
    else if( codecName == "mp2" )
    {
        info.lossless = false;
        info.description = i18n("MPEG-1 Audio Layer II is an old lossy audio format."); // http://en.wikipedia.org/wiki/MPEG-1_Audio_Layer_II
        info.mimeTypes.append( "audio/mpeg" );
        info.extensions.append( "mp2" );
    }
//     else if( codecName == "sonic" ) // TODO description
//     {
//         info.lossless = false;
//         info.description = i18n("Sonic");
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
//     else if( codecName == "sonicls" ) // TODO description
//     {
//         info.lossless = true;
//         info.description = i18n("Sonic Lossless");
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
    else if( codecName == "als" ) // TODO description
    {
        info.lossless = true;
        info.description = i18n("MPEG-4 Audio Lossless Coding");
//         info.mimeTypes.append( "audio/x-ms-wma" );
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
        info.description = i18n("Monkey's Audio is a propritary lossless audio format."); // http://en.wikipedia.org/wiki/Monkey's_Audio
        info.mimeTypes.append( "audio/x-ape" );
        info.extensions.append( "ape" );
        info.extensions.append( "apl" );
    }
//     else if( codecName == "eac3" ) // TODO description
//     {
//         info.lossless = false;
//         info.description = i18n("Dolby Digital Plus (Enhanced AC-3) is an advanced version of AC-3 for use on Blu-Ray discs.");
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
    else if( codecName == "speex" )
    {
        info.lossless = false;
        info.description = i18n("Speex is a free and lossy audio codec designed for encoding speech."); // http://en.wikipedia.org/wiki/Speex
        info.mimeTypes.append( "audio/speex" );
        info.mimeTypes.append( "audio/ogg" );
        info.extensions.append( "spx" );
    }
    else if( codecName == "mp1" )
    {
        info.lossless = false;
        info.description = i18n("MPEG-1 Audio Layer I very old and lossy file format."); // http://en.wikipedia.org/wiki/MP1
        info.mimeTypes.append( "audio/mpeg" );
        info.extensions.append( "mp1" );
    }
    else if( codecName == "mpc" )
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
        info.description = i18n("Shorten is an old lossless audio format."); // http://en.wikipedia.org/wiki/Shorten
        info.mimeTypes.append( "audio/x-ms-wma" );
        info.extensions.append( "wma" );
    }
//     else if( codecName == "mlp" ) // TODO description
//     {
//         info.lossless = true;
//         info.description = i18n("Meridian Lossless Packing is an old propritary lossless audio format."); // http://en.wikipedia.org/wiki/Meridian_Lossless_Packing
// //         info.mimeTypes.append( "audio/x-ms-wma" );
// //         info.extensions.append( "wma" );
//     }
//     else if( codecName == "truehd" ) // TODO description
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
        info.description = i18n("WavPack is a free lossless audio format."); // http://en.wikipedia.org/wiki/WavPack
        info.mimeTypes.append( "audio/x-wavpack" );
        info.extensions.append( "wv" );
        info.extensions.append( "wvp" );
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

    return info;
}


// QString soundkonverter_codec_mplayer::getCodecFromFile( const KUrl& filename, const QString& mimeType )
// {
//     for( int i=0; i<allCodecs.count(); i++ )
//     {
//         if( formatInfo(allCodecs.at(i)).mimeTypes.indexOf(mimeType) != -1 )
//         {
//             return allCodecs.at(i);
//         }
//     }
//     
//     QString extension = filename.url().right( filename.url().length() - filename.url().lastIndexOf(".") - 1 );
// 
//     for( int i=0; i<allCodecs.count(); i++ )
//     {
//         if( formatInfo(allCodecs.at(i)).extensions.indexOf(extension) != -1 )
//         {
//             return allCodecs.at(i);
//         }
//     }
//         
//     return "";
// }

bool soundkonverter_codec_mplayer::isConfigSupported( ActionType action )
{
    return false;
}

void soundkonverter_codec_mplayer::showConfigDialog( ActionType action, const QString& format, QWidget *parent )
{}

bool soundkonverter_codec_mplayer::hasInfo()
{
    return false;
}

void soundkonverter_codec_mplayer::showInfo()
{}

QWidget *soundkonverter_codec_mplayer::newCodecWidget()
{
    MPlayerCodecWidget *widget = new MPlayerCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_mplayer::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec != "wav" )
    {
/*        command += binaries["mplayer"];
        command += "-i";
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-acodec";
        command += codecMap[conversionOptions->codecName];
        command += "-ab";
        command += QString::number(conversionOptions->bitrate) + "k";
        if( conversionOptions->samplingRate > 0 )
        {
            command += "-ar";
            command += QString::number(conversionOptions->samplingRate);
        }
        if( conversionOptions->channels > 0 )
        {
            command += "-ac 1";
        }
        command += "\"" + outputFile.toLocalFile() + "\"";*/
    }
    else // NOTE really necessary?
    {
        command += binaries["mplayer"];
        command += "-ao";
        command += "pcm:file=\"" + outputFile.toLocalFile() + "\"";
        command += "\"" + inputFile.toLocalFile() + "\"";
    }

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    emit log( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_codec_mplayer::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( conversionOptions->codecName == "wav" )
    {
        command += "mplayer";
        command += "-i";
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_mplayer::parseOutput( const QString& output, int *length )
{
    // size=    1508kB time=48.25 bitrate= 256.0kbits/s
    
    QString data = output;
    QString time;
    
    QRegExp reg("(\\d{2,}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    if( length && data.contains(reg) )
    {
        *length = reg.cap(1).toInt()*3600 + reg.cap(2).toInt()*60 + reg.cap(3).toInt();
//         emit log( 1000, "got length: " + QString::number(*length) );
    }
    if( data.contains("time") )
    {
        data.remove( 0, data.indexOf("time")+5 );
        time = data.left( data.indexOf(" ") );
        return time.toFloat();
    }
    
    // TODO error handling
    // Error while decoding stream #0.0
    
    return -1;
}

float soundkonverter_codec_mplayer::parseOutput( const QString& output )
{
    return parseOutput( output, 0 );
}

void soundkonverter_codec_mplayer::processOutput()
{
    CodecPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<CodecPluginItem*>(backendItems.at(i));
            progress = parseOutput( output, &pluginItem->data.length );
            if( progress == -1 && !output.simplified().isEmpty() ) emit log( backendItems.at(i)->id, output );
            progress = progress * 100 / pluginItem->data.length;
            if( progress > backendItems.at(i)->progress ) backendItems.at(i)->progress = progress;
            return;
        }
    }
}

#include "soundkonverter_codec_mplayer.moc"

