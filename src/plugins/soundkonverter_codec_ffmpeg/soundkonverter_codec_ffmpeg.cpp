
#include "ffmpegcodecglobal.h"

#include "soundkonverter_codec_ffmpeg.h"
#include "../../core/conversionoptions.h"
#include "ffmpegcodecwidget.h"


// TODO video files

soundkonverter_codec_ffmpeg::soundkonverter_codec_ffmpeg( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["ffmpeg"] = "";

//     // encoders
//     codecMap["wav"] = "pcm_s16le";
//     codecMap["ogg vorbis"] = "libvorbis"; // vorbis
//     codecMap["mp3"] = "libmp3lame";
//     codecMap["flac"] = "flac";
//     codecMap["wma"] = "wmav2";
//     codecMap["aac"] = "libfaac"; // aac - removed from ffmpeg ???
//     codecMap["ac3"] = "ac3";
//     codecMap["alac"] = "alac";
//     codecMap["mp2"] = "mp2";
// //     codecMap["sonic"] = "sonic";
// //     codecMap["sonic lossless"] = "sonicls";
//     codecMap["amr nb"] = "libopencore_amrnb"; // amr
}

soundkonverter_codec_ffmpeg::~soundkonverter_codec_ffmpeg()
{}

QString soundkonverter_codec_ffmpeg::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_ffmpeg::codecTable()
{
    QList<ConversionPipeTrunk> table;

    if( !binaries["ffmpeg"].isEmpty() )
    {
        infoProcess = new KProcess();
        infoProcess.data()->setOutputChannelMode( KProcess::MergedChannels );
        connect( infoProcess.data(), SIGNAL(readyRead()), this, SLOT(infoProcessOutput()) );
        connect( infoProcess.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(infoProcessExit(int,QProcess::ExitStatus)) );

        QStringList command;
        command += binaries["ffmpeg"];
        command += "-codecs";
        infoProcess.data()->clearProgram();
        infoProcess.data()->setShellCommand( command.join(" ") );
        infoProcess.data()->start();

        infoProcess.data()->waitForFinished( 1000 );
    }
    else
    {
        // encoders
        codecMap["wav"] = "pcm_s16le";
        codecMap["ogg vorbis"] = "libvorbis"; // vorbis
        codecMap["mp3"] = "libmp3lame";
        codecMap["flac"] = "flac";
        codecMap["wma"] = "wmav2";
        codecMap["aac"] = "libfaac"; // aac - removed from ffmpeg ???
        codecMap["ac3"] = "ac3";
        codecMap["alac"] = "alac";
        codecMap["mp2"] = "mp2";
//         codecMap["sonic"] = "sonic";
//         codecMap["sonic lossless"] = "sonicls";
        codecMap["amr nb"] = "libopencore_amrnb";
    }

    /// decode
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
    fromCodecs += "amr wb";
    fromCodecs += "ape";
//     fromCodecs += "eac3";
    fromCodecs += "speex";
    fromCodecs += "m4a";
    fromCodecs += "mp1";
    fromCodecs += "mpc";
    fromCodecs += "shorten";
//     fromCodecs += "mlp";
//     fromCodecs += "truehd";
//     fromCodecs += "truespeech";
    fromCodecs += "tta";
    fromCodecs += "wavpack";
    fromCodecs += "ra";
    /// containers
    fromCodecs += "3gp";
    fromCodecs += "rm";
    /// video
    fromCodecs += "avi";
    fromCodecs += "mkv";
    fromCodecs += "ogv";
    fromCodecs += "mpeg";
    fromCodecs += "mov";
    fromCodecs += "mp4";
    fromCodecs += "flv";
    fromCodecs += "wmv";
    fromCodecs += "rv";

    /// encode
    if( !codecMap["wav"].isEmpty() )
        toCodecs += "wav";

    if( !codecMap["ogg vorbis"].isEmpty() )
        toCodecs += "ogg vorbis";

    if( !codecMap["mp3"].isEmpty() )
        toCodecs += "mp3";

    if( !codecMap["flac"].isEmpty() )
        toCodecs += "flac";

    if( !codecMap["wma"].isEmpty() )
        toCodecs += "wma";

    if( !codecMap["aac"].isEmpty() )
        toCodecs += "aac"; // libfaac removed from ffmpeg ???

    if( !codecMap["ac3"].isEmpty() )
        toCodecs += "ac3";

    if( !codecMap["alac"].isEmpty() )
        toCodecs += "alac";

    if( !codecMap["aac"].isEmpty() )
        toCodecs += "m4a"; // libfaac removed from ffmpeg ???

    if( !codecMap["mp2"].isEmpty() )
        toCodecs += "mp2";

//         toCodecs += "sonic";
//         toCodecs += "sonic lossless";

    if( !codecMap["amr nb"].isEmpty() )
        toCodecs += "amr nb";


    for( int i=0; i<fromCodecs.count(); i++ )
    {
        for( int j=0; j<toCodecs.count(); j++ )
        {
            if( fromCodecs.at(i) == "wav" && toCodecs.at(j) == "wav" )
                continue;

            ConversionPipeTrunk newTrunk;
            newTrunk.codecFrom = fromCodecs.at(i);
            newTrunk.codecTo = toCodecs.at(j);
            newTrunk.rating = 90;
            newTrunk.enabled = ( binaries["ffmpeg"] != "" );
            if( toCodecs.at(j) == "wav" )
            {
                newTrunk.problemInfo = standardMessage( "decode_codec,backend", fromCodecs.at(i), "ffmpeg" ) + "\n" + standardMessage( "install_patented_backend", "ffmpeg" );
            }
            else if( fromCodecs.at(i) == "wav" )
            {
                newTrunk.problemInfo = standardMessage( "encode_codec,backend", toCodecs.at(j), "ffmpeg" ) + "\n" + standardMessage( "install_patented_backend", "ffmpeg" );
            }
            else
            {
//                 newTrunk.problemInfo = standardMessage( "transcode_codec,backend", "?", "ffmpeg" ) + "\n" + standardMessage( "install_patented_backend", "ffmpeg" );
            }
//             newTrunk.problemInfo = i18n("You need to install 'ffmpeg'. Since ffmpeg inludes many patented codecs, it may not be included in the default installation of your distribution. Many distributions offer ffmpeg in an additional software repository.");
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


bool soundkonverter_codec_ffmpeg::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_ffmpeg::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_ffmpeg::hasInfo()
{
    return false;
}

void soundkonverter_codec_ffmpeg::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_ffmpeg::newCodecWidget()
{
    FFmpegCodecWidget *widget = new FFmpegCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_ffmpeg::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec != "wav" )
    {
        command += binaries["ffmpeg"];
        command += "-i";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-acodec";
        command += codecMap[conversionOptions->codecName];
        if( outputCodec != "alac" )
        {
            command += "-ab";
            command += QString::number(conversionOptions->bitrate) + "k";
            if(  conversionOptions->samplingRate > 0 )
            {
                command += "-ar";
                command += QString::number(conversionOptions->samplingRate);
            }
            if( conversionOptions->channels > 0 )
            {
                command += "-ac 1";
            }
        }
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["ffmpeg"];
        command += "-i";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
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

QStringList soundkonverter_codec_ffmpeg::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    QStringList command;

    if( outputCodec == "wav" )
    {
        command += binaries["ffmpeg"];
        command += "-i";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_ffmpeg::parseOutput( const QString& output, int *length )
{
    // Duration: 00:02:16.50, start: 0.000000, bitrate: 1411 kb/s
    // size=    2445kB time=00:01:58.31 bitrate= 169.3kbits/s

    QRegExp regLength("Duration: (\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    if( length && output.contains(regLength) )
    {
        *length = regLength.cap(1).toInt()*3600 + regLength.cap(2).toInt()*60 + regLength.cap(3).toInt();
    }
    QRegExp reg("time=(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    if( output.contains(reg) )
    {
        return reg.cap(1).toInt()*3600 + reg.cap(2).toInt()*60 + reg.cap(3).toInt();;
    }

    // TODO error handling
    // Error while decoding stream #0.0

    return -1;
}

float soundkonverter_codec_ffmpeg::parseOutput( const QString& output )
{
    return parseOutput( output, 0 );
}

void soundkonverter_codec_ffmpeg::processOutput()
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            const QString output = backendItems.at(i)->process->readAllStandardOutput().data();

            CodecPluginItem *pluginItem = qobject_cast<CodecPluginItem*>(backendItems.at(i));

            float progress = parseOutput( output, &pluginItem->data.length );
            if( progress == -1 && !output.simplified().isEmpty() )
                emit log( backendItems.at(i)->id, output );

            progress = progress * 100 / pluginItem->data.length;
            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

void soundkonverter_codec_ffmpeg::infoProcessOutput()
{
    const QString output = infoProcess.data()->readAllStandardOutput().data();

    // DEA    pcm_s16le       PCM signed 16-bit little-endian

    if( output.contains( QRegExp(" (D| )E.{4} pcm_s16le")) )
        codecMap["wav"] = "pcm_s16le";

    if( output.contains( QRegExp(" (D| )E.{4} libvorbis")) )
        codecMap["ogg vorbis"] = "libvorbis";

    if( output.contains( QRegExp(" (D| )E.{4} libmp3lame")) )
        codecMap["mp3"] = "libmp3lame";

    if( output.contains( QRegExp(" (D| )E.{4} flac")) )
        codecMap["flac"] = "flac";

    if( output.contains( QRegExp(" (D| )E.{4} wmav2")) )
        codecMap["wma"] = "wmav2";

    if( output.contains( QRegExp(" (D| )E.{4} libfaac")) )
        codecMap["aac"] = "libfaac";

    if( output.contains( QRegExp(" (D| )E.{4} ac3")) )
        codecMap["ac3"] = "ac3";

    if( output.contains( QRegExp(" (D| )E.{4} alac")) )
        codecMap["alac"] = "alac";

    if( output.contains( QRegExp(" (D| )E.{4} mp2")) )
        codecMap["mp2"] = "mp2";

    if( output.contains( QRegExp(" (D| )E.{4} libopencore_amrnb")) )
        codecMap["amr nb"] = "libopencore_amrnb";
}

void soundkonverter_codec_ffmpeg::infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)
    Q_UNUSED(exitCode)

    infoProcess.data()->deleteLater();
}


#include "soundkonverter_codec_ffmpeg.moc"

