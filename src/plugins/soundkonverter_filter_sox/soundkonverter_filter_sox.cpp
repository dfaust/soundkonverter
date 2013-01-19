
#include "soxfilterglobal.h"

#include "soundkonverter_filter_sox.h"
#include "../../core/conversionoptions.h"
#include "soxfilteroptions.h"
#include "soxfilterwidget.h"
#include "soxcodecwidget.h"

#include <KDialog>
#include <QHBoxLayout>
#include <KComboBox>
#include <KMessageBox>
#include <QLabel>
#include <QFileInfo>


soundkonverter_filter_sox::soundkonverter_filter_sox( QObject *parent, const QStringList& args  )
    : FilterPlugin( parent )
{
    Q_UNUSED(args)

    binaries["sox"] = "";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    configVersion = group.readEntry( "configVersion", 0 );
    samplingRateQuality = group.readEntry( "samplingRateQuality", "high" );
    experimentalEffectsEnabled = group.readEntry( "experimentalEffectsEnabled", false );
    soxLastModified = group.readEntry( "soxLastModified", QDateTime() );
    soxCodecList = group.readEntry( "codecList", QStringList() ).toSet();

    SoxCodecData data;

    // 8svx aif aifc aiff aiffc al amb amr-nb amr-wb anb au avr awb caf cdda cdr cvs cvsd cvu dat dvms f32 f4 f64 f8 fap flac fssd gsm gsrt hcom htk ima ircam la lpc lpc10 lu mat mat4 mat5 maud mp2 mp3 nist ogg paf prc pvf raw s1 s16 s2 s24 s3 s32 s4 s8 sb sd2 sds sf sl sln smp snd sndfile sndr sndt sou sox sph sw txw u1 u16 u2 u24 u3 u32 u4 u8 ub ul uw vms voc vorbis vox w64 wav wavpcm wv wve xa xi

    // todo
    // al amb anb au avr awb caf cdda cdr cvs cvsd cvu dat dvms f32 f4 f64 f8 fap fssd gsm gsrt hcom htk ima ircam la lpc lpc10 lu mat mat4 mat5 maud nist ogg paf prc pvf raw s1 s16 s2 s24 s3 s32 s4 s8 sb sd2 sds sf sl sln smp snd sndfile sndr sndt sou sox sph sw txw u1 u16 u2 u24 u3 u32 u4 u8 ub ul uw vms voc vox w64 wavpcm wv wve xa xi

    // WARNING enabled codecs need to be rescanned everytime new codecs are added here -> increase plugin version

    data.codecName = "wav";
    data.soxCodecName = "wav";
    data.external = false;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "flac";
    data.soxCodecName = "flac";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "ogg vorbis";
    data.soxCodecName = "vorbis";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "mp2";
    data.soxCodecName = "mp2";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "mp3";
    data.soxCodecName = "mp3";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "amr nb";
    data.soxCodecName = "amr-nb";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "amr wb";
    data.soxCodecName = "amr-wb";
    data.external = true;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "8svx";
    data.soxCodecName = "8svx";
    data.external = false;
    data.experimental = false;
    codecList.append( data );

    data.codecName = "aiff";
    data.soxCodecName = "aiff"; // aiff has meta data, aif not
    data.external = false;
    data.experimental = false;
    codecList.append( data );

//     sox only supports uncompressed aiff-c
//     data.codecName = "aiff-c";
//     data.soxCodecName = "aifc";
//     data.external = false;
//     data.experimental = false;
//     codecList.append( data );

    for( int i=0; i<codecList.count(); i++ )
    {
        if( !codecList.at(i).external && ( !codecList.at(i).experimental || experimentalEffectsEnabled ) )
        {
            codecList[i].enabled = true;
        }
        else
        {
            codecList[i].enabled = false;
        }
    }
}

soundkonverter_filter_sox::~soundkonverter_filter_sox()
{}

QString soundkonverter_filter_sox::name()
{
    return global_plugin_name;
}

int soundkonverter_filter_sox::version()
{
    return global_plugin_version;
}

