
#include "optionsdetailed.h"
#include "ui_optionsdetailed.h"

// #include "global.h"
#include "config.h"
#include "core/codecplugin.h"
#include "core/codecwidget.h"
#include "outputdirectory.h"

#include <KLocalizedString>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QStandardPaths>
#include <QMenu>

OptionsDetailed::OptionsDetailed(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::OptionsDetailed)
{
    ui->setupUi(this);

    connect(ui->formatComboBox, SIGNAL(activated(const QString&)), this, SLOT(formatChanged(const QString&)));
//     connect(ui->formatComboBox, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()));

    connect(ui->pluginComboBox, SIGNAL(activated(const QString&)), this, SLOT(encoderChanged(const QString&)));
    connect(ui->pluginComboBox, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()));

    connect(ui->configurePluginButton, SIGNAL(clicked()),          this, SLOT(configurePlugin()));

//     connect( ui->replayGainCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    connect(ui->saveProfileButton, SIGNAL(clicked()),              this, SLOT(saveCustomProfile()));

//     lEstimSize = new QLabel( QString(QChar(8776))+"? B / min." );
//     lEstimSize->hide(); // hide for now because most plugins report inaccurate data
}

OptionsDetailed::~OptionsDetailed()
{
    if( currentPlugin && pluginWidget )
    {
        currentPlugin->deleteCodecWidget(pluginWidget);
    }
}

void OptionsDetailed::init(Config *config)
{
    this->config = config;

    ui->outputDirectory->init(config);

    ui->formatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::CompressionType(PluginLoader::InferiorQuality | PluginLoader::Lossy | PluginLoader::Lossless | PluginLoader::Hybrid)));

    foreach( const QString& pluginName, config->data.backends.enabledFilters )
    {
        FilterPlugin *plugin = qobject_cast<FilterPlugin*>(config->pluginLoader()->backendPluginByName(pluginName));
        if( !plugin )
            continue;

        FilterWidget *widget = plugin->newFilterWidget();
        if( !widget )
            continue;

        filterWidgets.insert(widget, plugin);
        connect(widget, SIGNAL(optionsChanged()), this, SLOT(somethingChanged()));

        ui->filterLayout->addWidget(widget);

        widget->show();
    }

    updateProfiles();

    ui->formatComboBox->setCurrentIndex(0);
    formatChanged(ui->formatComboBox->currentText());
}

void OptionsDetailed::resetFilterOptions()
{
    for( int i=0; i<filterWidgets.size(); i++ )
    {
        FilterWidget *widget = filterWidgets.keys().at(i);
        if( widget )
        {
            widget->setCurrentFilterOptions( 0 );
        }
    }
}

void OptionsDetailed::setReplayGainChecked( bool enabled )
{
    ui->replayGainCheckBox->setChecked(enabled);
}

bool OptionsDetailed::isReplayGainEnabled(QString *toolTip)
{
    if( toolTip )
        *toolTip = ui->replayGainCheckBox->toolTip();

    return ui->replayGainCheckBox->isEnabled();
}

bool OptionsDetailed::isReplayGainChecked()
{
    return ui->replayGainCheckBox->isChecked();
}

CodecPlugin *OptionsDetailed::getCurrentPlugin()
{
    return currentPlugin;
}

void OptionsDetailed::updateProfiles()
{
    QMenu *menu = new QMenu(this);
    const QStringList profiles = config->customProfiles();
    foreach( const QString& profile, profiles )
    {
        menu->addAction(profile, this, SLOT(loadCustomProfileButtonClicked()));
    }

    ui->loadProfileButton->setMenu(menu);
    ui->loadProfileButton->setVisible(!profiles.isEmpty());
}

