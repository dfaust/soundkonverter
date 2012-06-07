
#include "soxfilterglobal.h"

#include "soundkonverter_filter_sox.h"
#include "../../core/conversionoptions.h"
#include "soxfilteroptions.h"
#include "soxfilterwidget.h"
#include "soxcodecwidget.h"

#include <KDialog>
#include <QHBoxLayout>
#include <KComboBox>
#include <QLabel>


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

    allCodecs += "wav";
}

soundkonverter_filter_sox::~soundkonverter_filter_sox()
{}

QString soundkonverter_filter_sox::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_filter_sox::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    QStringList filter;
    filter += "normalize";

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["sox"] != "" );
    newTrunk.problemInfo = standardMessage( "filter,backend", filter.join(", "), "multiple", "sox" ) + "\n" + standardMessage( "install_opensource_backend", "sox" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "flac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["sox"] != "" );
    newTrunk.problemInfo = standardMessage( "filter,backend", filter.join(", "), "multiple", "sox" ) + "\n" + standardMessage( "install_opensource_backend", "sox" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

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

int soundkonverter_filter_sox::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return -1;

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

    QStringList command;

    foreach( FilterOptions *_filterOptions,_conversionOptions->filterOptions )
    {
        if( _filterOptions->pluginName == global_plugin_name )
        {
            SoxFilterOptions *filterOptions = dynamic_cast<SoxFilterOptions*>(_filterOptions);
            command += binaries["sox"];
            if( filterOptions->data.normalize )
            {
                command += "--norm=" + QString::number(filterOptions->data.normalizeVolume);
            }
            if( inputFile.isEmpty() )
            {
                command += "-t";
                command += inputCodec;
            }
            command += "\"" + escapeUrl(inputFile) + "\"";
            if( filterOptions->data.sampleSize )
            {
                command += "-b";
                command += QString::number(filterOptions->data.sampleSize);
            }
            if( outputFile.isEmpty() )
            {
                command += "-t";
                command += outputCodec;
            }
            command += "\"" + escapeUrl(outputFile) + "\"";
            if( filterOptions->data.sampleRate )
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
        }
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


#include "soundkonverter_filter_sox.moc"

