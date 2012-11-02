
#include "neroaaccodecglobal.h"

#include "soundkonverter_codec_neroaac.h"
#include "neroaaccodecwidget.h"
#include "../../core/conversionoptions.h"
#include "../../metadata/tagengine.h"


// NeroAAC is a propritary implementation of an aac encoder that claims to reach a better quality then faac.\nYou can download it at http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php

soundkonverter_codec_neroaac::soundkonverter_codec_neroaac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["neroAacEnc"] = "";
    binaries["neroAacDec"] = "";

    allCodecs += "m4a/aac";
    allCodecs += "mp4";
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
    newTrunk.codecTo = "m4a/aac";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["neroAacEnc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "m4a/aac", "neroAacEnc" ) + "\n" + i18n( "You can download '%1' at '%2' after entering an email address.", QString("neroAacEnc"), QString("http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "m4a/aac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["neroAacDec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "m4a/aac", "neroAacDec" ) + "\n" + i18n( "You can download '%1' at '%2' after entering an email address.", QString("neroAacDec"), QString("http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "mp4";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["neroAacDec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "mp4", "neroAacDec" ) + "\n" + i18n( "You can download '%1' at '%2' after entering an email address.", QString("neroAacDec"), QString("http://www.nero.com/enu/downloads-nerodigital-nero-aac-codec.php") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_neroaac::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_neroaac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_neroaac::hasInfo()
{
    return false;
}

void soundkonverter_codec_neroaac::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_neroaac::newCodecWidget()
{
    NeroaacCodecWidget *widget = new NeroaacCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_neroaac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->data.length = tags ? tags->length : 200;
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

QStringList soundkonverter_codec_neroaac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "m4a/aac" )
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
                command += QString::number(conversionOptions->bitrate*1000);
            }
            else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
            {
                command += "-cbr";
                command += QString::number(conversionOptions->bitrate*1000);
            }
        }
        command += "-if";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-of";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["neroAacDec"];
        command += "-if";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "-of";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_neroaac::parseOutput( const QString& output, int length )
{
    if( length == 0 )
        return -1;

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
            if( progress == -1 && !output.simplified().isEmpty() )
                logOutput( backendItems.at(i)->id, output );
            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;
            return;
        }
    }
}


#include "soundkonverter_codec_neroaac.moc"
