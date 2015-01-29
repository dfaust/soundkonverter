
#include "configgeneralpage.h"

#include "../config.h"

#include <KLocalizedString>

#include <solid/device.h>

ConfigGeneralPage::ConfigGeneralPage(Config *_config, QWidget *parent) :
    ConfigPageBase(parent),
    config(_config)
{
    ui.setupUi(this);

    const int processorsCount = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString()).count();

    ui.startTabComboBox->setCurrentIndex(config->data.general.startTab);
    connect(ui.startTabComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.defaultProfileComboBox->addItem(i18n("Last used"), "last_used");
    ui.defaultProfileComboBox->addItem(i18n("Very low"),  "very_low");
    ui.defaultProfileComboBox->addItem(i18n("Low"),       "low");
    ui.defaultProfileComboBox->addItem(i18n("Medium"),    "medium");
    ui.defaultProfileComboBox->addItem(i18n("High"),      "high");
    ui.defaultProfileComboBox->addItem(i18n("Very high"), "very_high");
    ui.defaultProfileComboBox->addItem(i18n("Lossless"),  "lossless");
    ui.defaultProfileComboBox->addItems(config->customProfiles());
    if( ui.defaultProfileComboBox->findData(config->data.general.defaultProfile) != -1 )
        ui.defaultProfileComboBox->setCurrentIndex(ui.defaultProfileComboBox->findData(config->data.general.defaultProfile));
    connect(ui.defaultProfileComboBox, SIGNAL(activated(int)), this, SLOT(profileChanged()));
    connect(ui.defaultProfileComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));
    profileChanged();

    if( ui.defaultFormatComboBox->findText(config->data.general.defaultFormat) != -1 )
        ui.defaultFormatComboBox->setCurrentIndex(ui.defaultFormatComboBox->findText(config->data.general.defaultFormat));
    connect(ui.defaultFormatComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.conflictHandlingComboBox->addItem(i18n("Generate new file name"), "new_file_name");
    ui.conflictHandlingComboBox->addItem(i18n("Skip file"),              "skip_file");
    connect(ui.conflictHandlingComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.numConversionsSpinBox->setToolTip(i18n("You shouldn't set this number higher than the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processorsCount));
    ui.numConversionsSpinBox->setValue(config->data.general.numFiles);
    connect(ui.numConversionsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()));

    ui.waitForAlbumGainCheckBox->setChecked(config->data.general.waitForAlbumGain);
    connect(ui.waitForAlbumGainCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));

    ui.copyIfSameCodecCheckBox->setChecked(config->data.general.copyIfSameCodec);
    connect(ui.copyIfSameCodecCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));

    ui.replayGainGroupingComboBox->addItem(i18nc("Group files in the Replay Gain tool by", "Album tags and directories"), "album_directory");
    ui.replayGainGroupingComboBox->addItem(i18nc("Group files in the Replay Gain tool by", "Album tags only"),            "album");
    ui.replayGainGroupingComboBox->addItem(i18nc("Group files in the Replay Gain tool by", "Directories only"),           "directory");
    ui.replayGainGroupingComboBox->setCurrentIndex((int)config->data.general.replayGainGrouping);
    connect(ui.replayGainGroupingComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));
    ui.numReplayGainSpinBox->setToolTip(i18n("You shouldn't set this number higher than the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processorsCount));
    ui.numReplayGainSpinBox->setValue(config->data.general.numReplayGainFiles);
    connect(ui.numReplayGainSpinBox, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()));
}

ConfigGeneralPage::~ConfigGeneralPage()
{
}

void ConfigGeneralPage::resetDefaults()
{
    const int processorsCount = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString()).count();

    ui.startTabComboBox->setCurrentIndex(0);
    ui.defaultProfileComboBox->setCurrentIndex(0);
    ui.defaultFormatComboBox->setCurrentIndex(0);
    ui.conflictHandlingComboBox->setCurrentIndex(0);
    ui.numConversionsSpinBox->setValue(processorsCount > 0 ? processorsCount : 1);
    ui.waitForAlbumGainCheckBox->setChecked(true);
    ui.copyIfSameCodecCheckBox->setChecked(false);
    ui.replayGainGroupingComboBox->setCurrentIndex(0);
    ui.numReplayGainSpinBox->setValue(processorsCount > 0 ? processorsCount : 1);

    emit configChanged(true);
}

void ConfigGeneralPage::saveSettings()
{
    config->data.general.startTab = ui.startTabComboBox->currentIndex();
    config->data.general.defaultProfile = ui.defaultProfileComboBox->currentText();
    config->data.general.defaultFormat = ui.defaultFormatComboBox->currentText();
    config->data.general.conflictHandling = (Config::Data::General::ConflictHandling)ui.conflictHandlingComboBox->currentIndex();
    config->data.general.numFiles = ui.numConversionsSpinBox->value();
    config->data.general.waitForAlbumGain = ui.waitForAlbumGainCheckBox->isChecked();
    config->data.general.copyIfSameCodec = ui.copyIfSameCodecCheckBox->isChecked();
    config->data.general.replayGainGrouping = (Config::Data::General::ReplayGainGrouping)ui.replayGainGroupingComboBox->currentIndex();
    config->data.general.numReplayGainFiles = ui.numReplayGainSpinBox->value();
}

void ConfigGeneralPage::somethingChanged()
{
    const bool changed =
        ui.startTabComboBox->currentIndex()             != config->data.general.startTab                ||
        ui.defaultProfileComboBox->currentText()        != config->data.general.defaultProfile          ||
        ui.defaultFormatComboBox->currentText()         != config->data.general.defaultFormat           ||
        ui.conflictHandlingComboBox->currentIndex()     != (int)config->data.general.conflictHandling   ||
        ui.numConversionsSpinBox->value()               != config->data.general.numFiles                ||
        ui.waitForAlbumGainCheckBox->isChecked()        != config->data.general.waitForAlbumGain        ||
        ui.copyIfSameCodecCheckBox->isChecked()         != config->data.general.copyIfSameCodec         ||
        ui.replayGainGroupingComboBox->currentIndex()   != (int)config->data.general.replayGainGrouping ||
        ui.numReplayGainSpinBox->value()                != config->data.general.numReplayGainFiles;

    emit configChanged(changed);
}

void ConfigGeneralPage::profileChanged()
{
    const QString profile = ui.defaultProfileComboBox->currentData().toString();
    const QString lastFormat = !ui.defaultFormatComboBox->currentText().isEmpty() ? ui.defaultFormatComboBox->currentText() : config->data.general.defaultFormat;

    ui.defaultFormatComboBox->clear();

    if( profile == "last_used" )
    {
        ui.defaultFormatComboBox->addItem(i18n("Last used"));
    }
    else if( profile == "very_low" || profile == "low" || profile == "medium" || profile == "high" || profile == "very_high" )
    {
        ui.defaultFormatComboBox->addItem(i18n("Last used"));
        ui.defaultFormatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::Lossy));
    }
    else if( profile == "lossless" )
    {
        ui.defaultFormatComboBox->addItem(i18n("Last used"));
        ui.defaultFormatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::Lossless));
    }
    else
    {
        ConversionOptions *conversionOptions = config->data.profiles.value(profile);
        if( conversionOptions )
            ui.defaultFormatComboBox->addItem(conversionOptions->codecName);
    }

    if( ui.defaultFormatComboBox->findText(lastFormat) != -1 )
        ui.defaultFormatComboBox->setCurrentIndex(ui.defaultFormatComboBox->findText(lastFormat));
}
