
#include "ffmpegcodecglobal.h"

#include "soundkonverter_codec_ffmpeg.h"
#include "ffmpegcodecwidget.h"
#include "../../core/conversionoptions.h"
#include "../../metadata/tagengine.h"

#include <KMessageBox>
#include <KDialog>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QFileInfo>


// TODO check for decoders at runtime, too

soundkonverter_codec_ffmpeg::soundkonverter_codec_ffmpeg( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["ffmpeg"] = "";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    configVersion = group.readEntry( "configVersion", 0 );
    experimentalCodecsEnabled = group.readEntry( "experimentalCodecsEnabled", false );
    ffmpegVersionMajor = group.readEntry( "ffmpegVersionMajor", 0 );
    ffmpegVersionMinor = group.readEntry( "ffmpegVersionMinor", 0 );
    ffmpegLastModified = group.readEntry( "ffmpegLastModified", QDateTime() );
    ffmpegCodecList = group.readEntry( "codecList", QStringList() ).toSet();

    CodecData data;
    FFmpegCodecData ffmpegData;

    // WARNING enabled codecs need to be rescanned everytime new codecs are added here -> increase plugin version

    data.ffmpegCodecList.clear();
    data.codecName = "wav";
    ffmpegData.name = "wav";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "ogg vorbis";
    ffmpegData.name = "libvorbis";
    ffmpegData.external = true;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    ffmpegData.name = "vorbis";
    ffmpegData.external = true; // ?
    ffmpegData.experimental = true;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

//     data.ffmpegCodecList.clear();
//     data.codecName = "opus";
//     ffmpegData.name = "opus";
//     ffmpegData.external = true;
//     ffmpegData.experimental = false;
//     data.ffmpegCodecList.append( ffmpegData );
//     codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "mp3";
    ffmpegData.name = "libmp3lame";
    ffmpegData.external = true;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "flac";
    ffmpegData.name = "flac";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "wma";
    ffmpegData.name = "wmav2";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    ffmpegData.name = "wmav1";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "aac";
    ffmpegData.name = "libfaac";
    ffmpegData.external = true;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    ffmpegData.name = "aac";
    ffmpegData.external = false;
    ffmpegData.experimental = true;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "m4a/aac";
    ffmpegData.name = "libfaac";
    ffmpegData.external = true;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    ffmpegData.name = "aac";
    ffmpegData.external = false;
    ffmpegData.experimental = true;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "ac3";
    ffmpegData.name = "ac3";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "m4a/alac";
    ffmpegData.name = "alac";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "mp2";
    ffmpegData.name = "mp2";
    ffmpegData.external = false;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

    data.ffmpegCodecList.clear();
    data.codecName = "amr nb";
    ffmpegData.name = "libopencore_amrnb";
    ffmpegData.external = true;
    ffmpegData.experimental = false;
    data.ffmpegCodecList.append( ffmpegData );
    codecList.append( data );

//     codecMap["sonic"] = "sonic";
//     codecMap["sonic lossless"] = "sonicls";
//     codecMap["real audio 1"] = "real_144";
//     codecMap["e-ac3"] = "eac3";

    for( int i=0; i<codecList.count(); i++ )
    {
        for( int j=0; j<codecList.at(i).ffmpegCodecList.count(); j++ )
        {
            if( !codecList.at(i).ffmpegCodecList.at(j).external && ( !codecList.at(i).ffmpegCodecList.at(j).experimental || experimentalCodecsEnabled ) )
            {
                codecList[i].currentFFmpegCodec = codecList.at(i).ffmpegCodecList.at(j);
                break;
            }
        }
    }
}

soundkonverter_codec_ffmpeg::~soundkonverter_codec_ffmpeg()
{}

QString soundkonverter_codec_ffmpeg::name()
{
    return global_plugin_name;
}

int soundkonverter_codec_ffmpeg::version()
{
    return global_plugin_version;
}

