
#include "cdparanoiaripperglobal.h"

#include "soundkonverter_ripper_cdparanoia.h"

#include <QWidget>
#include <QLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <KLocale>
#include <KDialog>


soundkonverter_ripper_cdparanoia::soundkonverter_ripper_cdparanoia( QObject *parent, const QStringList& args  )
    : RipperPlugin( parent )
{
    Q_UNUSED(args)

    binaries["cdparanoia"] = "";

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "Plugin-"+name() );
    forceReadSpeed = group.readEntry( "forceReadSpeed", 0 );
    forceEndianness = group.readEntry( "forceEndianness", 0 );
    maximumRetries = group.readEntry( "maximumRetries", 20 );
    enableParanoia = group.readEntry( "enableParanoia", true );
    enableExtraParanoia = group.readEntry( "enableExtraParanoia", true );
}

soundkonverter_ripper_cdparanoia::~soundkonverter_ripper_cdparanoia()
{}

QString soundkonverter_ripper_cdparanoia::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_ripper_cdparanoia::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "audio cd";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["cdparanoia"] != "" );
    newTrunk.data.canRipEntireCd = true;
    newTrunk.problemInfo = i18n( "In order to rip audio cds per track or to a single file, you need to install 'cdparanoia'.\n'cdparanoia' is usually shipped with your distribution, the package name can vary." );
    table.append( newTrunk );

    return table;
}

bool soundkonverter_ripper_cdparanoia::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_ripper_cdparanoia::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
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

        QHBoxLayout *configDialogBox0 = new QHBoxLayout();
        configDialogForceReadSpeedCheckBox = new QCheckBox( i18n("Force read speed:"), configDialogWidget );
        configDialogBox0->addWidget( configDialogForceReadSpeedCheckBox );
        configDialogForceReadSpeedSpinBox = new QSpinBox( configDialogWidget );
        configDialogForceReadSpeedSpinBox->setRange(1, 64);
        configDialogForceReadSpeedSpinBox->setSuffix(" x");
        configDialogBox0->addWidget( configDialogForceReadSpeedSpinBox );
        configDialogBox->addLayout( configDialogBox0 );
        connect( configDialogForceReadSpeedCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( configDialogForceReadSpeedChanged(int) ) );

        QHBoxLayout *configDialogBox1 = new QHBoxLayout();
        QLabel *configDialogForceEndiannessLabel = new QLabel( i18nc("Byte-Order", "Endianness:"), configDialogWidget );
        configDialogBox1->addWidget( configDialogForceEndiannessLabel );
        configDialogForceEndiannessComboBox = new QComboBox( configDialogWidget );
        configDialogForceEndiannessComboBox->addItem( "Auto" );
        configDialogForceEndiannessComboBox->addItem( "Little endian" );
        configDialogForceEndiannessComboBox->addItem( "Big endian" );
        configDialogBox1->addWidget( configDialogForceEndiannessComboBox );
        configDialogBox->addLayout( configDialogBox1 );

        QHBoxLayout *configDialogBox2 = new QHBoxLayout();
        QLabel *configDialogMaximumRetriesLabel = new QLabel( i18n("Maximum read retries:"), configDialogWidget );
        configDialogBox2->addWidget( configDialogMaximumRetriesLabel );
        configDialogMaximumRetriesSpinBox = new QSpinBox( configDialogWidget );
        configDialogMaximumRetriesSpinBox->setRange(0, 100);
        configDialogBox2->addWidget( configDialogMaximumRetriesSpinBox );
        configDialogBox->addLayout( configDialogBox2 );

        QHBoxLayout *configDialogBox3 = new QHBoxLayout( configDialogWidget );
        configDialogEnableParanoiaCheckBox = new QCheckBox( i18n("Enable paranoia"), configDialogWidget );
        configDialogBox3->addWidget( configDialogEnableParanoiaCheckBox );
        configDialogBox->addLayout( configDialogBox3 );

        QHBoxLayout *configDialogBox4 = new QHBoxLayout( configDialogWidget );
        configDialogEnableExtraParanoiaCheckBox = new QCheckBox( i18n("Enable extra paranoia"), configDialogWidget );
        configDialogBox4->addWidget( configDialogEnableExtraParanoiaCheckBox );
        configDialogBox->addLayout( configDialogBox4 );

        configDialog.data()->setMainWidget( configDialogWidget );
        connect( configDialog.data(), SIGNAL( okClicked() ), this, SLOT( configDialogSave() ) );
        connect( configDialog.data(), SIGNAL( defaultClicked() ), this, SLOT( configDialogDefault() ) );
    }
    configDialogForceReadSpeedCheckBox->setChecked( forceReadSpeed > 0 );
    configDialogForceReadSpeedSpinBox->setValue( forceReadSpeed );
    configDialogForceEndiannessComboBox->setCurrentIndex( forceEndianness );
    configDialogMaximumRetriesSpinBox->setValue( maximumRetries );
    configDialogEnableParanoiaCheckBox->setChecked( enableParanoia );
    configDialogEnableExtraParanoiaCheckBox->setChecked( enableExtraParanoia );

    configDialogForceReadSpeedChanged( configDialogForceReadSpeedCheckBox->checkState() );

    configDialog.data()->show();
}

