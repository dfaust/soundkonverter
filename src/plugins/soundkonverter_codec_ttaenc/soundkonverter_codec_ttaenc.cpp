
#include "ttaenccodecglobal.h"

#include "soundkonverter_codec_ttaenc.h"
#include "../../core/conversionoptions.h"
#include "ttaenccodecwidget.h"


soundkonverter_codec_ttaenc::soundkonverter_codec_ttaenc( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["ttaenc"] = "";

    allCodecs += "tta";
    allCodecs += "wav";
}

soundkonverter_codec_ttaenc::~soundkonverter_codec_ttaenc()
{}

QString soundkonverter_codec_ttaenc::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_ttaenc::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "tta";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["ttaenc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "tta", "ttaenc" ) + "\n" + standardMessage( "install_opensource_backend", "ttaenc" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "tta";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["ttaenc"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "tta", "ttaenc" ) + "\n" + standardMessage( "install_opensource_backend", "ttaenc" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_ttaenc::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_ttaenc::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_ttaenc::hasInfo()
{
    return false;
}

void soundkonverter_codec_ttaenc::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_ttaenc::newCodecWidget()
{
    TTAEncCodecWidget *widget = new TTAEncCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_ttaenc::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_ttaenc::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    if( inputFile.isEmpty() )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "tta" )
    {
        command += binaries["ttaenc"];
        command += "-e";
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }
    else
    {
        command += binaries["ttaenc"];
        command += "-d";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_ttaenc::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no output provided

    return -1;
}


#include "soundkonverter_codec_ttaenc.moc"