QList<ConversionPipeTrunk> soundkonverter_filter_sox::codecTable()
{
    QList<ConversionPipeTrunk> table;

    if( !binaries["sox"].isEmpty() )
    {
        QFileInfo soxInfo( binaries["sox"] );
        if( soxInfo.lastModified() > soxLastModified || configVersion < version() )
        {
            infoProcess = new KProcess();
            infoProcess.data()->setOutputChannelMode( KProcess::MergedChannels );
            connect( infoProcess.data(), SIGNAL(readyRead()), this, SLOT(infoProcessOutput()) );
            connect( infoProcess.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(infoProcessExit(int,QProcess::ExitStatus)) );

            QStringList command;
            command += binaries["sox"];
            command += "--help";
            infoProcess.data()->clearProgram();
            infoProcess.data()->setShellCommand( command.join(" ") );
            infoProcess.data()->start();

            infoProcess.data()->waitForFinished( 3000 );
        }
    }

    for( int i=0; i<codecList.count(); i++ )
    {
        if( ( !codecList.at(i).experimental || experimentalEffectsEnabled ) && soxCodecList.contains(codecList.at(i).soxCodecName) )
        {
            codecList[i].enabled = true;
        }
        else
        {
            codecList[i].enabled = false;
        }
        allCodecs += codecList.at(i).codecName;
    }

    foreach( const QString fromCodec, allCodecs )
    {
        foreach( const QString toCodec, allCodecs )
        {
            bool codecEnabled = false;
            QStringList soxProblemInfo;
            foreach( const SoxCodecData data, codecList )
            {
                if( data.codecName == toCodec )
                {
                    if( data.enabled ) // everything should work, lets exit
                    {
                        codecEnabled = true;
                        break;
                    }
                    if( data.experimental && !experimentalEffectsEnabled )
                    {
                        soxProblemInfo.append( i18n("Enable experimental codecs in the sox configuration dialog.") );
                    }
                    if( data.external )
                    {
                        soxProblemInfo.append( i18n("Compile sox with %1 support.",data.soxCodecName) );
                    }
                    break;
                }
            }

            ConversionPipeTrunk newTrunk;
            newTrunk.codecFrom = fromCodec;
            newTrunk.codecTo = toCodec;
            newTrunk.rating = 80;
            newTrunk.enabled = ( binaries["sox"] != "" ) && codecEnabled;
            if( binaries["sox"] == "" )
            {
                if( toCodec == "wav" )
                {
                    newTrunk.problemInfo = standardMessage( "decode_codec,backend", fromCodec, "sox" ) + "\n" + standardMessage( "install_patented_backend", "sox" );
                }
                else if( fromCodec == "wav" )
                {
                    newTrunk.problemInfo = standardMessage( "encode_codec,backend", toCodec, "sox" ) + "\n" + standardMessage( "install_patented_backend", "sox" );
                }
            }
            else
            {
                newTrunk.problemInfo = soxProblemInfo.join("\n"+i18nc("like in either or","or")+"\n");
            }
            newTrunk.data.hasInternalReplayGain = false;
            table.append( newTrunk );
        }
    }

    return table;
}

bool soundkonverter_filter_sox::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_filter_sox::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
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
        QLabel *configDialogSamplingRateQualityLabel = new QLabel( i18n("Sample rate change quality:"), configDialogWidget );
        configDialogBox->addWidget( configDialogSamplingRateQualityLabel );
        configDialogSamplingRateQualityComboBox = new KComboBox( configDialogWidget );
        configDialogSamplingRateQualityComboBox->addItem( i18n("Quick"), "quick" );
        configDialogSamplingRateQualityComboBox->addItem( i18n("Low"), "low" );
        configDialogSamplingRateQualityComboBox->addItem( i18n("Medium"), "medium" );
        configDialogSamplingRateQualityComboBox->addItem( i18n("High"), "high" );
        configDialogSamplingRateQualityComboBox->addItem( i18n("Very high"), "very high" );
        configDialogBox->addWidget( configDialogSamplingRateQualityComboBox );

        configDialog.data()->setMainWidget( configDialogWidget );
        connect( configDialog.data(), SIGNAL( okClicked() ), this, SLOT( configDialogSave() ) );
        connect( configDialog.data(), SIGNAL( defaultClicked() ), this, SLOT( configDialogDefault() ) );
    }
    configDialogSamplingRateQualityComboBox->setCurrentIndex( configDialogSamplingRateQualityComboBox->findData(samplingRateQuality) );
    configDialog.data()->show();
}

void soundkonverter_filter_sox::configDialogSave()
{
    if( configDialog.data() )
    {
        samplingRateQuality = configDialogSamplingRateQualityComboBox->itemData( configDialogSamplingRateQualityComboBox->currentIndex() ).toString();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "samplingRateQuality", samplingRateQuality );

        configDialog.data()->deleteLater();
    }
}

void soundkonverter_filter_sox::configDialogDefault()
{
    if( configDialog.data() )
    {
        configDialogSamplingRateQualityComboBox->setCurrentIndex( configDialogSamplingRateQualityComboBox->findData("high") );
    }
}

bool soundkonverter_filter_sox::hasInfo()
{
    return false;
}

void soundkonverter_filter_sox::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

FilterWidget *soundkonverter_filter_sox::newFilterWidget()
{
    SoxFilterWidget *widget = new SoxFilterWidget();
    if( lastUsedFilterOptions )
    {
        widget->setCurrentFilterOptions( lastUsedFilterOptions );
        delete lastUsedFilterOptions;
        lastUsedFilterOptions = 0;
    }
    return qobject_cast<FilterWidget*>(widget);
}

