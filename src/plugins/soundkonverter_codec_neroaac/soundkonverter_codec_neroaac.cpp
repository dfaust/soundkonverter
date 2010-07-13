
#include "neroaaccodecglobal.h"

#include "soundkonverter_codec_neroaac.h"
#include "../../core/conversionoptions.h"
#include "neroaaccodecwidget.h"
#include "../../core/tagdata.h"


// NeroAAC is a propritary implementation of an aac encoder that claims to reach a better quality then faac.\nYou can download it at http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php

soundkonverter_codec_neroaac::soundkonverter_codec_neroaac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    binaries["neroAacEnc"] = "";
    binaries["neroAacDec"] = "";
    
    allCodecs += "aac";
    allCodecs += "wav";
}

soundkonverter_codec_neroaac::~soundkonverter_codec_neroaac()
{}

QString soundkonverter_codec_neroaac::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_neroaac::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "aac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["neroAacEnc"] != "" );
    newTrunk.problemInfo = i18n("In order to encode aac files, you need to install 'neroAacEnc'.\nYou can download it at http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php after entering an email adress.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "aac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["neroAacDec"] != "" );
    newTrunk.problemInfo = i18n("In order to decode aac files, you need to install 'neroAacDec'.\nYou can download it at http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php after entering an email adress.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_neroaac::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "aac" )
    {
        info.lossless = false;
        info.description = i18n("Advanced Audio Coding is a lossy and popular audio format."); // http://en.wikipedia.org/wiki/Advanced_Audio_Coding
        info.mimeTypes.append( "audio/aac" );
        info.mimeTypes.append( "audio/aacp" );
        info.mimeTypes.append( "audio/mp4" );
        info.extensions.append( "aac" );
        info.extensions.append( "3gp" );
        info.extensions.append( "mp4" );
        info.extensions.append( "m4a" );
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

bool soundkonverter_codec_neroaac::isConfigSupported( ActionType action )
{
    return false;
}

void soundkonverter_codec_neroaac::showConfigDialog( ActionType action, const QString& format, QWidget *parent )
{}

bool soundkonverter_codec_neroaac::hasInfo()
{
    return false;
}

void soundkonverter_codec_neroaac::showInfo()
{}

QWidget *soundkonverter_codec_neroaac::newCodecWidget()
{
    NeroaacCodecWidget *widget = new NeroaacCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_neroaac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() ) return -1;

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->data.length = tags ? tags->length : 0;
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

QStringList soundkonverter_codec_neroaac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "aac" )
    {
        command += binaries["neroAacEnc"];
        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "-q";
            command += QString::number(conversionOptions->quality);
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            if( conversionOptions->bitrateMode == ConversionOptions::Abr )
            {
                command += "-br";
                command += QString::number(conversionOptions->bitrate);
            }
            else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
            {
                command += "-cbr";
                command += QString::number(conversionOptions->bitrate);
            }
        }
        command += "-if";
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-of";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }
    else
    {
        command += binaries["neroAacDec"];
        command += "-if";
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-of";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_neroaac::parseOutput( const QString& output, int length )
{
    if( length == 0 ) return -1;
    
    // Processed 218 seconds...
  
    QRegExp regEnc("Processed (\\d+) seconds");
    if( output.contains(regEnc) )
    {
        return (float)regEnc.cap(1).toInt()*100/length;
    }
    
    // no output when decoding
    
    return -1;
}

float soundkonverter_codec_neroaac::parseOutput( const QString& output )
{
    return parseOutput( output, 0 );
}

void soundkonverter_codec_neroaac::processOutput()
{
    CodecPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<CodecPluginItem*>(backendItems.at(i));
            progress = parseOutput( output, pluginItem->data.length );
            if( progress == -1 && !output.simplified().isEmpty() ) emit log( backendItems.at(i)->id, output );
            if( progress > backendItems.at(i)->progress ) backendItems.at(i)->progress = progress;
            return;
        }
    }
}


#include "soundkonverter_codec_neroaac.moc"
