
#include "faaccodecglobal.h"

#include "soundkonverter_codec_faac.h"
#include "../../core/conversionoptions.h"
#include "faaccodecwidget.h"


soundkonverter_codec_faac::soundkonverter_codec_faac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["faac"] = "";
    binaries["faad"] = "";

    allCodecs += "aac";
    allCodecs += "m4a";
    allCodecs += "mp4";
    allCodecs += "wav";
}

soundkonverter_codec_faac::~soundkonverter_codec_faac()
{}

QString soundkonverter_codec_faac::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_faac::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "aac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faac"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "m4a";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faac"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "m4a", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "aac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faad"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "m4a";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faad"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "m4a", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "mp4";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faad"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "mp4", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_faac::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_faac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_faac::hasInfo()
{
    return false;
}

void soundkonverter_codec_faac::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_faac::newCodecWidget()
{
    FaacCodecWidget *widget = new FaacCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_faac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_faac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "aac" || outputCodec == "m4a" )
    {
        command += binaries["faac"];
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
        if( conversionOptions->samplingRate > 0 )
        {
            command += "-c";
            command += QString::number(conversionOptions->samplingRate);
        }
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }
    else
    {
        command += binaries["faad"];
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_faac::parseOutput( const QString& output )
{
    //  9397/9397  (100%)|  136.1  |    9.1/9.1    |   23.92x | 0.0

    QRegExp regEnc("(\\d+)/(\\d+)");
    if( output.contains(regEnc) )
    {
        return (float)regEnc.cap(1).toInt()*100/regEnc.cap(2).toInt();
    }

    // 15% decoding xxx

    QRegExp regDec("(\\d+)%");
    if( output.contains(regDec) )
    {
        return (float)regDec.cap(1).toInt();
    }

    return -1;
}


#include "soundkonverter_codec_faac.moc"
