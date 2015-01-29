
#include "configbackendspage.h"
#include "../config.h"
#include "../core/codecplugin.h"
#include "../core/filterplugin.h"
#include "../core/ripperplugin.h"

#include <KLocalizedString>

#include <QCheckBox>
#include <QMessageBox>

ConfigBackendsPage::ConfigBackendsPage(Config *_config, QWidget *parent) :
    ConfigPageBase(parent),
    config(_config)
{
    ui.setupUi(this);

    foreach( const Config::CodecData & codec, config->data.backends.codecs )
    {
        if( codec.codecName == "audio cd" )
        {
            ui.ripperComboBox->addItems(codec.decoders);
        }
    }

    connect(ui.ripperComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));
    connect(ui.ripperComboBox, SIGNAL(activated(const QString &)), this, SLOT(ripperChanged(const QString &)));
    connect(ui.configureRipperButton, SIGNAL(clicked()), this, SLOT(configureRipper()));

    int row = 0;

    foreach( const QString& filterPluginName, config->data.backends.filters )
    {
        if( row == 0 )
        {
            QLabel *filterLabel = new QLabel(i18n("Enable plugins:"), this);
            ui.filtersGridLayout->addWidget(filterLabel, row, 0);
        }

        QCheckBox *filterCheckBox = new QCheckBox(filterPluginName, this);
        filterCheckBox->setChecked(config->data.backends.enabledFilters.contains(filterPluginName));
        ui.filtersGridLayout->addWidget(filterCheckBox, row, 1);
        filterCheckBoxes.append(filterCheckBox);
        connect(filterCheckBox, SIGNAL(stateChanged(int)), this, SLOT(somethingChanged()));

        QToolButton *configureFilterButton = new QToolButton(this);
        configureFilterButton->setIcon(QIcon::fromTheme("configure"));
        configureFilterButton->setAutoRaise(true);
        ui.filtersGridLayout->addWidget(configureFilterButton, row, 1);
        connect(configureFilterButton, SIGNAL(clicked()), this, SLOT(configureFilter()));
        filterConfigButtons.append(configureFilterButton);

        FilterPlugin *plugin = qobject_cast<FilterPlugin *>(config->pluginLoader()->backendPluginByName(filterPluginName));

        if( plugin )
        {
            configureFilterButton->setEnabled(plugin->isConfigSupported(BackendPlugin::General, ""));
        }
        else
        {
            configureFilterButton->setEnabled(false);
        }

        if( configureFilterButton->isEnabled() )
            configureFilterButton->setToolTip(i18n("Configure %1 ...", filterPluginName));

        row++;
    }

    ui.filtersGridLayout->setColumnStretch(0, 2);
    ui.filtersGridLayout->setColumnStretch(1, 1);

    QStringList formats = config->pluginLoader()->formatList(PluginLoader::Possibilities(PluginLoader::Encode | PluginLoader::Decode | PluginLoader::ReplayGain), PluginLoader::CompressionType(PluginLoader::InferiorQuality | PluginLoader::Lossy | PluginLoader::Lossless | PluginLoader::Hybrid));
    formats.removeAll("wav");
    formats.removeAll("audio cd");
    ui.prioritiesFormatComboBox->addItems(formats);
    connect(ui.prioritiesFormatComboBox, SIGNAL(activated(const QString &)), this, SLOT(formatChanged(const QString &)));

    ui.prioritiesDecodersGroupBox->init(BackendPlugin::Decoder, config);
    ui.prioritiesEncodersGroupBox->init(BackendPlugin::Encoder, config);
    ui.prioritiesReplaygainGroupBox->init(BackendPlugin::ReplayGain, config);

    connect(ui.prioritiesDecodersGroupBox, SIGNAL(orderChanged()), this, SLOT(somethingChanged()));
    connect(ui.prioritiesEncodersGroupBox, SIGNAL(orderChanged()), this, SLOT(somethingChanged()));
    connect(ui.prioritiesReplaygainGroupBox, SIGNAL(orderChanged()), this, SLOT(somethingChanged()));

    connect(ui.optimizeButton, SIGNAL(clicked()), this, SLOT(showOptimizations()));

    ripperChanged(ui.ripperComboBox->currentText());
    formatChanged(ui.prioritiesFormatComboBox->currentText());
}

