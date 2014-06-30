
#include "vorbistoolscodecglobal.h"

#include "soundkonverter_codec_vorbistools.h"
#include "../../core/conversionoptions.h"
#include "vorbistoolscodecwidget.h"


soundkonverter_codec_vorbistools::soundkonverter_codec_vorbistools( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["oggenc"] = "";
    binaries["oggdec"] = "";

    allCodecs += "ogg vorbis";
    allCodecs += "wav";
}

soundkonverter_codec_vorbistools::~soundkonverter_codec_vorbistools()
{}

QString soundkonverter_codec_vorbistools::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_vorbistools::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "ogg vorbis";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["oggenc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "ogg vorbis", "oggenc" ) + "\n" + i18n( "'%1' is usually in the package '%2' which should be shipped with your distribution.", QString("oggenc"), QString("vorbis-tools") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "ogg vorbis";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["oggdec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "ogg vorbis", "oggdec" ) + "\n" + i18n( "'%1' is usually in the package '%2' which should be shipped with your distribution.", QString("oggdec"), QString("vorbis-tools") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_vorbistools::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_vorbistools::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_vorbistools::hasInfo()
{
    return false;
}

void soundkonverter_codec_vorbistools::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_vorbistools::newCodecWidget()
{
    VorbisToolsCodecWidget *widget = new VorbisToolsCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_vorbistools::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_vorbistools::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "ogg vorbis" )
    {
        command += binaries["oggenc"];
//         if( inputFile.isEmpty() )
//         {
//             command += "--raw";
//         }
        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "-q";
            command += QString::number(conversionOptions->quality);
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            if( conversionOptions->bitrateMode == ConversionOptions::Abr )
            {
                command += "-b";
                command += QString::number(conversionOptions->bitrate);
            }
            else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
            {
                command += "--managed";
                command += "-b";
                command += QString::number(conversionOptions->bitrate);
            }
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["oggdec"];
        if( outputFile.isEmpty() )
        {
            command += "-Q";
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_vorbistools::parseOutput( const QString& output )
{
    //         [ 99.5%]

    if( output == "" || !output.contains("%") || output.contains("error",Qt::CaseInsensitive) )
        return -1;

    QString data = output;
    data.remove( 0, data.indexOf("[")+1 );
    data = data.left( data.indexOf("%") );
    return data.toFloat();
}


#include "soundkonverter_codec_vorbistools.moc"
