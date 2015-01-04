
#include "aboutplugins.h"
#include "config.h"

#include <KLocalizedString>
#include <QToolTip>
#include <QtCore> // KDevelop foreach syntax highlighting fix

AboutPlugins::AboutPlugins(Config *_config, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    config(_config),
    currentPlugin(0)
{
//     const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    ui.setupUi(this);

    connect(ui.closeButton, SIGNAL(clicked()),                          this, SLOT(close()));
    connect(ui.pluginsList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(currentPluginChanged(const QString&)));
    connect(ui.aboutPluginLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(showProblemInfo(const QString&)));
    connect(ui.configurePluginButton, SIGNAL(clicked()),                this, SLOT(configurePluginButtonClicked()));

    QStringList pluginNames;

    foreach( CodecPlugin *plugin, config->pluginLoader()->getAllCodecPlugins() )
    {
        pluginNames += plugin->name();
    }
    pluginNames.sort();
    ui.pluginsList->addItems(pluginNames);
    pluginNames.clear();

    foreach( FilterPlugin *plugin, config->pluginLoader()->getAllFilterPlugins() )
    {
        pluginNames += plugin->name();
    }
    pluginNames.sort();
    ui.pluginsList->addItems(pluginNames);
    pluginNames.clear();

    foreach( ReplayGainPlugin *plugin, config->pluginLoader()->getAllReplayGainPlugins() )
    {
        pluginNames += plugin->name();
    }
    pluginNames.sort();
    ui.pluginsList->addItems(pluginNames);
    pluginNames.clear();

    foreach( RipperPlugin *plugin, config->pluginLoader()->getAllRipperPlugins() )
    {
        pluginNames += plugin->name();
    }
    pluginNames.sort();
    ui.pluginsList->addItems(pluginNames);
    pluginNames.clear();

//     ui.pluginsList->setFixedWidth( 15*fontHeight );

    ui.pluginsList->setCurrentRow(0);
    if( QListWidgetItem *currentItem = ui.pluginsList->currentItem() )
    {
        currentPluginChanged(currentItem->text());
    }

//     setInitialSize( QSize(50*fontHeight,40*fontHeight) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "AboutPlugins" );
//     restoreDialogSize( group );
}

AboutPlugins::~AboutPlugins()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "AboutPlugins" );
//     saveDialogSize( group );
}

