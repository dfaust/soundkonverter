
#include "aboutplugins.h"
#include "config.h"

#include <KLocale>
#include <KIcon>
#include <KPushButton>
#include <QLayout>
#include <QLabel>
#include <QListWidget>


AboutPlugins::AboutPlugins( Config *_config, QWidget* parent, Qt::WFlags f )
    : KDialog( parent, f ),
    config( _config ),
    currentPlugin( 0 )
{
    setCaption( i18n("About plugins") );
    setWindowIcon( KIcon("preferences-plugin") );
    setButtons( KDialog::Close );
    setButtonFocus( KDialog::Close );

    resize( 500, 300 );

    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QHBoxLayout *box = new QHBoxLayout( widget );

    QVBoxLayout *pluginListBox = new QVBoxLayout( widget );
    box->addLayout( pluginListBox );

    QLabel *installedPlugins = new QLabel( i18n("Installed plugins:"), this );
    pluginListBox->addWidget( installedPlugins );

    QListWidget *pluginsList = new QListWidget( this );
    pluginListBox->addWidget( pluginsList );
    connect( pluginsList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(currentPluginChanged(const QString&)) );

    QStringList pluginNames;
    QList<CodecPlugin*> codecPlugins = config->pluginLoader()->getAllCodecPlugins();
    for( int i=0; i<codecPlugins.count(); i++ )
    {
        pluginNames += codecPlugins.at(i)->name();
    }
    pluginNames.sort();
    pluginsList->addItems( pluginNames );

    pluginNames.clear();
    QList<ReplayGainPlugin*> replaygainPlugins = config->pluginLoader()->getAllReplayGainPlugins();
    for( int i=0; i<replaygainPlugins.count(); i++ )
    {
        pluginNames += replaygainPlugins.at(i)->name();
    }
    pluginNames.sort();
    pluginsList->addItems( pluginNames );

    pluginNames.clear();
    QList<RipperPlugin*> ripperPlugins = config->pluginLoader()->getAllRipperPlugins();
    for( int i=0; i<ripperPlugins.count(); i++ )
    {
        pluginNames += ripperPlugins.at(i)->name();
    }
    pluginNames.sort();
    pluginsList->addItems( pluginNames );

    pluginsList->setFixedWidth( 180 );

    box->addSpacing( 10 );

    QVBoxLayout *pluginInfoBox = new QVBoxLayout( widget );
    box->addLayout( pluginInfoBox );

    aboutPluginLabel = new QLabel( this );
    aboutPluginLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    aboutPluginLabel->setWordWrap( true );
    pluginInfoBox->addWidget( aboutPluginLabel );

    pluginInfoBox->addStretch();

    QHBoxLayout *configurePluginBox = new QHBoxLayout( widget );
    pluginInfoBox->addLayout( configurePluginBox );
    configurePlugin = new KPushButton( KIcon("configure"), "", widget );
    configurePlugin->hide();
    configurePluginBox->addWidget( configurePlugin );
    configurePluginBox->addStretch();
    connect( configurePlugin, SIGNAL(clicked()), this, SLOT(configurePluginClicked()) );

    pluginsList->setCurrentRow(0);
    QListWidgetItem *currentItem = pluginsList->currentItem();
    if( currentItem )
    {
        currentPluginChanged( currentItem->text() );
    }
}

AboutPlugins::~AboutPlugins()
{}

void AboutPlugins::currentPluginChanged( const QString& pluginName )
{
    currentPlugin = config->pluginLoader()->backendPluginByName( pluginName );
    if( !currentPlugin )
    {
        aboutPluginLabel->setText( "" );
        return;
    }

    QStringList info;
    info += i18n("About plugin %1:",pluginName);

    info += i18n("Plugin type: %1",currentPlugin->type());

    QMap<QString,QString> binaries = currentPlugin->binaries;
    QStringList binariesString;
    if( binaries.count() > 0 )
    {
        binariesString += i18n("Backend binaries:");
    }
    for( int i=0; i<binaries.count(); i++ )
    {
        if( !binaries.values().at(i).isEmpty() )
            binariesString += i18n("%1 found at: %2",binaries.keys().at(i),binaries.values().at(i));
        else
            binariesString += i18n("%1 (not found)",binaries.keys().at(i));
    }
    info += binariesString.join("\n");

    if( currentPlugin->type() == "codec" )
    {
        CodecPlugin *codecPlugin = (CodecPlugin*)currentPlugin;

        QStringList codecsString;
        QSet<QString> encode;
        QSet<QString> decode;
        QList<ConversionPipeTrunk> codecTable = codecPlugin->codecTable();
        for( int i=0; i<codecTable.count(); i++ )
        {
            if( codecTable.at(i).codecTo != "wav" )
                encode += codecTable.at(i).codecTo;

            if( codecTable.at(i).codecFrom != "wav" )
                decode += codecTable.at(i).codecFrom;
        }
        QStringList list;
        codecsString += i18n("Supported codecs:");
        list = encode.toList();
        list.sort();
        codecsString += i18n("Encode: %1",list.join(", "));
        list = decode.toList();
        list.sort();
        codecsString += i18n("Decode: %1",list.join(", "));
        info += codecsString.join("\n");
    }
    else if( currentPlugin->type() == "replaygain" )
    {
        ReplayGainPlugin *replaygainPlugin = (ReplayGainPlugin*)currentPlugin;

        QStringList codecs;
        QList<ReplayGainPipe> codecTable = replaygainPlugin->codecTable();
        for( int i=0; i<codecTable.count(); i++ )
        {
            codecs += codecTable.at(i).codecName;
        }
        codecs.sort();
        info += QString( i18n("Supported codecs:") + "\n" + codecs.join(", ") );
    }
    else if( currentPlugin->type() == "ripper" )
    {
    }

    aboutPluginLabel->setText( info.join("\n\n") );

    if( currentPlugin->isConfigSupported(BackendPlugin::General,"") )
    {
        configurePlugin->setText( i18n("Configure %1 ...",currentPlugin->name()) );
        configurePlugin->show();
    }
    else
    {
        configurePlugin->hide();
    }
}

void AboutPlugins::configurePluginClicked()
{
    if( currentPlugin )
    {
        currentPlugin->showConfigDialog( BackendPlugin::General, "", this );
    }
}