void soundkonverter_ripper_cdparanoia::configDialogForceReadSpeedChanged( int state )
{
    if( configDialog.data() )
    {
        configDialogForceReadSpeedSpinBox->setEnabled( state == Qt::Checked );
    }
}

void soundkonverter_ripper_cdparanoia::configDialogSave()
{
    if( configDialog.data() )
    {
        forceReadSpeed = configDialogForceReadSpeedCheckBox->isChecked() ? configDialogForceReadSpeedSpinBox->value() : 0;
        forceEndianness = configDialogForceEndiannessComboBox->currentIndex();
        maximumRetries = configDialogMaximumRetriesSpinBox->value();
        enableParanoia = configDialogEnableParanoiaCheckBox->isChecked();
        enableExtraParanoia = configDialogEnableExtraParanoiaCheckBox->isChecked();

        KSharedConfig::Ptr conf = KGlobal::config();
        KConfigGroup group;

        group = conf->group( "Plugin-"+name() );
        group.writeEntry( "forceReadSpeed", forceReadSpeed );
        group.writeEntry( "forceEndianness", forceEndianness );
        group.writeEntry( "maximumRetries", maximumRetries );
        group.writeEntry( "enableParanoia", enableParanoia );
        group.writeEntry( "enableExtraParanoia", enableExtraParanoia );

        configDialog.data()->deleteLater();
    }
}

void soundkonverter_ripper_cdparanoia::configDialogDefault()
{
    if( configDialog.data() )
    {
        configDialogForceReadSpeedCheckBox->setChecked( false );
        configDialogForceReadSpeedSpinBox->setValue( 1 );
        configDialogForceEndiannessComboBox->setCurrentIndex( 0 );
        configDialogMaximumRetriesSpinBox->setValue( 20 );
        configDialogEnableParanoiaCheckBox->setChecked( true );
        configDialogEnableExtraParanoiaCheckBox->setChecked( true );
    }
}

bool soundkonverter_ripper_cdparanoia::hasInfo()
{
    return false;
}

void soundkonverter_ripper_cdparanoia::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

unsigned int soundkonverter_ripper_cdparanoia::rip( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    QStringList command;

    command += binaries["cdparanoia"];
    command += "--stderr-progress";
    command += "--force-cdrom-device";
    command += device;
    if( forceReadSpeed > 0 )
    {
        command += "--force-read-speed";
        command += QString::number(forceReadSpeed);
    }
    if( forceEndianness == 1 )
    {
        command += "--force-cdrom-little-endian";
    }
    else if( forceEndianness == 2 )
    {
        command += "--force-cdrom-big-endian";
    }
    command += "--never-skip=" + QString::number(maximumRetries);
    if( !enableExtraParanoia )
    {
        if( !enableParanoia )
        {
            command += "--disable-paranoia";
        }
        else
        {
            command += "--disable-extra-paranoia";
        }
    }
    else if( !enableParanoia )
    {
        command += "--disable-paranoia";
    }
    if( track > 0 )
    {
        command += QString::number(track);
    }
    else
    {
        command += "1-" + QString::number(tracks);
    }
    command += "\"" + outputFile.toLocalFile() + "\"";

    RipperPluginItem *newItem = new RipperPluginItem( this );
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

QStringList soundkonverter_ripper_cdparanoia::ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    Q_UNUSED(device)
    Q_UNUSED(track)
    Q_UNUSED(tracks)
    Q_UNUSED(outputFile)

    return QStringList();
}

float soundkonverter_ripper_cdparanoia::parseOutput( const QString& output, int *fromSector, int *toSector )
{
    // Ripping from sector       0 (track  1 [0:00.00])
    //           to sector   16361 (track  1 [3:38.11])

    // ##: -2 [wrote] @ 19242887\n

    if( output.contains("sector") )
    {
        if( fromSector && output.contains("from sector") )
        {
            QString data = output;
            data.remove( 0, data.indexOf("from sector") + 11 );
            data = data.left( data.indexOf("(") );
            data = data.simplified();
            *fromSector = data.toInt();
        }
        if( toSector && output.contains("to sector") )
        {
            QString data = output;
            data.remove( 0, data.indexOf("to sector") + 9 );
            data = data.left( data.indexOf("(") );
            data = data.simplified();
            *toSector = data.toInt();
        }
        return -1;
    }

    if( output == "" || !output.contains("@") ) return -1;
    if( !output.contains("[wrote] @") ) return 0;

    QString data = output;
    data.remove( 0, data.indexOf("[wrote] @") + 9 );
    data = data.left( data.indexOf("\n") );
    data = data.simplified();
    return data.toFloat() / 1176;
}

float soundkonverter_ripper_cdparanoia::parseOutput( const QString& output )
{
    return parseOutput( output, 0, 0 );
}

void soundkonverter_ripper_cdparanoia::processOutput()
{
    RipperPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<RipperPluginItem*>(backendItems.at(i));

            progress = parseOutput( output, &pluginItem->data.fromSector, &pluginItem->data.toSector );

            if( progress == -1 && !output.simplified().isEmpty() )
                logOutput( backendItems.at(i)->id, output );

            progress = (progress-pluginItem->data.fromSector) * 100 / (pluginItem->data.toSector-pluginItem->data.fromSector);

            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

#include "soundkonverter_ripper_cdparanoia.moc"
