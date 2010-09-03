
#include "timiditycodecglobal.h"

#include "soundkonverter_codec_timidity.h"
#include "../../core/conversionoptions.h"
#include "timiditycodecwidget.h"


soundkonverter_codec_timidity::soundkonverter_codec_timidity( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
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
    newTrunk.problemInfo = i18n("In order to decode midi files, you need to install 'timidity'.\ntimidity should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "mod";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["timidity"] != "" );
    newTrunk.problemInfo = i18n("In order to decode mod files, you need to install 'timidity'.\ntimidity should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_timidity::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "midi" )
    {
        info.lossless = true;
        info.description = i18n("midi is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
        info.mimeTypes.append( "audio/midi" );
        info.extensions.append( "midi" );
        info.extensions.append( "mid" );
        info.extensions.append( "kar" );
    }
    if( codecName == "midi" )
    {
        info.lossless = true;
        info.description = i18n("mod is a very old sound file format, that doesn't encode audio waves but stores instrument timings.");
        info.mimeTypes.append( "audio/x-mod" );
        info.extensions.append( "669" );
        info.extensions.append( "m15" );
        info.extensions.append( "med" );
        info.extensions.append( "mod" );
        info.extensions.append( "mtm" );
        info.extensions.append( "ult" );
        info.extensions.append( "uni" );
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

bool soundkonverter_codec_timidity::isConfigSupported( ActionType action, const QString& codecName )
{
    return false;
}

void soundkonverter_codec_timidity::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_codec_timidity::hasInfo()
{
    return false;
}

void soundkonverter_codec_timidity::showInfo( QWidget *parent )
{}

QWidget *soundkonverter_codec_timidity::newCodecWidget()
{
    TimidityCodecWidget *widget = new TimidityCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_timidity::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_timidity::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "wav" )
    {
        command += binaries["timidity"];
        command += "-Ow";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
        command += "\"" + inputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_timidity::parseOutput( const QString& output )
{
    // no output

    return -1;
}


#include "soundkonverter_codec_timidity.moc"