CodecWidget *soundkonverter_filter_sox::newCodecWidget()
{
    SoxCodecWidget *widget = new SoxCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_filter_sox::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

    FilterPluginItem *newItem = new FilterPluginItem( this );
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

QStringList soundkonverter_filter_sox::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED( tags );
    Q_UNUSED( replayGain );

    if( !_conversionOptions )
        return QStringList();

    ConversionOptions *conversionOptions = _conversionOptions;

    QStringList command;

    SoxFilterOptions *filterOptions = 0;
    foreach( FilterOptions *_filterOptions,conversionOptions->filterOptions )
    {
        if( _filterOptions->pluginName == global_plugin_name )
            filterOptions = dynamic_cast<SoxFilterOptions*>(_filterOptions);
    }

    command += binaries["sox"];
    command += "--no-glob";
    if( conversionOptions->pluginName == name() )
    {
        command += conversionOptions->cmdArguments;
    }
    if( inputFile.isEmpty() || inputCodec == "amr nb" ) // NOTE sox expects anb as extension for amr nb files
    {
        command += "--type";
        command += soxCodecName(inputCodec);
    }
    command += "\"" + escapeUrl(inputFile) + "\"";
    if( filterOptions && filterOptions->data.sampleSize )
    {
        command += "--bits";
        command += QString::number(filterOptions->data.sampleSize);
    }
    if( outputCodec == "flac" && ( conversionOptions->pluginName == global_plugin_name || conversionOptions->pluginName == "FLAC" ) )
    {
        command += "--compression";
        command += QString::number(conversionOptions->compressionLevel);
    }
    else if( outputCodec == "ogg vorbis" && ( conversionOptions->pluginName == global_plugin_name || conversionOptions->pluginName == "Vorbis Tools" ) )
    {
        command += "--compression";
        command += QString::number(conversionOptions->quality);
    }
    else if( outputCodec == "mp2" )
    {
        command += "--compression";
        command += QString::number(conversionOptions->bitrate);
    }
    else if( outputCodec == "mp3" && ( conversionOptions->pluginName == global_plugin_name || conversionOptions->pluginName == "lame" ) )
    {
        command += "--compression";

        QString compressionLevel = QString::number(conversionOptions->compressionLevel);
        if( compressionLevel == "0" )
            compressionLevel = "01";

        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "-" + QString::number(conversionOptions->quality) + "." + compressionLevel;
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            command += QString::number(conversionOptions->bitrate) + "." + compressionLevel;
        }
    }
    else if( outputCodec == "amr nb" && conversionOptions->pluginName == global_plugin_name )
    {
        command += "--compression";
        command += QString::number(conversionOptions->quality);
    }
    else if( outputCodec == "amr wb" && conversionOptions->pluginName == global_plugin_name )
    {
        command += "--compression";
        command += QString::number(conversionOptions->quality);
    }
    if( outputFile.isEmpty() || outputCodec == "amr nb" ) // NOTE sox expects anb as extension for amr nb files
    {
        command += "--type";
        command += soxCodecName(outputCodec);
    }
    command += "\"" + escapeUrl(outputFile) + "\"";
    if( filterOptions && filterOptions->data.sampleRate )
    {
        command += "rate";
        if( samplingRateQuality == "quick" )
            command += "-q";
        else if( samplingRateQuality == "low" )
            command += "-l";
        else if( samplingRateQuality == "medium" )
            command += "-m";
        else if( samplingRateQuality == "high" )
            command += "-h";
        else if( samplingRateQuality == "very high" )
            command += "-v";
        command += QString::number(filterOptions->data.sampleRate);
    }

    return command;
}

float soundkonverter_filter_sox::parseOutput( const QString& output )
{
    Q_UNUSED( output );

//     // 01-Unknown.wav: 98% complete, ratio=0,479    // encode
//     // 01-Unknown.wav: 27% complete                 // decode
//
//     QRegExp regEnc("(\\d+)% complete");
//     if( output.contains(regEnc) )
//     {
//         return (float)regEnc.cap(1).toInt();
//     }
//
    return -1;
}

FilterOptions *soundkonverter_filter_sox::filterOptionsFromXml( QDomElement filterOptions )
{
    SoxFilterOptions *options = new SoxFilterOptions();
    options->fromXml( filterOptions );
    return options;
}

QString soundkonverter_filter_sox::soxCodecName( const QString& codecName )
{
    foreach( SoxCodecData data, codecList )
    {
        if( data.codecName == codecName )
            return data.soxCodecName;
    }

    return codecName;
}

void soundkonverter_filter_sox::infoProcessOutput()
{
    infoProcessOutputData.append( infoProcess.data()->readAllStandardOutput().data() );
}

void soundkonverter_filter_sox::infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)
    Q_UNUSED(exitCode)

    QRegExp formatsReg("AUDIO FILE FORMATS: ([^\n]*)");
    if( infoProcessOutputData.contains(formatsReg) )
    {
        const QStringList formats = formatsReg.cap(1).split(" ",QString::SkipEmptyParts);

        soxCodecList.clear();

        for( int i=0; i<codecList.count(); i++ )
        {
            if( formats.contains(codecList.at(i).soxCodecName) )
            {
                soxCodecList += codecList.at(i).soxCodecName;
            }
        }

        QFileInfo soxInfo( binaries["sox"] );
        soxLastModified = soxInfo.lastModified();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "configVersion", version() );
        group.writeEntry( "soxLastModified", soxLastModified );
        group.writeEntry( "codecList", soxCodecList.toList() );
    }

    infoProcessOutputData.clear();
    infoProcess.data()->deleteLater();
}



#include "soundkonverter_filter_sox.moc"

