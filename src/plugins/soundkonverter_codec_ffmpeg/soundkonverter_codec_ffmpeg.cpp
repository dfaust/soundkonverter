
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

    configDialogExperimantalCodecsEnabledCheckBox = 0;

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

    // WARNING enabled codecs need to be rescanned everytime new codecs are added here -> increase plugin version

    QHash<QString, QStringList> codecs;

    codecs.insert("wav",        QStringList() << "wav");
    codecs.insert("ogg vorbis", QStringList() << "libvorbis" << "vorbis");
    codecs.insert("opus",       QStringList() << "libopus");
    codecs.insert("mp3",        QStringList() << "libmp3lame");
    codecs.insert("flac",       QStringList() << "flac");
    codecs.insert("wma",        QStringList() << "wmav2" << "wmav1");
    codecs.insert("aac",        QStringList() << "aac"); // libfaac, libvo_aacenc
    codecs.insert("m4a/aac",    QStringList() << "aac"); // libfaac, libvo_aacenc
    codecs.insert("ac3",        QStringList() << "ac3");
    codecs.insert("m4a/alac",   QStringList() << "alac");
    codecs.insert("mp2",        QStringList() << "mp2" << "libtwolame"); // mp2fixed
//     codecs.insert("amr nb",     QStringList() << "libopencore_amrnb"); // Only 8000Hz sample rate supported
    codecs.insert("wavpack",    QStringList() << "wavpack");
    codecs.insert("speex",      QStringList() << "libspeex");
    codecs.insert("tta",        QStringList() << "tta");
    codecs.insert("ra",         QStringList() << "real_144");

    foreach( const QString& codecName, codecs.keys() )
    {
        CodecData data;
        data.codecName = codecName;

        foreach( const QString& encoderName, codecs.value(codecName) )
        {
            FFmpegEncoderData encoderData;
            encoderData.name = encoderName;
            data.ffmpegEnoderList.append( encoderData );
        }

        codecList.append( data );
    }

    for( int i=0; i<codecList.count(); i++ )
    {
        for( int j=0; j<codecList.at(i).ffmpegEnoderList.count(); j++ )
        {
            if( ( !codecList.at(i).ffmpegEnoderList.at(j).experimental || experimentalCodecsEnabled ) && ffmpegCodecList.contains(codecList.at(i).ffmpegEnoderList.at(j).name) )
            {
                codecList[i].currentFFmpegEncoder = codecList.at(i).ffmpegEnoderList.at(j);
                break;
            }
        }
    }
}

soundkonverter_codec_ffmpeg::~soundkonverter_codec_ffmpeg()
{}

QString soundkonverter_codec_ffmpeg::name() const
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
    fromCodecs += "opus";
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
    fromCodecs += "webm";
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
            command += "-encoders";
            infoProcess.data()->clearProgram();
            infoProcess.data()->setShellCommand( command.join(" ") );
            infoProcess.data()->start();

            infoProcess.data()->waitForFinished( 3000 );
        }
    }

    for( int i=0; i<codecList.count(); i++ )
    {
        for( int j=0; j<codecList.at(i).ffmpegEnoderList.count(); j++ )
        {
            if( ( !codecList.at(i).ffmpegEnoderList.at(j).experimental || experimentalCodecsEnabled ) && ffmpegCodecList.contains(codecList.at(i).ffmpegEnoderList.at(j).name) )
            {
                codecList[i].currentFFmpegEncoder = codecList.at(i).ffmpegEnoderList.at(j);
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
                bool experimantalInfo = false;
                for( int k=0; k<codecList.count(); k++ )
                {
                    if( codecList.at(k).codecName == toCodecs.at(j) )
                    {
                        if( !codecList.at(k).currentFFmpegEncoder.name.isEmpty() ) // everything should work, lets exit
                        {
                            codecEnabled = true;
                            break;
                        }
                        for( int l=0; l<codecList.at(k).ffmpegEnoderList.count(); l++ )
                        {
                            if( codecList.at(k).ffmpegEnoderList.at(l).experimental && !experimentalCodecsEnabled && !experimantalInfo )
                            {
                                ffmpegProblemInfo.append( i18n("Enable experimental codecs in the ffmpeg configuration dialog.") );
                                experimantalInfo = true;
                            }
                            else
                            {
                                ffmpegProblemInfo.append( i18n("Compile ffmpeg with %1 support.",codecList.at(k).ffmpegEnoderList.at(l).name) );
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
    return qobject_cast<CodecWidget*>(widget);
}

int soundkonverter_codec_ffmpeg::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    QStringList command;
    const ConversionOptions *conversionOptions = _conversionOptions;

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
                command += codecList.at(i).currentFFmpegEncoder.name;
                if( codecList.at(i).currentFFmpegEncoder.experimental )
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

QStringList soundkonverter_codec_ffmpeg::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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
    QRegExp reg1("time=(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    QRegExp reg2("time=(\\d+)\\.\\d");
    if( output.contains(reg1) )
    {
        return reg1.cap(1).toInt()*3600 + reg1.cap(2).toInt()*60 + reg1.cap(3).toInt();
    }
    else if( output.contains(reg2) )
    {
        return reg2.cap(1).toInt();
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
        for( int j=0; j<codecList.at(i).ffmpegEnoderList.count(); j++ )
        {
            QRegExp regEncoder("[AVS][F\\.][S\\.]([X\\.])[B\\.][D\\.] "+codecList.at(i).ffmpegEnoderList.at(j).name+"\\b");
            if( infoProcessOutputData.contains( regEncoder ))
            {
                const bool experimental = regEncoder.cap(1) == "X";
                if( experimental )
                {
                    codecList[i].ffmpegEnoderList[j].experimental = true;
                }
                ffmpegCodecList += codecList.at(i).ffmpegEnoderList.at(j).name;
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

