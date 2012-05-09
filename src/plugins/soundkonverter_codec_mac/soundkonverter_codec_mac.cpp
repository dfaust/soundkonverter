
#include "maccodecglobal.h"

#include "soundkonverter_codec_mac.h"
#include "../../core/conversionoptions.h"
#include "maccodecwidget.h"


soundkonverter_codec_mac::soundkonverter_codec_mac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["mac"] = "";

    allCodecs += "ape";
    allCodecs += "wav";
}

soundkonverter_codec_mac::~soundkonverter_codec_mac()
{}

QString soundkonverter_codec_mac::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_mac::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "ape";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mac"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "ape", "mac" ) + "\n" + standardMessage( "install_website_backend,url", "mac", "http://www.monkeysaudio.com" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "ape";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mac"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "ape", "mac" ) + "\n" + standardMessage( "install_website_backend,url", "mac", "http://www.monkeysaudio.com" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_mac::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_mac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_mac::hasInfo()
{
    return false;
}

void soundkonverter_codec_mac::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_mac::newCodecWidget()
{
    MacCodecWidget *widget = new MacCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_mac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return -1;

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "ape" )
    {
        command += binaries["mac"];
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += "-c"+QString::number((int)conversionOptions->compressionLevel);
        }
    }
    else
    {
        command += binaries["mac"];
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "-d";
    }

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

QStringList soundkonverter_codec_mac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputFile)
    Q_UNUSED(outputFile)
    Q_UNUSED(inputCodec)
    Q_UNUSED(outputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    return QStringList();
}

float soundkonverter_codec_mac::parseOutput( const QString& output )
{
    // Progress: 55.2% (1.0 seconds remaining, 1.2 seconds total)

    QRegExp regEnc("Progress:\\s+(\\d+.\\d)%");
    if( output.contains(regEnc) )
    {
        return regEnc.cap(1).toFloat();
    }

    return -1;
}


#include "soundkonverter_codec_mac.moc"