void OptionsDetailed::formatChanged(const QString& format)
{
    const QString oldEncoder = ui->pluginComboBox->currentText();

    ui->pluginComboBox->clear();
    //if( format != "wav" ) // TODO make it nicer if wav is selected
    foreach( const Config::CodecData& codecData, config->data.backends.codecs )
    {
        if( codecData.codecName == format )
        {
            ui->pluginComboBox->addItems(codecData.encoders);
        }
    }
    ui->pluginComboBox->setCurrentIndex( 0 );

    if( ui->pluginComboBox->currentText() != oldEncoder )
    {
        encoderChanged(ui->pluginComboBox->currentText());
    }
    else if( pluginWidget )
    {
        pluginWidget->setCurrentFormat(ui->formatComboBox->currentText());
    }

    ui->pluginLabel->setVisible(format != "wav");
    ui->pluginComboBox->setVisible(format != "wav");
    ui->configurePluginButton->setVisible(format != "wav");

    if( pluginWidget )
        pluginWidget->setVisible(format != "wav");

    QStringList errorList;
    ui->replayGainCheckBox->setEnabled(config->pluginLoader()->canReplayGain(ui->formatComboBox->currentText(), currentPlugin, &errorList));
    if( !ui->replayGainCheckBox->isEnabled() )
    {
        QPalette notificationPalette = ui->replayGainCheckBox->palette();
        notificationPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(174,127,130));
        ui->replayGainCheckBox->setPalette( notificationPalette );

        if( !errorList.isEmpty() )
        {
            errorList.prepend(i18n("Replay Gain is not supported for the %1 file format.\nPossible solutions are listed below.", ui->formatComboBox->currentText()));
        }
        else
        {
            errorList += i18n("Replay Gain is not supported for the %1 file format.\nPlease check your distribution's package manager in order to install an additional Replay Gain plugin.", ui->formatComboBox->currentText());
        }

        ui->replayGainCheckBox->setToolTip(errorList.join("\n\n"));
    }
    else
    {
        ui->replayGainCheckBox->setToolTip(i18n("Replay Gain tags can tell your music player how loud a track is\nso it can adjust the volume to play all tracks with equal loudness."));
    }

    somethingChanged();
}

void OptionsDetailed::encoderChanged(const QString& encoder)
{
    CodecPlugin *plugin = qobject_cast<CodecPlugin*>(config->pluginLoader()->backendPluginByName(encoder));
    if( !plugin )
    {
//         TODO leads to crashes
//         QMessageBox::critical( this, "soundKonverter", i18n("Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nOptionsDetailed::encoderChanged; PluginLoader::codeui->pluginComboBoxByName returned 0 for encoder: '%1'").arg(encoder), i18n("Internal error") );
        return;
    }

    if( pluginWidget )
    {
        ui->pluginLayout->removeWidget(pluginWidget);
        disconnect(pluginWidget, SIGNAL(optionsChanged()), 0, 0);
        pluginWidget = currentPlugin->deleteCodecWidget(pluginWidget);
    }

    currentPlugin = plugin;
    pluginWidget = plugin->newCodecWidget();

    if( pluginWidget )
    {
        connect(pluginWidget, SIGNAL(optionsChanged()), this, SLOT(somethingChanged()));
        qobject_cast<CodecWidget*>(pluginWidget)->setCurrentFormat(ui->formatComboBox->currentText());
        if( plugin->lastUsedConversionOptions )
        {
            pluginWidget->setCurrentConversionOptions(plugin->lastUsedConversionOptions);
            delete plugin->lastUsedConversionOptions;
            plugin->lastUsedConversionOptions = 0;
        }
        ui->pluginLayout->addWidget(pluginWidget);
    }

    ui->configurePluginButton->setEnabled(plugin->isConfigSupported(BackendPlugin::Encoder, ""));

    if( ui->configurePluginButton->isEnabled() )
        ui->configurePluginButton->setToolTip(i18n("Configure %1 ...", encoder));
    else
        ui->configurePluginButton->setToolTip("");
}

void OptionsDetailed::somethingChanged()
{
    int dataRate = 0;

    if( pluginWidget )
        dataRate = pluginWidget->currentDataRate();

//     if( dataRate > 0 )
//     {
//         const QString dataRateString = Global::prettyNumber(dataRate,"B");
//         lEstimSize->setText( QString(QChar(8776))+" "+dataRateString+" / min." );
//         lEstimSize->setToolTip( i18n("Using the current conversion options will create files with approximately %1 per minute.").arg(dataRateString) );
//     }
//     else
//     {
//         lEstimSize->setText( QString(QChar(8776))+" ? B / min." );
//         lEstimSize->setToolTip( "" );
//     }

    emit currentDataRateChanged( dataRate );
}

