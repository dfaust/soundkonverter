
#include "faaccodecglobal.h"

#include "soundkonverter_codec_faac.h"
#include "../../core/conversionoptions.h"
#include "faaccodecwidget.h"

#include <QFileInfo>
#include <KConfigGroup>


soundkonverter_codec_faac::soundkonverter_codec_faac( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["faac"] = "";
    binaries["faad"] = "";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    configVersion = group.readEntry( "configVersion", 0 );
    faacLastModified = group.readEntry( "faacLastModified", QDateTime() );
    faacHasMp4Support = group.readEntry( "faacHasMp4Support", true );

    allCodecs += "aac";
    allCodecs += "m4a/aac";
    allCodecs += "mp4";
    allCodecs += "wav";
}

soundkonverter_codec_faac::~soundkonverter_codec_faac()
{}

QString soundkonverter_codec_faac::name()
{
    return global_plugin_name;
}

int soundkonverter_codec_faac::version()
{
    return global_plugin_version;
}

QList<ConversionPipeTrunk> soundkonverter_codec_faac::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    if( !binaries["faac"].isEmpty() )
    {
        QFileInfo faacInfo( binaries["faac"] );
        if( faacInfo.lastModified() > faacLastModified || configVersion < version() )
        {
            infoProcess = new KProcess();
            infoProcess.data()->setOutputChannelMode( KProcess::MergedChannels );
            connect( infoProcess.data(), SIGNAL(readyRead()), this, SLOT(infoProcessOutput()) );
            connect( infoProcess.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(infoProcessExit(int,QProcess::ExitStatus)) );

            QStringList command;
            command += binaries["faac"];
            command += "--help";
            infoProcess.data()->clearProgram();
            infoProcess.data()->setShellCommand( command.join(" ") );
            infoProcess.data()->start();

            infoProcess.data()->waitForFinished( 3000 );
        }
    }

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "aac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faac"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "m4a/aac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faac"] != "" && faacHasMp4Support );
    if( binaries["faad"] != "" && !faacHasMp4Support )
    {
        newTrunk.problemInfo = i18n("Compile faac with MP4 support.");
    }
    else
    {
        newTrunk.problemInfo = standardMessage( "encode_codec,backend", "m4a/aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    }
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "aac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faad"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "m4a/aac";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["faad"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "m4a/aac", "faac" ) + "\n" + standardMessage( "install_patented_backend", "faac" );
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

CodecWidget *soundkonverter_codec_faac::newCodecWidget()
{
    FaacCodecWidget *widget = new FaacCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_faac::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_faac::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "aac" || outputCodec == "m4a/aac" )
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
        if( outputCodec == "m4a/aac" )
        {
            command += "-w"; // Wrap AAC data in MP4 container
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

void soundkonverter_codec_faac::infoProcessOutput()
{
    infoProcessOutputData.append( infoProcess.data()->readAllStandardOutput().data() );
}

void soundkonverter_codec_faac::infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)
    Q_UNUSED(exitCode)

    if( infoProcessOutputData.contains("MP4 support unavailable", Qt::CaseInsensitive) )
    {
        faacHasMp4Support = false;
    }
    else
    {
        faacHasMp4Support = true;
    }

    QFileInfo ffmpegInfo( binaries["faac"] );
    faacLastModified = ffmpegInfo.lastModified();

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    group.writeEntry( "configVersion", version() );
    group.writeEntry( "faacLastModified", faacLastModified );
    group.writeEntry( "faacHasMp4Support", faacHasMp4Support );

    infoProcessOutputData.clear();
    infoProcess.data()->deleteLater();
}


#include "soundkonverter_codec_faac.moc"
