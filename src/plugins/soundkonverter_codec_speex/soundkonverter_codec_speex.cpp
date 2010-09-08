
#include "speexcodecglobal.h"

#include "soundkonverter_codec_speex.h"
#include "../../core/conversionoptions.h"
#include "speexcodecwidget.h"


soundkonverter_codec_speex::soundkonverter_codec_speex( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
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
    newTrunk.problemInfo = i18n("In order to encode speex files, you need to install the package 'speex'.\nspeex should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "speex";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["speexdec"] != "" );
    newTrunk.problemInfo = i18n("In order to decode speex files, you need to install the package 'speex'.\nspeex should be shipped with your distribution.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_speex::formatInfo( const QString& codecName )
{
    if( codecName == "speex" )
    {
        BackendPlugin::FormatInfo info;
        info.codecName = codecName;
        info.lossless = false;
        info.description = i18n("Speex is a free and lossy audio codec designed for low quality speech encoding.\nFor more information see: http://www.speex.org");
        info.mimeTypes.append( "audio/x-speex" );
        info.mimeTypes.append( "audio/x-speex+ogg" );
        info.extensions.append( "spx" );
//         info.extensions.append( "ogg" );
        return info;
    }

    return BackendPlugin::formatInfo( codecName );
}

// QString soundkonverter_codec_speex::getCodecFromFile( const KUrl& filename, const QString& mimeType )
// {
//     if( mimeType == "application/x-ogg" || mimeType == "application/ogg" || mimeType == "audio/ogg" || mimeType == "audio/vorbis" || mimeType == "audio/x-vorbis+ogg" )
//     {
//         return "speex";
//     }
//     else if( mimeType == "audio/x-wav" || mimeType == "audio/wav" )
//     {
//         return "wav";
//     }
//     else if( mimeType == "application/octet-stream" )
//     {
//         if( filename.url().endsWith(".ogg") ) return "speex";
//         if( filename.url().endsWith(".wav") ) return "wav";
//     }
// 
//     return "";
// }

bool soundkonverter_codec_speex::isConfigSupported( ActionType action, const QString& format )
{
    return false;
}

void soundkonverter_codec_speex::showConfigDialog( ActionType action, const QString& format, QWidget *parent )
{}

bool soundkonverter_codec_speex::hasInfo()
{
    return false;
}

void soundkonverter_codec_speex::showInfo( QWidget *parent )
{}

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

QStringList soundkonverter_codec_speex::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "speex" )
    {
        command += binaries["speexenc"];
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
        command += binaries["speexdec"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_speex::parseOutput( const QString& output )
{
    //         [ 99.5%]
  
    if( output == "" || !output.contains("%") || output.contains("error",Qt::CaseInsensitive) ) return -1;

    QString data = output;
    data.remove( 0, data.indexOf("[")+1 );
    data = data.left( data.indexOf("%") );
    return data.toFloat();
}


#include "soundkonverter_codec_speex.moc"
