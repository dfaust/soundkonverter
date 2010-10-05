
#include "wavpackcodecglobal.h"

#include "soundkonverter_codec_wavpack.h"
#include "../../core/conversionoptions.h"
#include "wavpackcodecwidget.h"


soundkonverter_codec_wavpack::soundkonverter_codec_wavpack( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    binaries["wavpack"] = "";
    binaries["wvunpack"] = "";

    allCodecs += "wavpack";
    allCodecs += "wav";
}

soundkonverter_codec_wavpack::~soundkonverter_codec_wavpack()
{}

QString soundkonverter_codec_wavpack::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_wavpack::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "wavpack";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["wavpack"] != "" );
    newTrunk.problemInfo = i18n("In order to encode wavpack files, you need to install 'wavpack'.\nYou can get it at http://www.wavpack.com");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wavpack";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["wvunpack"] != "" );
    newTrunk.problemInfo = i18n("In order to decode wavpack files, you need to install 'wvunpack'.\nwvunpack should be in the package 'wavpack'.");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_wavpack::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "wavpack" )
    {
        info.lossless = true;
        info.description = i18n("WavPack is a free and lossless audio codec.\nFor more information see: http://www.wavpack.com");
        info.mimeTypes.append( "audio/x-wavpack" );
        info.extensions.append( "wv" );
        info.extensions.append( "wvp" );
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

bool soundkonverter_codec_wavpack::isConfigSupported( ActionType action, const QString& codecName )
{
    return false;
}

void soundkonverter_codec_wavpack::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_codec_wavpack::hasInfo()
{
    return false;
}

void soundkonverter_codec_wavpack::showInfo( QWidget *parent )
{}

QWidget *soundkonverter_codec_wavpack::newCodecWidget()
{
    WavPackCodecWidget *widget = new WavPackCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_wavpack::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_wavpack::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "wavpack" )
    {
        command += binaries["wavpack"];
        if( conversionOptions->pluginName == global_plugin_name )
        {
            switch( (int)conversionOptions->compressionLevel )
            {
                case 0:
                {
                    command += "-f";
                    break;
                }
                case 2:
                {
                    command += "-h";
                    break;
                }
                case 3:
                {
                    command += "-hh";
                    break;
                }
            }
            command += conversionOptions->cmdArguments;
        }
        if( inputFile.toLocalFile() == "-" )
        {
            command += "--raw-pcm";
        }
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }
    else
    {
        command += binaries["wvunpack"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "-o";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }

    return command;
}

float soundkonverter_codec_wavpack::parseOutput( const QString& output )
{
    // creating test.wv,  58% done...
    // restoring test.wv.wav,  31% done...
  
    QRegExp reg("\\s+(\\d+)% done");
    if( output.contains(reg) )
    {
        return (float)reg.cap(1).toInt();
    }
    
    return -1;
}


#include "soundkonverter_codec_wavpack.moc"