void OptionsDetailed::configurePlugin()
{
    CodecPlugin *plugin = qobject_cast<CodecPlugin*>(config->pluginLoader()->backendPluginByName(ui->pluginComboBox->currentText()));

    if( plugin )
    {
        plugin->showConfigDialog( BackendPlugin::Encoder, "", this );
    }
}

ConversionOptions *OptionsDetailed::currentConversionOptions(bool saveLastUsed)
{
    ConversionOptions *options = 0;

    if( pluginWidget && currentPlugin )
    {
        options = pluginWidget->currentConversionOptions();
        if( options )
        {
            options->codecName = ui->formatComboBox->currentText();
            if( options->codecName != "wav" )
                options->pluginName = currentPlugin->name();
            else
                options->pluginName = "";
            options->profile = pluginWidget->currentProfile();
            options->outputDirectoryMode = ui->outputDirectory->mode();
            options->outputDirectory = ui->outputDirectory->directory();
            options->outputFilesystem = ui->outputDirectory->filesystem();
            options->replaygain = ui->replayGainCheckBox->isEnabled() && ui->replayGainCheckBox->isChecked();

            for( int i=0; i<filterWidgets.size(); i++ )
            {
                FilterWidget *widget = filterWidgets.keys().at(i);
                FilterPlugin *plugin = filterWidgets.values().at(i);
                if( widget && plugin )
                {
                    FilterOptions *filterOptions = widget->currentFilterOptions();
                    if( filterOptions )
                    {
                        filterOptions->pluginName = plugin->name();
                        options->filterOptions.append( filterOptions );
                    }
                }
            }

            if( saveLastUsed )
            {
                config->data.general.lastProfile = currentProfile();
                saveCustomProfile( true );
                config->data.general.lastFormat = ui->formatComboBox->currentText();
            }
        }
    }

    return options;
}

bool OptionsDetailed::setCurrentConversionOptions(ConversionOptions *options)
{
    if( !options )
        return false;

    ui->formatComboBox->setCurrentIndex(ui->formatComboBox->findText(options->codecName));
    formatChanged(ui->formatComboBox->currentText());

    if( options->codecName != "wav" )
    {
        ui->pluginComboBox->setCurrentIndex(ui->pluginComboBox->findText(options->pluginName));
        encoderChanged(ui->pluginComboBox->currentText());
    }

    ui->outputDirectory->setMode( (OutputDirectory::Mode)options->outputDirectoryMode );
    ui->outputDirectory->setDirectory( options->outputDirectory );
    ui->replayGainCheckBox->setChecked( options->replaygain );

    bool succeeded = true;

    if( options->codecName == "wav" )
        succeeded = true;
    else if( pluginWidget )
        succeeded = pluginWidget->setCurrentConversionOptions(options);
    else
        succeeded = false;

    QStringList usedFilter;
    foreach( FilterOptions *filterOptions, options->filterOptions )
    {
        bool filterSucceeded = false;
        foreach( FilterWidget *widget, filterWidgets.keys() )
        {
            FilterPlugin *plugin = filterWidgets.value(widget);

            if( widget && plugin && filterOptions->pluginName == plugin->name() )
            {
                filterSucceeded = widget->setCurrentFilterOptions(filterOptions);
                usedFilter.append(filterOptions->pluginName);
                break;
            }
        }
        if( !filterSucceeded )
            succeeded = false;
    }

    // if a filter is disabled, its FilterOptions is 0 thus it won't be added to ConversionOptions, but we need to update the widget so it won't show false data
    foreach( FilterWidget *widget, filterWidgets.keys() )
    {
        FilterPlugin *plugin = filterWidgets.value(widget);

        if( widget && plugin && !usedFilter.contains(plugin->name()) )
        {
            widget->setCurrentFilterOptions(0);
        }
    }

    return succeeded;
}

