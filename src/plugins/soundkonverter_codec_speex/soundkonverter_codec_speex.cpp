
#include "speexcodecglobal.h"

#include "soundkonverter_codec_speex.h"
#include "../../core/conversionoptions.h"
#include "speexcodecwidget.h"


soundkonverter_codec_speex::soundkonverter_codec_speex( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["speexenc"] = "";
    binaries["speexdec"] = "";

    allCodecs += "speex";
    allCodecs += "wav";
}

soundkonverter_codec_speex::~soundkonverter_codec_speex()
{}

QString soundkonverter_codec_speex::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_speex::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "speex";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["speexenc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "speex", "speex" ) + "\n" + standardMessage( "install_opensource_backend", "speex" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "speex";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["speexdec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "speex", "speex" ) + "\n" + standardMessage( "install_opensource_backend", "speex" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_speex::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_speex::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_speex::hasInfo()
{
    return false;
}

void soundkonverter_codec_speex::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_speex::newCodecWidget()
{
    SpeexCodecWidget *widget = new SpeexCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_speex::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    const QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
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

QStringList soundkonverter_codec_speex::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "speex" )
    {
        command += binaries["speexenc"];
        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "--vbr";
            command += "--quality";
            command += QString::number(conversionOptions->quality);
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            command += "--abr";
            command += QString::number(conversionOptions->bitrate*1000);
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["speexdec"];
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_speex::parseOutput( const QString& output )
{
    // no output

    return -1;
}


#include "soundkonverter_codec_speex.moc"
