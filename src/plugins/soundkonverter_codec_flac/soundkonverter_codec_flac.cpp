
#include "flaccodecglobal.h"

#include "soundkonverter_codec_flac.h"
#include "../../core/conversionoptions.h"
#include "flaccodecwidget.h"


soundkonverter_codec_flac::soundkonverter_codec_flac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["flac"] = "";

    allCodecs += "flac";
    allCodecs += "wav";
}

soundkonverter_codec_flac::~soundkonverter_codec_flac()
{}

QString soundkonverter_codec_flac::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_flac::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "flac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["flac"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "flac", "flac" ) + "\n" + standardMessage( "install_opensource_backend", "flac" );
    newTrunk.data.hasInternalReplayGain = true;
    table.append( newTrunk );

    newTrunk.codecFrom = "flac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["flac"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "flac", "flac" ) + "\n" + standardMessage( "install_opensource_backend", "flac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_flac::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_flac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_flac::hasInfo()
{
    return false;
}

void soundkonverter_codec_flac::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_flac::newCodecWidget()
{
    FlacCodecWidget *widget = new FlacCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_flac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
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

    emit log( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_codec_flac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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
        command += binaries["flac"];
        command += "-V";
        if( conversionOptions->pluginName == global_plugin_name )
        {
            command += "--compression-level-"+QString::number((int)conversionOptions->compressionLevel);
        }
        if( conversionOptions->replaygain )
        {
            command += "--replay-gain";
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["flac"];
        command += "-d";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_flac::parseOutput( const QString& output )
{
    // 01-Unknown.wav: 98% complete, ratio=0,479    // encode
    // 01-Unknown.wav: 27% complete                 // decode

    QRegExp regEnc("(\\d+)% complete");
    if( output.contains(regEnc) )
    {
        return (float)regEnc.cap(1).toInt();
    }

    return -1;
}


#include "soundkonverter_codec_flac.moc"