QList<ConversionPipeTrunk> soundkonverter_codec_ffmpeg::codecTable()
{
    QList<ConversionPipeTrunk> table;
    QStringList fromCodecs;
    QStringList toCodecs;

    /// decode
    fromCodecs += "wav";
    fromCodecs += "ogg vorbis";
//     fromCodecs += "opus";
    fromCodecs += "mp3";
    fromCodecs += "flac";
    fromCodecs += "wma";
    fromCodecs += "aac";
    fromCodecs += "ac3";
    fromCodecs += "m4a/alac";
    fromCodecs += "mp2";
//     fromCodecs += "sonic";
//     fromCodecs += "sonic lossless";
    fromCodecs += "als";
    fromCodecs += "amr nb";
    fromCodecs += "amr wb";
    fromCodecs += "ape";
//     fromCodecs += "e-ac3";
    fromCodecs += "speex";
    fromCodecs += "m4a/aac";
    fromCodecs += "mp1";
    fromCodecs += "musepack";
    fromCodecs += "shorten";
//     fromCodecs += "mlp";
//     fromCodecs += "truehd";
//     fromCodecs += "truespeech";
    fromCodecs += "tta";
    fromCodecs += "wavpack";
    fromCodecs += "ra";
    fromCodecs += "sad";
    /// containers
    fromCodecs += "3gp";
    fromCodecs += "rm";
    /// video
    fromCodecs += "avi";
    fromCodecs += "mkv";
    fromCodecs += "ogv";
    fromCodecs += "mpeg";
    fromCodecs += "mov";
    fromCodecs += "mp4";
    fromCodecs += "flv";
    fromCodecs += "wmv";
    fromCodecs += "rv";

    /// encode
    if( !binaries["ffmpeg"].isEmpty() )
    {
        QFileInfo ffmpegInfo( binaries["ffmpeg"] );
        if( ffmpegInfo.lastModified() > ffmpegLastModified || configVersion < version() )
        {
            infoProcess = new KProcess();
            infoProcess.data()->setOutputChannelMode( KProcess::MergedChannels );
            connect( infoProcess.data(), SIGNAL(readyRead()), this, SLOT(infoProcessOutput()) );
            connect( infoProcess.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(infoProcessExit(int,QProcess::ExitStatus)) );

            QStringList command;
            command += binaries["ffmpeg"];
            command += "-codecs";
            infoProcess.data()->clearProgram();
            infoProcess.data()->setShellCommand( command.join(" ") );
            infoProcess.data()->start();

            infoProcess.data()->waitForFinished( 3000 );
        }
    }

    for( int i=0; i<codecList.count(); i++ )
    {
        for( int j=0; j<codecList.at(i).ffmpegCodecList.count(); j++ )
        {
            if( ( !codecList.at(i).ffmpegCodecList.at(j).experimental || experimentalCodecsEnabled ) && ffmpegCodecList.contains(codecList.at(i).ffmpegCodecList.at(j).name) )
            {
                codecList[i].currentFFmpegCodec = codecList.at(i).ffmpegCodecList.at(j);
                break;
            }
        }
        toCodecs += codecList.at(i).codecName;
    }

    for( int i=0; i<fromCodecs.count(); i++ )
    {
        for( int j=0; j<toCodecs.count(); j++ )
        {
            if( fromCodecs.at(i) == "wav" && toCodecs.at(j) == "wav" )
                continue;

            bool codecEnabled = ( toCodecs.at(j) == "wav" ); // always enabled if decoding
            QStringList ffmpegProblemInfo;
            if( !codecEnabled )
            {
                for( int k=0; k<codecList.count(); k++ )
                {
                    if( codecList.at(k).codecName == toCodecs.at(j) )
                    {
                        if( !codecList.at(k).currentFFmpegCodec.name.isEmpty() ) // everything should work, lets exit
                        {
                            codecEnabled = true;
                            break;
                        }
                        for( int l=0; l<codecList.at(k).ffmpegCodecList.count(); l++ )
                        {
                            if( codecList.at(k).ffmpegCodecList.at(l).experimental && !experimentalCodecsEnabled )
                            {
                                ffmpegProblemInfo.append( i18n("Enable experimental codecs in the ffmpeg configuration dialog.") );
                            }
                            if( codecList.at(k).ffmpegCodecList.at(l).external )
                            {
                                ffmpegProblemInfo.append( i18n("Compile ffmpeg with %1 support.",codecList.at(k).ffmpegCodecList.at(l).name) );
                            }
                        }
                        break;
                    }
                }
            }
            if( fromCodecs.at(i) == "opus" && ffmpegVersionMajor < 1 )
                codecEnabled = false;

            ConversionPipeTrunk newTrunk;
            newTrunk.codecFrom = fromCodecs.at(i);
            newTrunk.codecTo = toCodecs.at(j);
            newTrunk.rating = 90;
            newTrunk.enabled = ( binaries["ffmpeg"] != "" ) && codecEnabled;
            if( binaries["ffmpeg"] == "" )
            {
                if( toCodecs.at(j) == "wav" )
                {
                    newTrunk.problemInfo = standardMessage( "decode_codec,backend", fromCodecs.at(i), "ffmpeg" ) + "\n" + standardMessage( "install_patented_backend", "ffmpeg" );
                }
                else if( fromCodecs.at(i) == "wav" )
                {
                    newTrunk.problemInfo = standardMessage( "encode_codec,backend", toCodecs.at(j), "ffmpeg" ) + "\n" + standardMessage( "install_patented_backend", "ffmpeg" );
                }
            }
            else
            {
                newTrunk.problemInfo = ffmpegProblemInfo.join("\n"+i18nc("like in either or","or")+"\n");
            }
            newTrunk.data.hasInternalReplayGain = false;
            table.append( newTrunk );
        }
    }

    QSet<QString> codecs;
    codecs += QSet<QString>::fromList(fromCodecs);
    codecs += QSet<QString>::fromList(toCodecs);
    allCodecs = codecs.toList();

    return table;
}


