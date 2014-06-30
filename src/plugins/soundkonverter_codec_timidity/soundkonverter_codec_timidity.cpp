
#include "timiditycodecglobal.h"

#include "soundkonverter_codec_timidity.h"
#include "../../core/conversionoptions.h"
#include "timiditycodecwidget.h"


soundkonverter_codec_timidity::soundkonverter_codec_timidity( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["timidity"] = "";

    allCodecs += "midi";
    allCodecs += "mod";
    allCodecs += "wav";
}

soundkonverter_codec_timidity::~soundkonverter_codec_timidity()
{}

QString soundkonverter_codec_timidity::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_timidity::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "midi";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["timidity"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "midi", "timidity" ) + "\n" + standardMessage( "install_opensource_backend", "timidity" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "mod";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["timidity"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "mod", "timidity" ) + "\n" + standardMessage( "install_opensource_backend", "timidity" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_timidity::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_timidity::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_timidity::hasInfo()
{
    return false;
}

void soundkonverter_codec_timidity::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_timidity::newCodecWidget()
{
    TimidityCodecWidget *widget = new TimidityCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_timidity::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
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

QStringList soundkonverter_codec_timidity::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    QStringList command;

    if( outputCodec == "wav" )
    {
        command += binaries["timidity"];
        command += "-Ow";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_timidity::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no output

    return -1;
}


#include "soundkonverter_codec_timidity.moc"
