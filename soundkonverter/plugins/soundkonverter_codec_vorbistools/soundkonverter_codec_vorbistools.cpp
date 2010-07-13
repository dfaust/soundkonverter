
#include "vorbistoolscodecglobal.h"

#include "soundkonverter_codec_vorbistools.h"
#include "../../core/conversionoptions.h"
#include "vorbistoolscodecwidget.h"


soundkonverter_codec_vorbistools::soundkonverter_codec_vorbistools( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    binaries["oggenc"] = "";
    binaries["oggdec"] = "";
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
    newTrunk.problemInfo = i18n("In order to encode ogg vorbis files, you need to install 'oggenc'.\noggenc is usually in the package 'vorbis-tools' which should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "ogg vorbis";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["oggdec"] != "" );
    newTrunk.problemInfo = i18n("In order to decode ogg vorbis files, you need to install 'oggdec'.\noggdec is usually in the package 'vorbis-tools' which should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_vorbistools::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "ogg vorbis" )
    {
        info.lossless = false;
//         info.description = i18n("Ogg Vorbis is a free and lossy high quality audio codec.\nFor more information see: <a href=\"http://www.xiph.org/vorbis/\">http://www.xiph.org/vorbis/</a>");
        info.description = i18n("Ogg Vorbis is a free and lossy high quality audio codec.\nFor more information see: http://www.xiph.org/vorbis/");
        info.mimeTypes.append( "application/ogg" );
        info.mimeTypes.append( "audio/vorbis" );
        info.extensions.append( "ogg" );
    }
    else if( codecName == "wav" )
    {
        info.lossless = true;
        info.description = i18n("Wave won't compress the audio stream.");
        info.mimeTypes.append( "audio/x-wav" );
        info.extensions.append( "wav" );
    }

    return info;
}

QString soundkonverter_codec_vorbistools::getCodecFromFile( const KUrl& filename, const QString& mimeType )
{
    if( mimeType == "application/x-ogg" || mimeType == "application/ogg" || mimeType == "audio/ogg" || mimeType == "audio/vorbis" || mimeType == "audio/x-vorbis+ogg" )
    {
        return "ogg vorbis";
    }
    else if( mimeType == "audio/x-wav" || mimeType == "audio/wav" )
    {
        return "wav";
    }
    else if( mimeType == "application/octet-stream" )
    {
        if( filename.url().endsWith(".ogg") ) return "ogg vorbis";
        if( filename.url().endsWith(".wav") ) return "wav";
    }

    return "";
}

bool soundkonverter_codec_vorbistools::isConfigSupported( ActionType action )
{
    return false;
}

void soundkonverter_codec_vorbistools::showConfigDialog( ActionType action, const QString& format, QWidget *parent )
{}

bool soundkonverter_codec_vorbistools::hasInfo()
{
    return false;
}

void soundkonverter_codec_vorbistools::showInfo()
{}

QWidget *soundkonverter_codec_vorbistools::newCodecWidget()
{
    VorbisToolsCodecWidget *widget = new VorbisToolsCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_vorbistools::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_vorbistools::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "ogg vorbis" )
    {
        command += binaries["oggenc"];
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
                if( conversionOptions->bitrateMin > 0 || conversionOptions->bitrateMax > 0 )
                {
                    command += "--managed";
                    if( conversionOptions->bitrateMin > 0 )
                    {
                        command += "-m";
                        command += QString::number(conversionOptions->bitrateMin);
                    }
                    if( conversionOptions->bitrateMax > 0 )
                    {
                        command += "-M";
                        command += QString::number(conversionOptions->bitrateMax);
                    }
                }
            }
            else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
            {
                command += "--managed";
                command += "-b";
                command += QString::number(conversionOptions->bitrate);
            }
        }
        if( conversionOptions->samplingRate > 0 )
        {
            command += "--resample";
            command += QString::number(conversionOptions->samplingRate);
        }
        if( conversionOptions->channels > 0 )
        {
            command += "--downmix";
        }
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }
    else
    {
        command += binaries["oggdec"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_vorbistools::parseOutput( const QString& output )
{
    //         [ 99.5%]
  
    if( output == "" || !output.contains("%") || output.contains("error",Qt::CaseInsensitive) ) return -1;

    QString data = output;
    data.remove( 0, data.indexOf("[")+1 );
    data = data.left( data.indexOf("%") );
    return data.toFloat();
}


#include "soundkonverter_codec_vorbistools.moc"