bool soundkonverter_codec_ffmpeg::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_codec_ffmpeg::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    if( !configDialog.data() )
    {
        configDialog = new KDialog( parent );
        configDialog.data()->setCaption( i18n("Configure %1").arg(global_plugin_name)  );
        configDialog.data()->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Default );

        QWidget *configDialogWidget = new QWidget( configDialog.data() );
        QHBoxLayout *configDialogBox = new QHBoxLayout( configDialogWidget );
        configDialogExperimantalCodecsEnabledCheckBox = new QCheckBox( i18n("Enable experimental codecs"), configDialogWidget );
        configDialogBox->addWidget( configDialogExperimantalCodecsEnabledCheckBox );

        configDialog.data()->setMainWidget( configDialogWidget );
        connect( configDialog.data(), SIGNAL( okClicked() ), this, SLOT( configDialogSave() ) );
        connect( configDialog.data(), SIGNAL( defaultClicked() ), this, SLOT( configDialogDefault() ) );
    }
    configDialogExperimantalCodecsEnabledCheckBox->setChecked( experimentalCodecsEnabled );
    configDialog.data()->show();
}

void soundkonverter_codec_ffmpeg::configDialogSave()
{
    if( configDialog.data() )
    {
        const bool old_experimentalCodecsEnabled = experimentalCodecsEnabled;
        experimentalCodecsEnabled = configDialogExperimantalCodecsEnabledCheckBox->isChecked();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "experimentalCodecsEnabled", experimentalCodecsEnabled );

        if( experimentalCodecsEnabled != old_experimentalCodecsEnabled )
        {
            KMessageBox::information( configDialog.data(), i18n("Please restart soundKonverter in order to activate the changes.") );
        }
        configDialog.data()->deleteLater();
    }
}

void soundkonverter_codec_ffmpeg::configDialogDefault()
{
    if( configDialog.data() )
    {
        configDialogExperimantalCodecsEnabledCheckBox->setChecked( false );
    }
}

bool soundkonverter_codec_ffmpeg::hasInfo()
{
    return false;
}

