
#include "flakecodecglobal.h"

#include "soundkonverter_codec_flake.h"
#include "../../core/conversionoptions.h"
#include "flakecodecwidget.h"


soundkonverter_codec_flake::soundkonverter_codec_flake( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["flake"] = "";

    allCodecs += "flac";
    allCodecs += "wav";
}

soundkonverter_codec_flake::~soundkonverter_codec_flake()
{}

QString soundkonverter_codec_flake::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_flake::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "flac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["flake"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "flac", "flake" ) + "\n" + standardMessage( "install_website_backend,url", "flake", "http://flake-enc.sourceforge.net" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_flake::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_flake::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_flake::hasInfo()
{
    return false;
}

void soundkonverter_codec_flake::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
//         info.description = i18n("Flake is an alternative flac encoder.\nFor more information see: http://flake-enc.sourceforge.net");
}

CodecWidget *soundkonverter_codec_flake::newCodecWidget()
{
    FlakeCodecWidget *widget = new FlakeCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_flake::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_flake::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "flac" )
    {
        command += binaries["flake"];
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += "-"+QString::number((int)conversionOptions->compressionLevel);
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_flake::parseOutput( const QString& output )
{
    // progress:   6% | ratio: 0.556 | bitrate: 784.4 kbps

    QRegExp regEnc("progress:\\s+(\\d+)%");
    if( output.contains(regEnc) )
    {
        return (float)regEnc.cap(1).toInt();
    }

    return -1;
}


#include "soundkonverter_codec_flake.moc"
