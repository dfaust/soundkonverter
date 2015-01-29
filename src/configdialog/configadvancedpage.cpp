
#include "configadvancedpage.h"

#include "../config.h"

#include <KLocalizedString>

#include <QStandardPaths>

ConfigAdvancedPage::ConfigAdvancedPage(Config *_config, QWidget *parent) :
    ConfigPageBase(parent),
    config(_config)
{
    ui.setupUi(this);

    ui.preferredOggVorbisExtensionComboBox->setCurrentIndex(ui.preferredOggVorbisExtensionComboBox->findText(config->data.general.preferredOggVorbisExtension));
    connect(ui.preferredOggVorbisExtensionComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.preferredVorbisCommentCommentTagComboBox->setCurrentIndex(ui.preferredVorbisCommentCommentTagComboBox->findText(config->data.general.preferredVorbisCommentCommentTag));
    connect(ui.preferredVorbisCommentCommentTagComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.preferredVorbisCommentTrackTotalTagComboBox->setCurrentIndex(ui.preferredVorbisCommentTrackTotalTagComboBox->findText(config->data.general.preferredVorbisCommentTrackTotalTag));
    connect(ui.preferredVorbisCommentTrackTotalTagComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.preferredVorbisCommentDiscTotalTagComboBox->setCurrentIndex(ui.preferredVorbisCommentDiscTotalTagComboBox->findText(config->data.general.preferredVorbisCommentDiscTotalTag));
    connect(ui.preferredVorbisCommentDiscTotalTagComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));

    ui.useVfatNamesCheckBox->setChecked(config->data.general.useVFATNames);
    connect(ui.useVfatNamesCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));

    ui.ejectCdAfterRipCheckBox->setChecked(config->data.advanced.ejectCdAfterRip);
    connect(ui.ejectCdAfterRipCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));

    ui.writeLogFilesCheckBox->setToolTip(i18n("Write log files to the hard drive while converting.\nThis can be useful if a crash occurs and you can't access the log file using the log viewer.\nLog files will be written to %1", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/log/"));
    ui.writeLogFilesCheckBox->setChecked(config->data.general.writeLogFiles);
    connect(ui.writeLogFilesCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));

    ui.useSharedMemoryForTempFilesCheckBox->setChecked(config->data.advanced.useSharedMemoryForTempFiles);

    ui.maxSizeForSharedMemoryTempFilesSpinBox->setRange(1, config->data.advanced.sharedMemorySize);
    ui.maxSizeForSharedMemoryTempFilesSpinBox->setValue(config->data.advanced.maxSizeForSharedMemoryTempFiles);

    if( config->data.advanced.sharedMemorySize == 0 )
    {
        ui.useSharedMemoryForTempFilesCheckBox->setEnabled(false);
        ui.useSharedMemoryForTempFilesCheckBox->setChecked(false);
        ui.useSharedMemoryForTempFilesCheckBox->setToolTip(i18n("It seems there's no filesystem mounted on /dev/shm"));
    }

    ui.maxSizeForSharedMemoryTempFilesSpinBox->setEnabled(ui.useSharedMemoryForTempFilesCheckBox->isChecked());
    connect(ui.useSharedMemoryForTempFilesCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));
    connect(ui.useSharedMemoryForTempFilesCheckBox, SIGNAL(toggled(bool)), ui.maxSizeForSharedMemoryTempFilesSpinBox, SLOT(setEnabled(bool)));
    connect(ui.maxSizeForSharedMemoryTempFilesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()));

    ui.usePipesCheckBox->setChecked(config->data.advanced.usePipes);
    connect(ui.usePipesCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()));
}

ConfigAdvancedPage::~ConfigAdvancedPage()
{
}

void ConfigAdvancedPage::resetDefaults()
{
    ui.preferredOggVorbisExtensionComboBox->setCurrentIndex(0);
    ui.preferredVorbisCommentCommentTagComboBox->setCurrentIndex(1);
    ui.preferredVorbisCommentTrackTotalTagComboBox->setCurrentIndex(0);
    ui.preferredVorbisCommentDiscTotalTagComboBox->setCurrentIndex(0);
    ui.useVfatNamesCheckBox->setChecked(false);
    ui.ejectCdAfterRipCheckBox->setChecked(true);
    ui.writeLogFilesCheckBox->setChecked(false);
    ui.useSharedMemoryForTempFilesCheckBox->setChecked(false);
    ui.maxSizeForSharedMemoryTempFilesSpinBox->setValue(config->data.advanced.sharedMemorySize / 4);
    ui.usePipesCheckBox->setChecked(false);

    emit configChanged(true);
}

void ConfigAdvancedPage::saveSettings()
{
    config->data.general.preferredOggVorbisExtension = ui.preferredOggVorbisExtensionComboBox->currentText();
    config->data.general.preferredVorbisCommentCommentTag = ui.preferredVorbisCommentCommentTagComboBox->currentText();
    config->data.general.preferredVorbisCommentTrackTotalTag = ui.preferredVorbisCommentTrackTotalTagComboBox->currentText();
    config->data.general.preferredVorbisCommentDiscTotalTag = ui.preferredVorbisCommentDiscTotalTagComboBox->currentText();
    config->data.general.useVFATNames = ui.useVfatNamesCheckBox->isChecked();
    config->data.advanced.ejectCdAfterRip = ui.ejectCdAfterRipCheckBox->isChecked();
    config->data.general.writeLogFiles = ui.writeLogFilesCheckBox->isChecked();
    config->data.advanced.useSharedMemoryForTempFiles = ui.useSharedMemoryForTempFilesCheckBox->isEnabled() && ui.useSharedMemoryForTempFilesCheckBox->isChecked();
    config->data.advanced.maxSizeForSharedMemoryTempFiles = ui.maxSizeForSharedMemoryTempFilesSpinBox->value();
    config->data.advanced.usePipes = ui.usePipesCheckBox->isChecked();
}

void ConfigAdvancedPage::somethingChanged()
{
    const bool changed =
        ui.preferredOggVorbisExtensionComboBox->currentText()           != config->data.general.preferredOggVorbisExtension             ||
        ui.preferredVorbisCommentCommentTagComboBox->currentText()      != config->data.general.preferredVorbisCommentCommentTag        ||
        ui.preferredVorbisCommentTrackTotalTagComboBox->currentText()   != config->data.general.preferredVorbisCommentTrackTotalTag     ||
        ui.preferredVorbisCommentDiscTotalTagComboBox->currentText()    != config->data.general.preferredVorbisCommentDiscTotalTag      ||
        ui.useVfatNamesCheckBox->isChecked()                            != config->data.general.useVFATNames                            ||
        ui.ejectCdAfterRipCheckBox->isChecked()                         != config->data.advanced.ejectCdAfterRip                        ||
        ui.writeLogFilesCheckBox->isChecked()                           != config->data.general.writeLogFiles                           ||
        ui.useSharedMemoryForTempFilesCheckBox->isChecked()             != config->data.advanced.useSharedMemoryForTempFiles            ||
        ui.maxSizeForSharedMemoryTempFilesSpinBox->value()              != config->data.advanced.maxSizeForSharedMemoryTempFiles        ||
        ui.usePipesCheckBox->isChecked()                                != config->data.advanced.usePipes;

    emit configChanged(changed);
}

