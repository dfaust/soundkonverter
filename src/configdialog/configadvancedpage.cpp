//
// C++ Implementation: configgeneralpage
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configadvancedpage.h"

#include "../config.h"

#include <KLocale>
#include <KIntSpinBox>

#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KComboBox>
#include <KStandardDirs>


ConfigAdvancedPage::ConfigAdvancedPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QHBoxLayout *preferredOggVorbisExtensionBox = new QHBoxLayout( 0 );
    box->addLayout( preferredOggVorbisExtensionBox );
    QLabel* lPreferredOggVorbisExtension = new QLabel( i18n("Preferred extension for ogg vorbis files")+":", this );
    preferredOggVorbisExtensionBox->addWidget( lPreferredOggVorbisExtension );
    cPreferredOggVorbisExtension = new KComboBox( this );
    cPreferredOggVorbisExtension->addItem( "ogg" );
    cPreferredOggVorbisExtension->addItem( "oga" );
    cPreferredOggVorbisExtension->setCurrentIndex( config->data.general.preferredOggVorbisExtension == "ogg" ? 0 : 1 );
    preferredOggVorbisExtensionBox->addWidget( cPreferredOggVorbisExtension );
    connect( cPreferredOggVorbisExtension, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    preferredOggVorbisExtensionBox->setStretch( 0, 3 );
    preferredOggVorbisExtensionBox->setStretch( 1, 1 );

    box->addSpacing( 5 );

    QHBoxLayout *updateDelayBox = new QHBoxLayout( 0 );
    box->addLayout( updateDelayBox );
    QLabel* lUpdateDelay = new QLabel( i18n("Status update delay")+":", this );
    updateDelayBox->addWidget( lUpdateDelay );
    iUpdateDelay = new KIntSpinBox( 50, 1000, 50, 100, this );
    iUpdateDelay->setToolTip( i18n("Update the progress bar in this interval (time in milliseconds)") );
    iUpdateDelay->setSuffix( " " + i18nc("milliseconds","ms") );
    iUpdateDelay->setValue( config->data.general.updateDelay );
    updateDelayBox->addWidget( iUpdateDelay );
    connect( iUpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()) );
    updateDelayBox->setStretch( 0, 3 );
    updateDelayBox->setStretch( 1, 1 );

    box->addSpacing( 5 );

    QHBoxLayout *useVFATNamesBox = new QHBoxLayout( 0 );
    box->addLayout( useVFATNamesBox );
    cUseVFATNames = new QCheckBox( i18n("Always use FAT compatible output file names"), this );
    cUseVFATNames->setToolTip( i18n("Replaces some special characters like \'?\' by \'_\'.\nIf the output directory is on a FAT file system FAT compatible file names will automatically be used independently from this option.") );
    cUseVFATNames->setChecked( config->data.general.useVFATNames );
    useVFATNamesBox->addWidget( cUseVFATNames );
    connect( cUseVFATNames, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( 20 );

    QHBoxLayout *writeLogFilesBox = new QHBoxLayout( 0 );
    box->addLayout( writeLogFilesBox );
    cWriteLogFiles = new QCheckBox( i18n("Write log files to disc"), this );
    cWriteLogFiles->setToolTip( i18n("Write log files to the hard drive while converting.\nThis can be useful if a crash occurs and you can't access the log file using the log viewer.\nLog files will be written to %1",KStandardDirs::locateLocal("data","soundkonverter/log/")) );
    cWriteLogFiles->setChecked( config->data.general.writeLogFiles );
    writeLogFilesBox->addWidget( cWriteLogFiles );
    connect( cWriteLogFiles, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *removeFailedFilesBox = new QHBoxLayout( 0 );
    box->addLayout( removeFailedFilesBox );
    cRemoveFailedFiles = new QCheckBox( i18n("Remove partially converted files if a conversion fails"), this );
    cRemoveFailedFiles->setToolTip( i18n("Disable this for debugging or if you are sure the files get converted correctly.") );
    cRemoveFailedFiles->setChecked( config->data.general.removeFailedFiles );
    removeFailedFilesBox->addWidget( cRemoveFailedFiles );
    connect( cRemoveFailedFiles, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( 20 );

    QHBoxLayout *useSharedMemoryForTempFilesBox = new QHBoxLayout( 0 );
    box->addLayout( useSharedMemoryForTempFilesBox );
    cUseSharedMemoryForTempFiles = new QCheckBox( i18n("Store temporary files in memory unless the estimated size is more than")+":", this );
    cUseSharedMemoryForTempFiles->setChecked( config->data.advanced.useSharedMemoryForTempFiles );
    useSharedMemoryForTempFilesBox->addWidget( cUseSharedMemoryForTempFiles );
    iMaxSizeForSharedMemoryTempFiles = new KIntSpinBox( 1, config->data.advanced.sharedMemorySize, 1, config->data.advanced.sharedMemorySize / 2, this );
    iMaxSizeForSharedMemoryTempFiles->setToolTip( i18n("Don't store files that are expected to be bigger than this value in memory to avoid swapping") );
    iMaxSizeForSharedMemoryTempFiles->setSuffix( " " + i18nc("mega in bytes","MiB") );
    iMaxSizeForSharedMemoryTempFiles->setValue( config->data.advanced.maxSizeForSharedMemoryTempFiles );
    useSharedMemoryForTempFilesBox->addWidget( iMaxSizeForSharedMemoryTempFiles );
    if( config->data.advanced.sharedMemorySize == 0 )
    {
        cUseSharedMemoryForTempFiles->setEnabled( false );
        cUseSharedMemoryForTempFiles->setChecked( false );
        cUseSharedMemoryForTempFiles->setToolTip( i18n("It seems there's no filesystem mounted on /dev/shm") );
    }
    iMaxSizeForSharedMemoryTempFiles->setEnabled( cUseSharedMemoryForTempFiles->isChecked() );
    connect( cUseSharedMemoryForTempFiles, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    connect( cUseSharedMemoryForTempFiles, SIGNAL(toggled(bool)), iMaxSizeForSharedMemoryTempFiles, SLOT(setEnabled(bool)) );
    connect( iMaxSizeForSharedMemoryTempFiles, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()) );
    useSharedMemoryForTempFilesBox->setStretch( 0, 3 );
    useSharedMemoryForTempFilesBox->setStretch( 1, 1 );

    box->addSpacing( 5 );

    QHBoxLayout *usePipesBox = new QHBoxLayout( 0 );
    box->addLayout( usePipesBox );
    cUsePipes = new QCheckBox( i18n("Use pipes when possible"), this );
    cUsePipes->setToolTip( i18n("Pipes make it unnecessary to use temporary files, therefore increasing the performance.\nBut some backends cause errors in this mode so be cautious.") );
    cUsePipes->setChecked( config->data.advanced.usePipes );
    usePipesBox->addWidget( cUsePipes );
    connect( cUsePipes, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addStretch();
}

ConfigAdvancedPage::~ConfigAdvancedPage()
{}

void ConfigAdvancedPage::resetDefaults()
{
    cPreferredOggVorbisExtension->setCurrentIndex( 0 );
    iUpdateDelay->setValue( 100 );
    cUseVFATNames->setChecked( false );
    cWriteLogFiles->setChecked( false );
    cRemoveFailedFiles->setChecked( true );
    cUseSharedMemoryForTempFiles->setChecked( false );
    iMaxSizeForSharedMemoryTempFiles->setValue( config->data.advanced.sharedMemorySize / 2 );
    cUsePipes->setChecked( false );

    emit configChanged( true );
}

void ConfigAdvancedPage::saveSettings()
{
    config->data.general.preferredOggVorbisExtension = cPreferredOggVorbisExtension->currentText();
    config->data.general.updateDelay = iUpdateDelay->value();
    config->data.general.useVFATNames = cUseVFATNames->isChecked();
    config->data.general.writeLogFiles = cWriteLogFiles->isChecked();
    config->data.general.removeFailedFiles = cRemoveFailedFiles->isChecked();
    config->data.advanced.useSharedMemoryForTempFiles = cUseSharedMemoryForTempFiles->isEnabled() && cUseSharedMemoryForTempFiles->isChecked();
    config->data.advanced.maxSizeForSharedMemoryTempFiles = iMaxSizeForSharedMemoryTempFiles->value();
    config->data.advanced.usePipes = cUsePipes->isChecked();
}

void ConfigAdvancedPage::somethingChanged()
{
    const bool changed = cPreferredOggVorbisExtension->currentText() != config->data.general.preferredOggVorbisExtension ||
                         iUpdateDelay->value() != config->data.general.updateDelay ||
                         cUseVFATNames->isChecked() != config->data.general.useVFATNames ||
                         cWriteLogFiles->isChecked() != config->data.general.writeLogFiles ||
                         cRemoveFailedFiles->isChecked() != config->data.general.removeFailedFiles ||
                         cUseSharedMemoryForTempFiles->isChecked() != config->data.advanced.useSharedMemoryForTempFiles ||
                         iMaxSizeForSharedMemoryTempFiles->value() != config->data.advanced.maxSizeForSharedMemoryTempFiles ||
                         cUsePipes->isChecked() != config->data.advanced.usePipes;

    emit configChanged( changed );
}

