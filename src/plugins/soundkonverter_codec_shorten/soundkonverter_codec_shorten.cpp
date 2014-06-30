
#include "shortencodecglobal.h"

#include "soundkonverter_codec_shorten.h"
#include "../../core/conversionoptions.h"
#include "shortencodecwidget.h"


soundkonverter_codec_shorten::soundkonverter_codec_shorten( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["shorten"] = "";

    allCodecs += "shorten";
    allCodecs += "wav";
}

soundkonverter_codec_shorten::~soundkonverter_codec_shorten()
{}

QString soundkonverter_codec_shorten::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_shorten::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "shorten";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["shorten"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "shorten", "shorten" ) + "\n" + standardMessage( "install_website_backend,url", "shorten", "http://etree.org/shnutils/shorten/" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "shorten";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["shorten"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "shorten", "shorten" ) + "\n" + standardMessage( "install_website_backend,url", "shorten", "http://etree.org/shnutils/shorten/" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_shorten::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_shorten::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_shorten::hasInfo()
{
    return false;
}

void soundkonverter_codec_shorten::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_shorten::newCodecWidget()
{
    ShortenCodecWidget *widget = new ShortenCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_shorten::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_shorten::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "shorten" )
    {
        command += binaries["shorten"];
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["shorten"];
        command += "-x";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_shorten::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no output provided

    return -1;
}


#include "soundkonverter_codec_shorten.moc"
