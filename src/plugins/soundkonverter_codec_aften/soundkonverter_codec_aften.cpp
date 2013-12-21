
#include "aftencodecglobal.h"

#include "soundkonverter_codec_aften.h"
#include "../../core/conversionoptions.h"
#include "aftencodecwidget.h"


soundkonverter_codec_aften::soundkonverter_codec_aften( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["aften"] = "";

    allCodecs += "ac3";
    allCodecs += "wav";
}

soundkonverter_codec_aften::~soundkonverter_codec_aften()
{}

QString soundkonverter_codec_aften::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_aften::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "ac3";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["aften"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "ac3", "aften" ) + "\n" + standardMessage( "install_opensource_backend", "aften" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "ac3";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["aften"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "ac3", "aften" ) + "\n" + standardMessage( "install_opensource_backend", "aften" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_aften::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_aften::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_aften::hasInfo()
{
    return false;
}

void soundkonverter_codec_aften::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_aften::newCodecWidget()
{
    AftenCodecWidget *widget = new AftenCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_aften::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    const QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

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

QStringList soundkonverter_codec_aften::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "ac3" )
    {
        command += binaries["aften"];
        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "-q";
            command += QString::number(conversionOptions->quality);
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            command += "-b";
            command += QString::number(conversionOptions->bitrate);
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_aften::parseOutput( const QString& output )
{
    // progress:  59% | q: 269.7 | bw: 44.0 | bitrate: 192.0 kbps

    QRegExp reg("progress:\\s+(\\d+)%");
    if( output.contains(reg) )
    {
        return (float)reg.cap(1).toInt();
    }

    return -1;
}


#include "soundkonverter_codec_aften.moc"