void soundkonverter_codec_ffmpeg::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_ffmpeg::newCodecWidget()
{
    FFmpegCodecWidget *widget = new FFmpegCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_ffmpeg::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec != "wav" )
    {
        command += binaries["ffmpeg"];
        command += "-i";
        command += "\"" + escapeUrl(inputFile) + "\"";
        for( int i=0; i<codecList.count(); i++ )
        {
            if( codecList.at(i).codecName == outputCodec )
            {
                command += "-acodec";
                command += codecList.at(i).currentFFmpegCodec.name;
                if( codecList.at(i).currentFFmpegCodec.experimental )
                {
                    command += "-strict";
                    command += "experimental";
                }
                break;
            }
        }
        if( outputCodec != "m4a/alac" && outputCodec != "flac" )
        {
            command += "-ab";
            command += QString::number(conversionOptions->bitrate) + "k";
        }
        if( conversionOptions->pluginName == name() )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["ffmpeg"];
        command += "-i";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    if( tags )
        newItem->data.length = tags->length;

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    logCommand( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_codec_ffmpeg::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputFile)
    Q_UNUSED(outputFile)
    Q_UNUSED(inputCodec)
    Q_UNUSED(outputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    return QStringList();
}

float soundkonverter_codec_ffmpeg::parseOutput( const QString& output, int *length )
{
    // Duration: 00:02:16.50, start: 0.000000, bitrate: 1411 kb/s
    // size=    2445kB time=00:01:58.31 bitrate= 169.3kbits/s

    QRegExp regLength("Duration: (\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    if( length && output.contains(regLength) )
    {
        *length = regLength.cap(1).toInt()*3600 + regLength.cap(2).toInt()*60 + regLength.cap(3).toInt();
    }
    QRegExp reg("time=(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    if( output.contains(reg) )
    {
        return reg.cap(1).toInt()*3600 + reg.cap(2).toInt()*60 + reg.cap(3).toInt();
    }

    // TODO error handling
    // Error while decoding stream #0.0

    return -1;
}

float soundkonverter_codec_ffmpeg::parseOutput( const QString& output )
{
    return parseOutput( output, 0 );
}

void soundkonverter_codec_ffmpeg::processOutput()
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            const QString output = backendItems.at(i)->process->readAllStandardOutput().data();

            CodecPluginItem *pluginItem = qobject_cast<CodecPluginItem*>(backendItems.at(i));

            float progress = parseOutput( output, &pluginItem->data.length );
            if( progress == -1 && !output.simplified().isEmpty() )
                logOutput( backendItems.at(i)->id, output );

            progress = progress * 100 / pluginItem->data.length;
            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

void soundkonverter_codec_ffmpeg::infoProcessOutput()
{
    infoProcessOutputData.append( infoProcess.data()->readAllStandardOutput().data() );
}

void soundkonverter_codec_ffmpeg::infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)
    Q_UNUSED(exitCode)

    QRegExp regVersion("ffmpeg version (\\d+)\\.(\\d+) ");
    if( infoProcessOutputData.contains( regVersion ) )
    {
        ffmpegVersionMajor = regVersion.cap(1).toInt();
        ffmpegVersionMinor = regVersion.cap(2).toInt();
    }

    ffmpegCodecList.clear();

    for( int i=0; i<codecList.count(); i++ )
    {
        for( int j=0; j<codecList.at(i).ffmpegCodecList.count(); j++ )
        {
            if( infoProcessOutputData.contains( QRegExp(" (D| )E.{4} "+codecList.at(i).ffmpegCodecList.at(j).name+" ")) )
            {
                ffmpegCodecList += codecList.at(i).ffmpegCodecList.at(j).name;
            }
        }
    }

    QFileInfo ffmpegInfo( binaries["ffmpeg"] );
    ffmpegLastModified = ffmpegInfo.lastModified();

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    group.writeEntry( "configVersion", version() );
    group.writeEntry( "ffmpegVersionMajor", ffmpegVersionMajor );
    group.writeEntry( "ffmpegVersionMinor", ffmpegVersionMinor );
    group.writeEntry( "ffmpegLastModified", ffmpegLastModified );
    group.writeEntry( "codecList", ffmpegCodecList.toList() );

    infoProcessOutputData.clear();
    infoProcess.data()->deleteLater();
}

#include "soundkonverter_codec_ffmpeg.moc"

