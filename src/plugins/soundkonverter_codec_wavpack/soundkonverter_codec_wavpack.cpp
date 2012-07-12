
#include "wavpackcodecglobal.h"

#include "soundkonverter_codec_wavpack.h"
#include "../../core/conversionoptions.h"
#include "wavpackcodecwidget.h"


soundkonverter_codec_wavpack::soundkonverter_codec_wavpack( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

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
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "wavpack", "wavpack" ) + "\n" + standardMessage( "install_website_backend,url", "wavpack", "http://www.wavpack.com" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wavpack";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["wvunpack"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "wavpack", "wvunpack" ) + "\n" + i18n( "'%1' is usually in the package '%2' which you can download at %3", QString("wvunpack"), QString("wavpack"), QString("http://www.wavpack.com") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_wavpack::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_wavpack::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_wavpack::hasInfo()
{
    return false;
}

void soundkonverter_codec_wavpack::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_wavpack::newCodecWidget()
{
    WavPackCodecWidget *widget = new WavPackCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_wavpack::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_wavpack::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

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
        if( inputFile.isEmpty() )
        {
            command += "--raw-pcm";
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["wvunpack"];
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-o";
        command += "\"" + escapeUrl(outputFile) + "\"";
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
