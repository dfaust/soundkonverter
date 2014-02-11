
#include "fluidsynthcodecglobal.h"

#include "soundkonverter_codec_fluidsynth.h"
#include "../../core/conversionoptions.h"
#include "fluidsynthcodecwidget.h"

#include <QApplication>
#include <KDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <KUrlRequester>


soundkonverter_codec_fluidsynth::soundkonverter_codec_fluidsynth( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["fluidsynth"] = "";

    allCodecs += "midi";
    allCodecs += "mod";
    allCodecs += "wav";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    soundFontFile = group.readEntry( "soundFontFile", KUrl() );
}

soundkonverter_codec_fluidsynth::~soundkonverter_codec_fluidsynth()
{}

QString soundkonverter_codec_fluidsynth::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_fluidsynth::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "midi";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["fluidsynth"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "midi", "fluidsynth" ) + "\n" + standardMessage( "install_opensource_backend", "fluidsynth" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "mod";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 90;
    newTrunk.enabled = ( binaries["fluidsynth"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "mod", "fluidsynth" ) + "\n" + standardMessage( "install_opensource_backend", "fluidsynth" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_fluidsynth::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_codec_fluidsynth::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    if( !configDialog.data() )
    {
        const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

        configDialog = new KDialog( parent );
        configDialog.data()->setCaption( i18n("Configure %1").arg(global_plugin_name)  );
        configDialog.data()->setButtons( KDialog::Ok | KDialog::Cancel );

        QWidget *configDialogWidget = new QWidget( configDialog.data() );
        QHBoxLayout *configDialogBox = new QHBoxLayout( configDialogWidget );
        QLabel *configDialogSoundFontLabel = new QLabel( i18n("Use SoundFont file:"), configDialogWidget );
        configDialogSoundFontLabel->setToolTip( i18n("In order to convert the midi data to a wave form you need a SoundFont which maps the midi data to sound effects.\nHave a look at %1 in order to get SoundFont files.",QString("http://sourceforge.net/apps/trac/fluidsynth/wiki/SoundFont")) );
        configDialogBox->addWidget( configDialogSoundFontLabel );
        configDialogSoundFontUrlRequester = new KUrlRequester( configDialogWidget );
        configDialogSoundFontUrlRequester->setMinimumWidth( 30*fontHeight );
        configDialogBox->addWidget( configDialogSoundFontUrlRequester );

        configDialog.data()->setMainWidget( configDialogWidget );
        connect( configDialog.data(), SIGNAL( okClicked() ), this, SLOT( configDialogSave() ) );
    }
    configDialogSoundFontUrlRequester->setUrl( soundFontFile );
    configDialog.data()->show();
}

void soundkonverter_codec_fluidsynth::configDialogSave()
{
    if( configDialog.data() )
    {
        soundFontFile = configDialogSoundFontUrlRequester->url().toLocalFile();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "soundFontFile", soundFontFile );

        configDialog.data()->deleteLater();
    }
}

bool soundkonverter_codec_fluidsynth::hasInfo()
{
    return false;
}

void soundkonverter_codec_fluidsynth::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

CodecWidget *soundkonverter_codec_fluidsynth::newCodecWidget()
{
    FluidsynthCodecWidget *widget = new FluidsynthCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_fluidsynth::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( soundFontFile.isEmpty() )
    {
        emit log( 1000, i18n("FluidSynth is not configured, yet. You need to set a SoundFont file.") );
        return BackendPlugin::BackendNeedsConfiguration;
    }

    const QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
    {
        return BackendPlugin::UnknownError;
    }

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

QStringList soundkonverter_codec_fluidsynth::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(_conversionOptions)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( soundFontFile.isEmpty() )
        return QStringList();

    if( outputFile.isEmpty() )
        return QStringList();

    QStringList command;

    if( outputCodec == "wav" )
    {
        command += binaries["fluidsynth"];
        command += "-l";
        command += "--fast-render";
        command += "\"" + escapeUrl(outputFile) + "\"";
        command += "\"" + escapeUrl(soundFontFile) + "\"";
        command += "\"" + escapeUrl(inputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_fluidsynth::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no output

    return -1;
}


#include "soundkonverter_codec_fluidsynth.moc"
