
#include "mplayercodecglobal.h"

#include "soundkonverter_codec_mplayer.h"
#include "../../core/conversionoptions.h"
#include "mplayercodecwidget.h"


soundkonverter_codec_mplayer::soundkonverter_codec_mplayer( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["mplayer"] = "";
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
    fromCodecs += "als";
    fromCodecs += "amr nb";
    fromCodecs += "amr wb";
    fromCodecs += "ape";
    fromCodecs += "speex";
    fromCodecs += "m4a";
    fromCodecs += "mp1";
    fromCodecs += "musepack";
    fromCodecs += "shorten";
    fromCodecs += "tta";
    fromCodecs += "wavpack";
    fromCodecs += "ra";
    fromCodecs += "sad";
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
    toCodecs += "wav";

    for( int i=0; i<fromCodecs.count(); i++ )
    {
        for( int j=0; j<toCodecs.count(); j++ )
        {
            if( fromCodecs.at(i) == "wav" && toCodecs.at(j) == "wav" )
                continue;

            ConversionPipeTrunk newTrunk;
            newTrunk.codecFrom = fromCodecs.at(i);
            newTrunk.codecTo = toCodecs.at(j);
            newTrunk.rating = 80;
            newTrunk.enabled = ( binaries["mplayer"] != "" );
            newTrunk.problemInfo = standardMessage( "decode_codec,backend", fromCodecs.at(i), "mplayer" ) + "\n" + standardMessage( "install_patented_backend", "mplayer" );
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


bool soundkonverter_codec_mplayer::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_mplayer::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_mplayer::hasInfo()
{
    return false;
}

void soundkonverter_codec_mplayer::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

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
    const QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return -1;

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    logCommand( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_codec_mplayer::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( outputFile.isEmpty() )
        return QStringList();

    QStringList command;

    if( outputCodec == "wav" )
    {
        command += binaries["mplayer"];
        command += "-ao";
        command += "pcm:file=\"" + escapeUrl(outputFile) + "\"";
        command += "-vc";
        command += "null";
        command += "-vo";
        command += "null";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_mplayer::parseOutput( const QString& output )
{
    // decoding audio
    // A: 921.7 (15:21.7) of 2260.0 (37:40.0)  0.4%
    // decoding video
    // A:19743.6 V:19743.6 A-V:  0.016 ct: -0.506 491/491  0%  0%  0.6% 237 0

    QRegExp regAudio("A:\\s+(\\d+\\.\\d)\\s+\\(.*\\)\\s+of\\s+(\\d+\\.\\d)\\s+\\(.*\\)");
    if( output.contains(regAudio) )
    {
        return regAudio.cap(1).toFloat()/regAudio.cap(2).toFloat()*100.0f;
    }
    QRegExp regVideo("A:\\s*\\d+\\.\\d\\s+V:\\s*\\d+\\.\\d");
    if( output.contains(regVideo) )
    {
        return 0;
    }
    if( output.contains("Too many buffered pts") )
    {
        return 0;
    }

    // no encoding

    // TODO error handling

    return -1;
}

#include "soundkonverter_codec_mplayer.moc"