void AboutPlugins::currentPluginChanged(const QString& pluginName)
{
    currentPlugin = config->pluginLoader()->backendPluginByName(pluginName);
    if( !currentPlugin )
    {
        ui.aboutPluginLabel->setText( "" );
        return;
    }

    QStringList info;
    info += i18n("About plugin %1:", pluginName);

    info += i18n("Plugin type: %1", currentPlugin->type());

    QMap<QString, QString> binaries = currentPlugin->binaries;
    QStringList binariesString;
    if( binaries.count() > 0 )
    {
        binariesString += i18n("Backend binaries:");
    }
    foreach( const QString& binary, binaries.keys() )
    {
        if( !binaries.value(binary).isEmpty() )
            binariesString += i18n("%1 (found at: %2)", binary, "<span style=\"color:green\">" + binaries.value(binary) + "</span>");
        else
            binariesString += "<span style=\"color:red\">" + i18n("%1 (not found)", binary) + "</span>";
    }
    info += binariesString.join("<br>");

    problemInfos.clear();
    if( currentPlugin->type() == "codec" )
    {
        CodecPlugin *codecPlugin = qobject_cast<CodecPlugin*>(currentPlugin);

        QStringList codecsString;
        QMap<QString, int> encodeCodecs;
        QMap<QString, int> decodeCodecs;
        foreach( const ConversionPipeTrunk& trunk, codecPlugin->codecTable() )
        {
            if( trunk.codecTo != "wav" )
                encodeCodecs[trunk.codecTo] += trunk.enabled;

            if( trunk.codecFrom != "wav" )
                decodeCodecs[trunk.codecFrom] += trunk.enabled;
        }
        codecsString += i18n("Supported codecs:");
        QStringList list;
        foreach( const QString& codecName, encodeCodecs.keys() )
        {
            problemInfos["encode-"+codecName] = i18n("Currently deactivated.") + "\n\n" + config->pluginLoader()->pluginEncodeProblems(pluginName, codecName);
            list += encodeCodecs.value(codecName) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"encode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Encode: %1", list.join(", "));
        list.clear();
        foreach( const QString& codecName, decodeCodecs.keys() )
        {
            problemInfos["decode-"+codecName] = i18n("Currently deactivated.") + "\n\n" + config->pluginLoader()->pluginEncodeProblems(pluginName, codecName);
            list += decodeCodecs.value(codecName) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"decode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Decode: %1", list.join(", "));
        info += codecsString.join("<br>");
    }
    else if( currentPlugin->type() == "filter" )
    {
        CodecPlugin *codecPlugin = qobject_cast<CodecPlugin*>(currentPlugin);

        QStringList codecsString;
        QMap<QString, int> encodeCodecs;
        QMap<QString, int> decodeCodecs;
        foreach( const ConversionPipeTrunk& trunk, codecPlugin->codecTable() )
        {
            if( trunk.codecTo != "wav" )
                encodeCodecs[trunk.codecTo] += trunk.enabled;

            if( trunk.codecFrom != "wav" )
                decodeCodecs[trunk.codecFrom] += trunk.enabled;
        }
        codecsString += i18n("Supported codecs:");
        QStringList list;
        foreach( const QString& codecName, encodeCodecs.keys() )
        {
            problemInfos["encode-"+codecName] = i18n("Currently deactivated.") + "\n\n" + config->pluginLoader()->pluginEncodeProblems(pluginName, codecName);
            list += encodeCodecs.value(codecName) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"encode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Encode: %1", list.join(", "));
        list.clear();
        foreach( const QString& codecName, decodeCodecs.keys() )
        {
            problemInfos["decode-"+codecName] = i18n("Currently deactivated.") + "\n\n" + config->pluginLoader()->pluginEncodeProblems(pluginName, codecName);
            list += decodeCodecs.value(codecName) ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"decode-"+codecName+"\">" + codecName + "</a>";
        }
        codecsString += i18n("Decode: %1", list.join(", "));
        info += codecsString.join("<br>");
    }
    else if( currentPlugin->type() == "replaygain" )
    {
        ReplayGainPlugin *replaygainPlugin = qobject_cast<ReplayGainPlugin*>(currentPlugin);

        QStringList codecs;
        foreach( const ReplayGainPipe& trunk, replaygainPlugin->codecTable() )
        {
            const QString codecName = trunk.codecName;
            problemInfos["replaygain-"+codecName] = i18n("Currently deactivated.") + "\n\n" + config->pluginLoader()->pluginReplayGainProblems(pluginName, codecName);
            codecs += trunk.enabled ? "<span style=\"color:green\">" + codecName + "</span>" : "<a style=\"color:red\" href=\"replaygain-"+codecName+"\">" + codecName + "</a>";
        }
        info += QString( i18n("Supported codecs:") + "<br>" + codecs.join(", ") );
    }
    else if( currentPlugin->type() == "ripper" )
    {
    }

    ui.aboutPluginLabel->setText(info.join("<br><br>"));

    if( currentPlugin->isConfigSupported(BackendPlugin::General, "") )
    {
        ui.configurePluginButton->setText(i18n("Configure %1 ...", currentPlugin->name()));
        ui.configurePluginButton->show();
    }
    else
    {
        ui.configurePluginButton->hide();
    }
}

void AboutPlugins::configurePluginButtonClicked()
{
    if( currentPlugin )
    {
        currentPlugin->showConfigDialog(BackendPlugin::General, "", this);
    }
}

void AboutPlugins::showProblemInfo(const QString& problemId)
{
    QToolTip::showText(QCursor::pos(), problemInfos.value(problemId), ui.aboutPluginLabel);
}