bool OptionsDetailed::saveCustomProfile(bool lastUsed)
{
    if( pluginWidget && currentPlugin )
    {
        QString profileName;
        if( lastUsed )
        {
            profileName = "soundkonverter_last_used";
        }
        else
        {
            bool ok;
            profileName = QInputDialog::getText(this, i18n("New profile"), i18n("Enter a name for the new profile:"), QLineEdit::Normal, "", &ok);
            if( !ok )
                return false;
        }

        if( profileName.isEmpty() )
        {
            QMessageBox::information( this, i18n("You cannot save a profile without a name."), i18n("Profile name is empty") );
            return false;
        }

        QStringList profiles;
        profiles += i18n("Very low");
        profiles += i18n("Low");
        profiles += i18n("Medium");
        profiles += i18n("High");
        profiles += i18n("Very high");
        profiles += i18n("Lossless");
        profiles += i18n("Hybrid");
        profiles += i18n("Last used");
        profiles += "Last used";
        profiles += i18n("User defined");
        if( !lastUsed )
            profiles += "soundkonverter_last_used";

        if( profiles.contains(profileName) )
        {
            QMessageBox::information( this, i18n("You cannot overwrite the built-in profiles."), i18n("Profile already exists") );
            return false;
        }

        QDomDocument list("soundkonverter_profilelist");
        QDomElement root;
        bool profileFound = false;

        QFile listFile( QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/profiles.xml" );
        if( listFile.open( QIODevice::ReadOnly ) )
        {
            if( list.setContent( &listFile ) )
            {
                root = list.documentElement();
                if( root.nodeName() == "soundkonverter" && root.attribute("type") == "profilelist" )
                {
                    QDomNodeList conversionOptionsElements = root.elementsByTagName("conversionOptions");
                    for( int i=0; i<conversionOptionsElements.count(); i++ )
                    {
                        if( conversionOptionsElements.at(i).toElement().attribute("profileName") == profileName )
                        {
                            int ret;
                            if( lastUsed )
                                ret = QMessageBox::Yes;
                            else
                                ret = QMessageBox::question( this, i18n("Profile already exists"), i18n("A profile with this name already exists.\n\nDo you want to overwrite the existing one?") );

                            if( ret == QMessageBox::Yes )
                            {
                                ConversionOptions *conversionOptions = currentConversionOptions( false );
                                delete config->data.profiles[profileName];
                                config->data.profiles[profileName] = conversionOptions;
                                root.removeChild(conversionOptionsElements.at(i));
                                QDomElement profileElement = conversionOptions->toXml(list);
                                profileElement.setAttribute("profileName",profileName);
                                root.appendChild(profileElement);
                                profileFound = true;
                                break;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }
            }
            listFile.close();
        }

        if( listFile.open( QIODevice::WriteOnly ) )
        {
            if( list.childNodes().isEmpty() )
            {
                root = list.createElement("soundkonverter");
                root.setAttribute("type","profilelist");
                list.appendChild(root);
            }

            if( !profileFound )
            {
                ConversionOptions *conversionOptions = currentConversionOptions( false );
                config->data.profiles[profileName] = conversionOptions;
                QDomElement profileElement = conversionOptions->toXml(list);
                profileElement.setAttribute("profileName",profileName);
                root.appendChild(profileElement);
            }

            updateProfiles();
            emit customProfilesEdited();

            listFile.write(list.toString().toUtf8().data());
            listFile.close();

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void OptionsDetailed::loadCustomProfileButtonClicked()
{
    const QString profile = qobject_cast<QAction*>(QObject::sender())->text().replace("&", "");
    setCurrentProfile( profile );
}

QString OptionsDetailed::currentProfile()
{
    if( pluginWidget )
        return pluginWidget->currentProfile();
    else
        return "";
}

bool OptionsDetailed::setCurrentProfile(const QString& profile)
{
    if( config->data.profiles.keys().contains(profile) )
    {
        ConversionOptions *conversionOptions = config->data.profiles.value(profile);
        if( conversionOptions )
            return setCurrentConversionOptions(conversionOptions);
    }
    else if( pluginWidget )
    {
        return pluginWidget->setCurrentProfile(profile);
    }

    return false;
}

QString OptionsDetailed::currentFormat()
{
    return ui->formatComboBox->currentText();
}

void OptionsDetailed::setCurrentFormat( const QString& format )
{
    if( !format.isEmpty() && format != ui->formatComboBox->currentText() )
    {
        ui->formatComboBox->setCurrentIndex( ui->formatComboBox->findText(format) );
        formatChanged( ui->formatComboBox->currentText() );
    }
}
