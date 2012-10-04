
#include "opustoolscodecglobal.h"

#include "soundkonverter_codec_opustools.h"
#include "../../core/conversionoptions.h"
#include "opustoolscodecwidget.h"
#include "opustoolsconversionoptions.h"

#include <KDialog>
#include <QCheckBox>
#include <QBoxLayout>


soundkonverter_codec_opustools::soundkonverter_codec_opustools( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["opusenc"] = "";
    binaries["opusdec"] = "";

    allCodecs += "opus";
    allCodecs += "wav";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    configVersion = group.readEntry( "configVersion", 0 );
    uncoupledChannels = group.readEntry( "uncoupledChannels", false );
}

soundkonverter_codec_opustools::~soundkonverter_codec_opustools()
{}

QString soundkonverter_codec_opustools::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_opustools::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "opus";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["opusenc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "opus", "opusenc" ) + "\n" + i18n( "'%1' is usually in the package '%2' which should be shipped with your distribution.", QString("opusenc"), QString("opus-tools") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "opus";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["opusdec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "opus", "opusdec" ) + "\n" + i18n( "'%1' is usually in the package '%2' which should be shipped with your distribution.", QString("opusdec"), QString("opus-tools") );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_opustools::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_codec_opustools::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    if( !configDialog.data() )
    {
        configDialog = new KDialog( parent );
        configDialog.data()->setCaption( i18n("Configure %1").arg(global_plugin_name)  );
        configDialog.data()->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Default );

        QWidget *configDialogWidget = new QWidget( configDialog.data() );
        QVBoxLayout *configDialogBox = new QVBoxLayout( configDialogWidget );
        configDialogUncoupledChannelsCheckBox = new QCheckBox( i18n("Uncoupled channels"), configDialogWidget );
        configDialogUncoupledChannelsCheckBox->setToolTip( i18n("Use one mono stream per channel") );
        configDialogBox->addWidget( configDialogUncoupledChannelsCheckBox );

        configDialog.data()->setMainWidget( configDialogWidget );
        connect( configDialog.data(), SIGNAL( okClicked() ), this, SLOT( configDialogSave() ) );
        connect( configDialog.data(), SIGNAL( defaultClicked() ), this, SLOT( configDialogDefault() ) );
    }
    configDialogUncoupledChannelsCheckBox->setChecked( uncoupledChannels );
    configDialog.data()->show();
}

void soundkonverter_codec_opustools::configDialogSave()
{
    if( configDialog.data() )
    {
        uncoupledChannels = configDialogUncoupledChannelsCheckBox->isChecked();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "uncoupledChannels", uncoupledChannels );

        configDialog.data()->deleteLater();
    }
}

void soundkonverter_codec_opustools::configDialogDefault()
{
    if( configDialog.data() )
    {
        configDialogUncoupledChannelsCheckBox->setChecked( false );
    }
}

bool soundkonverter_codec_opustools::hasInfo()
{
    return false;
}

void soundkonverter_codec_opustools::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_opustools::newCodecWidget()
{
    OpusToolsCodecWidget *widget = new OpusToolsCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_opustools::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
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

QStringList soundkonverter_codec_opustools::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;
    OpusToolsConversionOptions *opusToolsConversionOptions = 0;
    if( conversionOptions->pluginName == name() )
    {
        opusToolsConversionOptions = static_cast<OpusToolsConversionOptions*>(conversionOptions);
    }

    if( outputCodec == "opus" )
    {
        command += binaries["opusenc"];
        command += "--bitrate";
        if( opusToolsConversionOptions )
        {
            command += QString::number(opusToolsConversionOptions->data.floatBitrate);
        }
        else
        {
            command += QString::number(conversionOptions->bitrate);
        }
        if( conversionOptions->bitrateMode == ConversionOptions::Abr )
        {
            command += "--vbr";
        }
        else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
        {
            command += "--hard-cbr";
        }
        // TODO --comp Encoding computational complexity (0-10, default: 10)
        if( uncoupledChannels )
        {
            command += "--uncoupled";
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["opusdec"];
        if( outputFile.isEmpty() )
        {
            command += "--quiet";
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_opustools::parseOutput( const QString& output )
{
    // [|] 00:00:30.66 15.3x realtime, 309.3kbit/s

    return -1;
}

ConversionOptions *soundkonverter_codec_opustools::conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    OpusToolsConversionOptions *options = new OpusToolsConversionOptions();
    options->fromXml( conversionOptions, filterOptionsElements );
    return options;
}

#include "soundkonverter_codec_opustools.moc"