ConfigBackendsPage::~ConfigBackendsPage()
{
}

void ConfigBackendsPage::ripperChanged(const QString& pluginName)
{
    RipperPlugin *plugin = qobject_cast<RipperPlugin*>(config->pluginLoader()->backendPluginByName(pluginName));

    if( plugin )
    {
        ui.configureRipperButton->setEnabled(plugin->isConfigSupported(BackendPlugin::General, ""));
    }
    else
    {
        ui.configureRipperButton->setEnabled(false);
    }

    if( ui.configureRipperButton->isEnabled() )
        ui.configureRipperButton->setToolTip(i18n("Configure %1 ...", pluginName));
    else
        ui.configureRipperButton->setToolTip("");
}

void ConfigBackendsPage::formatChanged(const QString& format, bool ignoreChanges)
{
    QStringList plugins;

    if( !ignoreChanges && ( ui.prioritiesDecodersGroupBox->changed() || ui.prioritiesEncodersGroupBox->changed() || ui.prioritiesReplaygainGroupBox->changed() ) )
    {
        const int ret = QMessageBox::question( this, i18n("Settings changed"), i18n("You have changed the current settings.\nDo you want to save them?") );
        if( ret == QMessageBox::Yes )
        {
            saveSettings();
            config->save();
        }
    }

    currentFormat = format;

    ui.prioritiesDecodersGroupBox->clear();
    ui.prioritiesEncodersGroupBox->clear();
    ui.prioritiesReplaygainGroupBox->clear();

    ui.prioritiesDecodersGroupBox->setFormat(format);
    ui.prioritiesEncodersGroupBox->setFormat(format);
    ui.prioritiesReplaygainGroupBox->setFormat(format);

    foreach( const Config::CodecData codec, config->data.backends.codecs )
    {
        if( codec.codecName == format )
        {
            foreach( const QString& decoder, codec.decoders )
            {
                ui.prioritiesDecodersGroupBox->addItem(decoder);
            }

            foreach( const QString& encoder, codec.encoders )
            {
                ui.prioritiesEncodersGroupBox->addItem(encoder);
            }

            foreach( const QString& replaygain, codec.replaygain )
            {
                ui.prioritiesReplaygainGroupBox->addItem(replaygain);
            }
        }
    }

    ui.prioritiesDecodersGroupBox->resetOrder();
    ui.prioritiesEncodersGroupBox->resetOrder();
    ui.prioritiesReplaygainGroupBox->resetOrder();

    emit configChanged( false );
}

void ConfigBackendsPage::resetDefaults()
{
    // rippers
    {
        QStringList allPlugins;

        foreach( RipperPlugin *plugin, config->pluginLoader()->getAllRipperPlugins() )
        {
            const QString pluginName = plugin->name();

            foreach( const ConversionPipeTrunk trunk, plugin->codecTable() )
            {
                if( trunk.enabled && allPlugins.filter(QRegExp("[0-9]{8,8}" + pluginName)).count() == 0 )
                {
                    allPlugins += QString::number(trunk.rating).rightJustified(8, '0') + pluginName;
                    break;
                }
            }
        }

        allPlugins.sort();

        if( allPlugins.count() > 0 )
        {
            const QString defaultRipper = allPlugins.first().right(allPlugins.first().length() - 8);
            ui.ripperComboBox->setCurrentIndex(ui.ripperComboBox->findText(defaultRipper));
        }
    }

    // filters
    {
        int i = 0;

        foreach( QCheckBox *checkBox, filterCheckBoxes )
        {
            checkBox->setChecked(i == 0);
            i++;
        }
    }

    // codecs
    {
        const int answer = QMessageBox::question(this, i18n("Backend optimization"), i18n("This will choose the best backends for all formats and save the new preferences immediately.\n\nDo you want to continue?"));

        if( answer == QMessageBox::Yes )
        {
            QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations(true);

            for( int i=0; i<optimizationList.count(); i++ )
            {
                optimizationList[i].solution = CodecOptimizations::Optimization::Fix;
            }

            config->doOptimizations(optimizationList);

            formatChanged(ui.prioritiesFormatComboBox->currentText(), true);

            emit configChanged( false );
        }
    }
}

