
#include "aboutplugins.h"
#include "config.h"

#include <KLocale>
#include <KIcon>
#include <KPushButton>
#include <QLayout>
#include <QLabel>
#include <QListWidget>
#include <QToolTip>


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
    aboutPluginLabel->setTextFormat( Qt::RichText );
    pluginInfoBox->addWidget( aboutPluginLabel );
    connect( aboutPluginLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(showProblemInfo(const QString&)) );

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
            binariesString += i18n("%1 (found at: %2)",binaries.keys().at(i),"<span style=\"color:green\">" + binaries.values().at(i) + "</span>");
        else
            binariesString += "<span style=\"color:red\">" + i18n("%1 (not found)",binaries.keys().at(i)) + "</span>";
    }
    info += binariesString.join("<br>");

    problemInfos.clear();
    if( currentPlugin->type() == "codec" )
    {
        CodecPlugin *codecPlugin = (CodecPlugin*)currentPlugin;

        QStringList codecsString;
        QMap<QString,bool> encodeCodecs;
        QMap<QString,bool> decodeCodecs;
        QList<ConversionPipeTrunk> codecTable = codecPlugin->codecTable();
        for( int i=0; i<codecTable.count(); i++ )
        {
            if( codecTable.at(i).codecTo != "wav" && ( !encodeCodecs.contains(codecTable.at(i).codecTo) || !encodeCodecs[codecTable.at(i).codecTo] ) )
                encodeCodecs[codecTable.at(i).codecTo] = codecTable.at(i).enabled;

            if( codecTable.at(i).codecFrom != "wav" && ( !decodeCodecs.contains(codecTable.at(i).codecFrom) || !decodeCodecs[codecTable.at(i).codecFrom] ) )
                decodeCodecs[codecTable.at(i).codecFrom] = codecTable.at(i).enabled;
        }
        codecsString += i18n("Supported codecs:");
        QStringList list;
        for( int i=0; i<encodeCodecs.count(); i++ )
        {
            const QString codecName = encodeCodecs.keys().at(i);
            problemInfos["encode-"+codecName] = config->pluginLoader()->pluginEncodeProblems( pluginName, codecName );
            list += encodeCodecs.values().at(i) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"encode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Encode: %1",list.join(", "));
        list.clear();
        for( int i=0; i<decodeCodecs.count(); i++ )
        {
            const QString codecName = decodeCodecs.keys().at(i);
            problemInfos["decode-"+codecName] = config->pluginLoader()->pluginDecodeProblems( pluginName, codecName );
            list += decodeCodecs.values().at(i) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"decode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Decode: %1",list.join(", "));
        info += codecsString.join("<br>");
    }
    else if( currentPlugin->type() == "replaygain" )
    {
        ReplayGainPlugin *replaygainPlugin = (ReplayGainPlugin*)currentPlugin;

        QStringList codecs;
        QList<ReplayGainPipe> codecTable = replaygainPlugin->codecTable();
        for( int i=0; i<codecTable.count(); i++ )
        {
            const QString codecName = codecTable.at(i).codecName;
            problemInfos["replaygain-"+codecName] = config->pluginLoader()->pluginReplayGainProblems( pluginName, codecName );
            codecs += codecTable.at(i).enabled ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"replaygain-"+codecName+"\">" + codecName + "</a>";
        }
        info += QString( i18n("Supported codecs:") + "<br>" + codecs.join(", ") );
    }
    else if( currentPlugin->type() == "ripper" )
    {
    }

    aboutPluginLabel->setText( info.join("<br><br>") );

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

void AboutPlugins::showProblemInfo( const QString& problemId )
{
    QToolTip::showText( QCursor::pos(), problemInfos[problemId], aboutPluginLabel );
}

