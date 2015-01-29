
#include "configcoverartpage.h"

#include "../config.h"

#include <KLocalizedString>

ConfigCoverArtPage::ConfigCoverArtPage(Config *_config, QWidget *parent) :
    ConfigPageBase(parent),
    config(_config)
{
    ui.setupUi(this);

    connect(ui.saveCoverButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(writeCoversChanged(QAbstractButton *)));
    connect(ui.saveCoverButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(somethingChanged()));

    if( config->data.coverArt.writeCovers == 0 )
        ui.alwaysSaveToDirRadioButton->setChecked(true);
    else if( config->data.coverArt.writeCovers == 1 )
        ui.onlySaveToDirRadioButton->setChecked(true);
    else if( config->data.coverArt.writeCovers == 2 )
        ui.neverSaveToDirRadioButton->setChecked(true);

    connect(ui.saveToDirFileNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(somethingChanged()));

    ui.saveToDirFileNameLineEdit->setText(config->data.coverArt.writeCoverDefaultName);
}

ConfigCoverArtPage::~ConfigCoverArtPage()
{
}

void ConfigCoverArtPage::resetDefaults()
{
    ui.onlySaveToDirRadioButton->setChecked(true);
    ui.saveToDirFileNameLineEdit->setText(i18nc("cover file name", "cover"));

    emit configChanged(true);
}

void ConfigCoverArtPage::saveSettings()
{
    if( ui.alwaysSaveToDirRadioButton->isChecked() )
        config->data.coverArt.writeCovers = 0;
    else if( ui.onlySaveToDirRadioButton->isChecked() )
        config->data.coverArt.writeCovers = 1;
    else if( ui.neverSaveToDirRadioButton->isChecked() )
        config->data.coverArt.writeCovers = 2;

    config->data.coverArt.writeCoverDefaultName = ui.saveToDirFileNameLineEdit->text();
}

void ConfigCoverArtPage::somethingChanged()
{
    const bool changed =
        ( ui.alwaysSaveToDirRadioButton->isChecked()       && config->data.coverArt.writeCovers    != 0 ) ||
        ( ui.onlySaveToDirRadioButton->isChecked()         && config->data.coverArt.writeCovers    != 1 ) ||
        ( ui.neverSaveToDirRadioButton->isChecked()        && config->data.coverArt.writeCovers    != 2 ) ||
        ui.saveToDirFileNameLineEdit->text() != config->data.coverArt.writeCoverDefaultName;

    emit configChanged(changed);
}

void ConfigCoverArtPage::writeCoversChanged(QAbstractButton *button)
{
    if( button == ui.neverSaveToDirRadioButton )
    {
        ui.saveToDirFileNameLabel->setEnabled(false);
        ui.saveToDirFileNameLineEdit->setEnabled(false);
    }
    else
    {
        ui.saveToDirFileNameLabel->setEnabled(true);
        ui.saveToDirFileNameLineEdit->setEnabled(true);
    }
}