void ConfigBackendsPage::saveSettings()
{
    // rippers
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == "audio cd" )
        {
            const QString currentRipper = ui.ripperComboBox->currentText();

            QStringList rippers = config->data.backends.codecs[i].decoders;
            rippers.removeAll(currentRipper);
            rippers.prepend(currentRipper);

            config->data.backends.codecs[i].decoders = rippers;
        }
    }

    // filters
    {
        config->data.backends.enabledFilters.clear();

        int i = 0;

        foreach( QCheckBox *checkBox, filterCheckBoxes )
        {
            const QString filterPluginName = config->data.backends.filters.at(i);

            if( checkBox->isChecked() )
                config->data.backends.enabledFilters.append(filterPluginName);

            i++;
        }
    }

    // codecs
    {
        for( int i=0; i<config->data.backends.codecs.count(); i++ )
        {
            if( config->data.backends.codecs.at(i).codecName == currentFormat )
            {
                config->data.backends.codecs[i].decoders = ui.prioritiesDecodersGroupBox->getList();
                config->data.backends.codecs[i].encoders = ui.prioritiesEncodersGroupBox->getList();
                config->data.backends.codecs[i].replaygain = ui.prioritiesReplaygainGroupBox->getList();
            }
        }

        ui.prioritiesDecodersGroupBox->resetOrder();
        ui.prioritiesEncodersGroupBox->resetOrder();
        ui.prioritiesReplaygainGroupBox->resetOrder();

        // ensure that the user won't get bothered by an optimization message at the next start
        QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations(true);

        for( int i=0; i<optimizationList.count(); i++ )
        {
            optimizationList[i].solution = CodecOptimizations::Optimization::Ignore;
        }

        config->data.backendOptimizationsIgnoreList.optimizationList = optimizationList;
    }
}

void ConfigBackendsPage::somethingChanged()
{
    bool changed = false;

    int i = 0;

    foreach( QCheckBox *checkBox, filterCheckBoxes )
    {
        if( checkBox == QObject::sender() )
        {
            const QString filterPluginName = config->data.backends.filters.at(i);

            if( checkBox->isChecked() != config->data.backends.enabledFilters.contains(filterPluginName) )
            {
                changed = true;
            }

            break;
        }

        i++;
    }

    if( !changed && ( ui.prioritiesDecodersGroupBox->changed() || ui.prioritiesEncodersGroupBox->changed() || ui.prioritiesReplaygainGroupBox->changed() ) )
        changed = true;

    emit configChanged(changed);
}

void ConfigBackendsPage::configureRipper()
{
    RipperPlugin *plugin = qobject_cast<RipperPlugin*>(config->pluginLoader()->backendPluginByName(ui.ripperComboBox->currentText()));

    if( plugin )
    {
        plugin->showConfigDialog(BackendPlugin::General, "", this);
    }
}

void ConfigBackendsPage::configureFilter()
{
    int i = 0;

    foreach( QToolButton *configButton, filterConfigButtons )
    {
        if( configButton == QObject::sender() )
        {
            const QString filterPluginName = config->data.backends.filters.at(i);

            FilterPlugin *plugin = qobject_cast<FilterPlugin*>(config->pluginLoader()->backendPluginByName(filterPluginName));
            if( plugin )
            {
                plugin->showConfigDialog(BackendPlugin::General, "", this);
            }

            break;
        }

        i++;
    }
}

void ConfigBackendsPage::showOptimizations()
{
    QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations(true);

    if( !optimizationList.isEmpty() )
    {
        CodecOptimizations *optimizationsDialog = new CodecOptimizations(optimizationList, this);
        connect(optimizationsDialog, SIGNAL(solutions(const QList<CodecOptimizations::Optimization>&)), config, SLOT(doOptimizations(const QList<CodecOptimizations::Optimization>&)));
        optimizationsDialog->exec();
    }
    else
    {
        QMessageBox::information(this, i18n("Backend optimization"), i18n("All backend settings seem to be optimal, there is nothing to do."));
    }

    formatChanged(ui.prioritiesFormatComboBox->currentText(), true);
}
