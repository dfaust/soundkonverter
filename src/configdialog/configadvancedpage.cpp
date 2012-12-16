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
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <KComboBox>
#include <KStandardDirs>


ConfigAdvancedPage::ConfigAdvancedPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QFont groupFont;
    groupFont.setBold( true );

    QLabel *lAdvanced = new QLabel( i18n("Advanced"), this );
    lAdvanced->setFont( groupFont );
    box->addWidget( lAdvanced );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *preferredOggVorbisExtensionBox = new QHBoxLayout();
    preferredOggVorbisExtensionBox->addSpacing( ConfigDialogOffset );
    box->addLayout( preferredOggVorbisExtensionBox );
    QLabel* lPreferredOggVorbisExtension = new QLabel( i18n("Preferred file name extension for ogg vorbis files:"), this );
    preferredOggVorbisExtensionBox->addWidget( lPreferredOggVorbisExtension );
    cPreferredOggVorbisExtension = new KComboBox( this );
    cPreferredOggVorbisExtension->addItem( "ogg" );
    cPreferredOggVorbisExtension->addItem( "oga" );
    cPreferredOggVorbisExtension->setCurrentIndex( config->data.general.preferredOggVorbisExtension == "ogg" ? 0 : 1 );
    preferredOggVorbisExtensionBox->addWidget( cPreferredOggVorbisExtension );
    connect( cPreferredOggVorbisExtension, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    preferredOggVorbisExtensionBox->setStretch( 0, 3 );
    preferredOggVorbisExtensionBox->setStretch( 1, 1 );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *preferredVorbisCommentCommentTagBox = new QHBoxLayout();
    preferredVorbisCommentCommentTagBox->addSpacing( ConfigDialogOffset );
    box->addLayout( preferredVorbisCommentCommentTagBox );
    QLabel* lPreferredVorbisCommentCommentTag = new QLabel( i18n("Preferred comment tag field for ogg vorbis and flac files:"), this );
    lPreferredVorbisCommentCommentTag->setToolTip( i18n("Some applications use the field DESCRIPTION even though the field COMMENT is the correct one.\nComments will be written to the selected field, when reading tags the selected field will be preferred.") );
    preferredVorbisCommentCommentTagBox->addWidget( lPreferredVorbisCommentCommentTag );
    cPreferredVorbisCommentCommentTag = new KComboBox( this );
    cPreferredVorbisCommentCommentTag->setToolTip( i18n("Some applications use the field DESCRIPTION even though the field COMMENT is the correct one.\nComments will be written to the selected field, when reading tags the selected field will be preferred.") );
    cPreferredVorbisCommentCommentTag->addItem( "COMMENT" );
    cPreferredVorbisCommentCommentTag->addItem( "DESCRIPTION" );
    cPreferredVorbisCommentCommentTag->setCurrentIndex( config->data.general.preferredVorbisCommentCommentTag == "COMMENT" ? 0 : 1 );
    preferredVorbisCommentCommentTagBox->addWidget( cPreferredVorbisCommentCommentTag );
    connect( cPreferredVorbisCommentCommentTag, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    preferredVorbisCommentCommentTagBox->setStretch( 0, 3 );
    preferredVorbisCommentCommentTagBox->setStretch( 1, 1 );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *preferredVorbisCommentTrackTotalTagBox = new QHBoxLayout();
    preferredVorbisCommentTrackTotalTagBox->addSpacing( ConfigDialogOffset );
    box->addLayout( preferredVorbisCommentTrackTotalTagBox );
    QLabel* lPreferredVorbisCommentTrackTotalTag = new QLabel( i18n("Preferred total tracks number tag field for ogg vorbis and flac files:"), this );
    lPreferredVorbisCommentTrackTotalTag->setToolTip( i18n("Total tracks number will be written to the selected field, when reading tags the selected field will be preferred.\nWhen using the field TRACKNUMBER, the track number and the total tracks number will be written to the same field separated by a slash ('/').") );
    preferredVorbisCommentTrackTotalTagBox->addWidget( lPreferredVorbisCommentTrackTotalTag );
    cPreferredVorbisCommentTrackTotalTag = new KComboBox( this );
    cPreferredVorbisCommentTrackTotalTag->setToolTip( i18n("Total tracks number will be written to the selected field, when reading tags the selected field will be preferred.\nWhen using the field TRACKNUMBER, the track number and the total tracks number will be written to the same field separated by a slash ('/').") );
    cPreferredVorbisCommentTrackTotalTag->addItem( "TRACKTOTAL" );
    cPreferredVorbisCommentTrackTotalTag->addItem( "TOTALTRACKS" );
    cPreferredVorbisCommentTrackTotalTag->addItem( "TRACKNUMBER" );
    cPreferredVorbisCommentTrackTotalTag->setCurrentIndex( cPreferredVorbisCommentTrackTotalTag->findText(config->data.general.preferredVorbisCommentTrackTotalTag) );
    preferredVorbisCommentTrackTotalTagBox->addWidget( cPreferredVorbisCommentTrackTotalTag );
    connect( cPreferredVorbisCommentTrackTotalTag, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    preferredVorbisCommentTrackTotalTagBox->setStretch( 0, 3 );
    preferredVorbisCommentTrackTotalTagBox->setStretch( 1, 1 );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *preferredVorbisCommentDiscTotalTagBox = new QHBoxLayout();
    preferredVorbisCommentDiscTotalTagBox->addSpacing( ConfigDialogOffset );
    box->addLayout( preferredVorbisCommentDiscTotalTagBox );
    QLabel* lPreferredVorbisCommentDiscTotalTag = new QLabel( i18n("Preferred total discs number tag field for ogg vorbis and flac files:"), this );
    lPreferredVorbisCommentDiscTotalTag->setToolTip( i18n("Total discs number will be written to the selected field, when reading tags the selected field will be preferred.\nWhen using the field DISCNUMBER, the disc number and the total discs number will be written to the same field separated by a slash ('/').") );
    preferredVorbisCommentDiscTotalTagBox->addWidget( lPreferredVorbisCommentDiscTotalTag );
    cPreferredVorbisCommentDiscTotalTag = new KComboBox( this );
    cPreferredVorbisCommentDiscTotalTag->setToolTip( i18n("Total discs number will be written to the selected field, when reading tags the selected field will be preferred.\nWhen using the field DISCNUMBER, the disc number and the total discs number will be written to the same field separated by a slash ('/').") );
    cPreferredVorbisCommentDiscTotalTag->addItem( "DISCTOTAL" );
    cPreferredVorbisCommentDiscTotalTag->addItem( "TOTALDISCS" );
    cPreferredVorbisCommentDiscTotalTag->addItem( "DISCNUMBER" );
    cPreferredVorbisCommentDiscTotalTag->setCurrentIndex( cPreferredVorbisCommentDiscTotalTag->findText(config->data.general.preferredVorbisCommentDiscTotalTag) );
    preferredVorbisCommentDiscTotalTagBox->addWidget( cPreferredVorbisCommentDiscTotalTag );
    connect( cPreferredVorbisCommentDiscTotalTag, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    preferredVorbisCommentDiscTotalTagBox->setStretch( 0, 3 );
    preferredVorbisCommentDiscTotalTagBox->setStretch( 1, 1 );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *useVFATNamesBox = new QHBoxLayout();
    useVFATNamesBox->addSpacing( ConfigDialogOffset );
    box->addLayout( useVFATNamesBox );
    cUseVFATNames = new QCheckBox( i18n("Always use FAT compatible output file names"), this );
    cUseVFATNames->setToolTip( i18n("Replaces some special characters like \'?\' by \'_\'.\nIf the output directory is on a FAT file system FAT compatible file names will automatically be used independently from this option.") );
    cUseVFATNames->setChecked( config->data.general.useVFATNames );
    useVFATNamesBox->addWidget( cUseVFATNames );
    connect( cUseVFATNames, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lDebug = new QLabel( i18n("Debug"), this );
    lDebug->setFont( groupFont );
    box->addWidget( lDebug );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *writeLogFilesBox = new QHBoxLayout();
    writeLogFilesBox->addSpacing( ConfigDialogOffset );
    box->addLayout( writeLogFilesBox );
    cWriteLogFiles = new QCheckBox( i18n("Write log files to disc"), this );
    cWriteLogFiles->setToolTip( i18n("Write log files to the hard drive while converting.\nThis can be useful if a crash occurs and you can't access the log file using the log viewer.\nLog files will be written to %1",KStandardDirs::locateLocal("data","soundkonverter/log/")) );
    cWriteLogFiles->setChecked( config->data.general.writeLogFiles );
    writeLogFilesBox->addWidget( cWriteLogFiles );
    connect( cWriteLogFiles, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lExperimental = new QLabel( i18n("Experimental"), this );
    lExperimental->setFont( groupFont );
    box->addWidget( lExperimental );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *useSharedMemoryForTempFilesBox = new QHBoxLayout();
    useSharedMemoryForTempFilesBox->addSpacing( ConfigDialogOffset );
    box->addLayout( useSharedMemoryForTempFilesBox );
    cUseSharedMemoryForTempFiles = new QCheckBox( i18n("Store temporary files in memory unless the estimated size is more than:"), this );
    cUseSharedMemoryForTempFiles->setToolTip( i18n("Don't store files that are expected to be bigger than this value in memory to avoid swapping") );
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

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *usePipesBox = new QHBoxLayout();
    usePipesBox->addSpacing( ConfigDialogOffset );
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
    cPreferredVorbisCommentCommentTag->setCurrentIndex( 1 );
    cPreferredVorbisCommentTrackTotalTag->setCurrentIndex( 0 );
    cPreferredVorbisCommentDiscTotalTag->setCurrentIndex( 0 );
    cUseVFATNames->setChecked( false );
    cWriteLogFiles->setChecked( false );
    cUseSharedMemoryForTempFiles->setChecked( false );
    iMaxSizeForSharedMemoryTempFiles->setValue( config->data.advanced.sharedMemorySize / 4 );
    cUsePipes->setChecked( false );

    emit configChanged( true );
}

void ConfigAdvancedPage::saveSettings()
{
    config->data.general.preferredOggVorbisExtension = cPreferredOggVorbisExtension->currentText();
    config->data.general.preferredVorbisCommentCommentTag = cPreferredVorbisCommentCommentTag->currentText();
    config->data.general.preferredVorbisCommentTrackTotalTag = cPreferredVorbisCommentTrackTotalTag->currentText();
    config->data.general.preferredVorbisCommentDiscTotalTag = cPreferredVorbisCommentDiscTotalTag->currentText();
    config->data.general.useVFATNames = cUseVFATNames->isChecked();
    config->data.general.writeLogFiles = cWriteLogFiles->isChecked();
    config->data.advanced.useSharedMemoryForTempFiles = cUseSharedMemoryForTempFiles->isEnabled() && cUseSharedMemoryForTempFiles->isChecked();
    config->data.advanced.maxSizeForSharedMemoryTempFiles = iMaxSizeForSharedMemoryTempFiles->value();
    config->data.advanced.usePipes = cUsePipes->isChecked();
}

void ConfigAdvancedPage::somethingChanged()
{
    const bool changed = cPreferredOggVorbisExtension->currentText() != config->data.general.preferredOggVorbisExtension ||
                         cPreferredVorbisCommentCommentTag->currentText() != config->data.general.preferredVorbisCommentCommentTag ||
                         cPreferredVorbisCommentTrackTotalTag->currentText() != config->data.general.preferredVorbisCommentTrackTotalTag ||
                         cPreferredVorbisCommentDiscTotalTag->currentText() != config->data.general.preferredVorbisCommentDiscTotalTag ||
                         cUseVFATNames->isChecked() != config->data.general.useVFATNames ||
                         cWriteLogFiles->isChecked() != config->data.general.writeLogFiles ||
                         cUseSharedMemoryForTempFiles->isChecked() != config->data.advanced.useSharedMemoryForTempFiles ||
                         iMaxSizeForSharedMemoryTempFiles->value() != config->data.advanced.maxSizeForSharedMemoryTempFiles ||
                         cUsePipes->isChecked() != config->data.advanced.usePipes;

    emit configChanged( changed );
}

