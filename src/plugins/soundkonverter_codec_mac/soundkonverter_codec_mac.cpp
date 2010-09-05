
#include "maccodecglobal.h"

#include "soundkonverter_codec_mac.h"
#include "../../core/conversionoptions.h"
#include "maccodecwidget.h"


soundkonverter_codec_mac::soundkonverter_codec_mac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
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
    newTrunk.problemInfo = i18n("In order to encode ape files, you need to install 'mac'.\nYou can get it at http://www.monkeysaudio.com");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "ape";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mac"] != "" );
    newTrunk.problemInfo = i18n("In order to decode ape files, you need to install 'mac'.\nYou can get it at http://www.monkeysaudio.com");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_mac::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "ape" )
    {
        info.lossless = true;
        info.description = i18n("Monkey’s Audio is a fast and efficient lossless audio format.");
        info.mimeTypes.append( "audio/x-ape" );
        info.extensions.append( "ape" );
        info.extensions.append( "mac" );
    }
    else if( codecName == "wav" )
    {
        info.lossless = true;
        info.description = i18n("Wave won't compress the audio stream.");
        info.mimeTypes.append( "audio/x-wav" );
        info.mimeTypes.append( "audio/wav" );
        info.extensions.append( "wav" );
    }

    return info;
}

bool soundkonverter_codec_mac::isConfigSupported( ActionType action, const QString& codecName )
{
    return false;
}

void soundkonverter_codec_mac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_codec_mac::hasInfo()
{
    return false;
}

void soundkonverter_codec_mac::showInfo( QWidget *parent )
{}

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
    if( !_conversionOptions ) return -1;
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "ape" )
    {
        command += binaries["mac"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += "-c"+QString::number((int)conversionOptions->compressionLevel);
        }
    }
    else
    {
        command += binaries["mac"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
        command += "-d";
    }

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

QStringList soundkonverter_codec_mac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
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
