
#include "shortencodecglobal.h"

#include "soundkonverter_codec_shorten.h"
#include "../../core/conversionoptions.h"
#include "shortencodecwidget.h"


soundkonverter_codec_shorten::soundkonverter_codec_shorten( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
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
    newTrunk.problemInfo = i18n("In order to encode shorten files, you need to install 'shorten'.\nYou can get it at http://etree.org/shnutils/shorten/");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "shorten";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["shorten"] != "" );
    newTrunk.problemInfo = i18n("In order to decode shorten files, you need to install 'shorten'.\nYou can get it at http://etree.org/shnutils/shorten/");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_shorten::formatInfo( const QString& codecName )
{
    if( codecName == "shorten" )
    {
        BackendPlugin::FormatInfo info;
        info.codecName = codecName;
        info.lossless = true;
        info.description = i18n("Shorten is a free and lossless audio codec.\nFor more information see: http://etree.org/shnutils/shorten/");
        info.mimeTypes.append( "application/x-shorten" );
        info.extensions.append( "shn" );
        return info;
    }

    return BackendPlugin::formatInfo( codecName );
}

bool soundkonverter_codec_shorten::isConfigSupported( ActionType action, const QString& codecName )
{
    return false;
}

void soundkonverter_codec_shorten::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_codec_shorten::hasInfo()
{
    return false;
}

void soundkonverter_codec_shorten::showInfo( QWidget *parent )
{}

QWidget *soundkonverter_codec_shorten::newCodecWidget()
{
    ShortenCodecWidget *widget = new ShortenCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_shorten::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() ) return -1;

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

QStringList soundkonverter_codec_shorten::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "shorten" )
    {
        command += binaries["shorten"];
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }
    else
    {
        command += binaries["shorten"];
        command += "-x";
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_shorten::parseOutput( const QString& output )
{
    // no output provided
    
    return -1;
}


#include "soundkonverter_codec_shorten.moc"
