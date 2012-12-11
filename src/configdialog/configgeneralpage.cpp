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
#include "configgeneralpage.h"

#include "../config.h"

#include <KLocale>
#include <KComboBox>
#include <KLineEdit>
#include <KPushButton>
#include <KFileDialog>
#include <KIntSpinBox>

#include <QLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDir>

#include <solid/device.h>


ConfigGeneralPage::ConfigGeneralPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    const int processorsCount = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString()).count();

    QVBoxLayout *box = new QVBoxLayout( this );

    QFont groupFont;
    groupFont.setBold( true );

    QLabel *lUserInterface = new QLabel( i18n("User interface"), this );
    lUserInterface->setFont( groupFont );
    box->addWidget( lUserInterface );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *startTabBox = new QHBoxLayout();
    startTabBox->addSpacing( ConfigDialogOffset );
    box->addLayout( startTabBox );
    QLabel *lStartTab = new QLabel( i18n("Start in Mode:"), this );
    startTabBox->addWidget( lStartTab );
    cStartTab = new KComboBox( this );
    cStartTab->addItem( i18n("Last used") );
    cStartTab->addItem( i18n("Simple") );
    cStartTab->addItem( i18n("Detailed") );
    cStartTab->setCurrentIndex( config->data.general.startTab );
    startTabBox->addWidget( cStartTab );
    connect( cStartTab, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *defaultProfileBox = new QHBoxLayout();
    defaultProfileBox->addSpacing( ConfigDialogOffset );
    box->addLayout( defaultProfileBox );
    QLabel *lDefaultProfile = new QLabel( i18n("Default profile:"), this );
    defaultProfileBox->addWidget( lDefaultProfile );
    cDefaultProfile = new KComboBox( this );
    QStringList sDefaultProfile;
    sDefaultProfile += i18n("Last used");
    sDefaultProfile += i18n("Very low");
    sDefaultProfile += i18n("Low");
    sDefaultProfile += i18n("Medium");
    sDefaultProfile += i18n("High");
    sDefaultProfile += i18n("Very high");
    sDefaultProfile += i18n("Lossless");
//     sDefaultProfile += i18n("Hybrid"); // currently unused
    sDefaultProfile += config->customProfiles();
    cDefaultProfile->addItems( sDefaultProfile );
    cDefaultProfile->setCurrentIndex( cDefaultProfile->findText(config->data.general.defaultProfile) );
    defaultProfileBox->addWidget( cDefaultProfile );
    connect( cDefaultProfile, SIGNAL(activated(int)), this, SLOT(profileChanged()) );
    connect( cDefaultProfile, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    QLabel *lDefaultFormat = new QLabel( i18n("Default format:"), this );
    defaultProfileBox->addWidget( lDefaultFormat );
    cDefaultFormat = new KComboBox( this );
    cDefaultFormat->setCurrentIndex( cDefaultFormat->findText(config->data.general.defaultFormat) );
    defaultProfileBox->addWidget( cDefaultFormat );
    connect( cDefaultFormat, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    profileChanged();

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lConversion = new QLabel( i18n("Conversion"), this );
    lConversion->setFont( groupFont );
    box->addWidget( lConversion );

    box->addSpacing( ConfigDialogSpacingSmall );

//     QHBoxLayout *priorityBox = new QHBoxLayout();
//     box->addLayout( priorityBox );
//     QLabel *lPriority = new QLabel( i18n("Process priority of the backends:"), this );
//     priorityBox->addWidget( lPriority );
//     cPriority = new KComboBox( this );
//     sPriority += i18n("Normal");
//     sPriority += i18n("Low");
//     cPriority->addItems( sPriority );
//     cPriority->setCurrentIndex( config->data.general.priority / 10 ); // NOTE that just works for 'normal' and 'low'
//     priorityBox->addWidget( cPriority );
//     connect( cPriority, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );
//
//     box->addSpacing( 5 );

    QHBoxLayout *conflictHandlingBox = new QHBoxLayout();
    conflictHandlingBox->addSpacing( ConfigDialogOffset );
    box->addLayout( conflictHandlingBox );
    QLabel *lConflictHandling = new QLabel( i18n("Conflict handling:"), this );
    conflictHandlingBox->addWidget( lConflictHandling );
    cConflictHandling = new KComboBox( this );
    cConflictHandling->addItem( i18n("Generate new file name") );
    cConflictHandling->addItem( i18n("Skip file") );
//     cConflictHandling->addItem( i18n("Overwrite file") );
    cConflictHandling->setToolTip( i18n("Do that if the output file already exists") );
    cConflictHandling->setCurrentIndex( (int)config->data.general.conflictHandling );
    conflictHandlingBox->addWidget( cConflictHandling );
    connect( cConflictHandling, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *numFilesBox = new QHBoxLayout();
    numFilesBox->addSpacing( ConfigDialogOffset );
    box->addLayout( numFilesBox );
    QLabel *lNumFiles = new QLabel( i18n("Number of files to convert at once:"), this );
    numFilesBox->addWidget( lNumFiles );
    iNumFiles = new KIntSpinBox( this );
    iNumFiles->setToolTip( i18n("You shouldn't set this number higher than the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processorsCount) );
    iNumFiles->setRange( 1, 100 );
    iNumFiles->setValue( config->data.general.numFiles );
    numFilesBox->addWidget( iNumFiles );
    connect( iNumFiles, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()) );
    numFilesBox->setStretch( 0, 3 );
    numFilesBox->setStretch( 1, 1 );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *waitForAlbumGainBox = new QHBoxLayout();
    waitForAlbumGainBox->addSpacing( ConfigDialogOffset );
    box->addLayout( waitForAlbumGainBox );
    cWaitForAlbumGain = new QCheckBox( i18n("Apply album gain to converted files"), this );
    cWaitForAlbumGain->setToolTip( i18n("Keep songs of the same album waiting in file list in order to apply album gain to all files.") );
    cWaitForAlbumGain->setChecked( config->data.general.waitForAlbumGain );
    waitForAlbumGainBox->addWidget( cWaitForAlbumGain );
    connect( cWaitForAlbumGain, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lReplayGainTool = new QLabel( i18n("Replay Gain tool"), this );
    lReplayGainTool->setFont( groupFont );
    box->addWidget( lReplayGainTool );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *replayGainGroupingBox = new QHBoxLayout();
    replayGainGroupingBox->addSpacing( ConfigDialogOffset );
    box->addLayout( replayGainGroupingBox );
    QLabel* lReplayGainGrouping = new QLabel( i18n("Group files in the Replay Gain tool by:"), this );
    replayGainGroupingBox->addWidget( lReplayGainGrouping );
    cReplayGainGrouping = new KComboBox( this );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Album tags and directories") );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Album tags only") );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Directories only") );
    cReplayGainGrouping->setCurrentIndex( (int)config->data.general.replayGainGrouping );
    replayGainGroupingBox->addWidget( cReplayGainGrouping );
    connect( cReplayGainGrouping, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *numReplayGainFilesBox = new QHBoxLayout();
    numReplayGainFilesBox->addSpacing( ConfigDialogOffset );
    box->addLayout( numReplayGainFilesBox );
    QLabel *lNumReplayGainFiles = new QLabel( i18n("Number of items to process at once:"), this );
    numReplayGainFilesBox->addWidget( lNumReplayGainFiles );
    iNumReplayGainFiles = new KIntSpinBox( this );
    iNumReplayGainFiles->setToolTip( i18n("You shouldn't set this number higher than the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processorsCount) );
    iNumReplayGainFiles->setRange( 1, 100 );
    iNumReplayGainFiles->setValue( config->data.general.numReplayGainFiles );
    numReplayGainFilesBox->addWidget( iNumReplayGainFiles );
    connect( iNumReplayGainFiles, SIGNAL(valueChanged(int)), this, SLOT(somethingChanged()) );
    numReplayGainFilesBox->setStretch( 0, 3 );
    numReplayGainFilesBox->setStretch( 1, 1 );

    box->addStretch();
}

ConfigGeneralPage::~ConfigGeneralPage()
{}

void ConfigGeneralPage::resetDefaults()
{
    const int processorsCount = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString()).count();

    cStartTab->setCurrentIndex( 0 );
    cDefaultProfile->setCurrentIndex( 0 );
    cDefaultFormat->setCurrentIndex( 0 );
//     cPriority->setCurrentIndex( 1 );
    cConflictHandling->setCurrentIndex( 0 );
    iNumFiles->setValue( processorsCount > 0 ? processorsCount : 1 );
    cWaitForAlbumGain->setChecked( true );
    cReplayGainGrouping->setCurrentIndex( 0 );
    iNumReplayGainFiles->setValue( processorsCount > 0 ? processorsCount : 1 );

    emit configChanged( true );
}

void ConfigGeneralPage::saveSettings()
{
    config->data.general.startTab = cStartTab->currentIndex();
    config->data.general.defaultProfile = cDefaultProfile->currentText();
    config->data.general.defaultFormat = cDefaultFormat->currentText();
//     config->data.general.priority = cPriority->currentIndex() * 10; // NOTE that just works for 'normal' and 'low'
    config->data.general.conflictHandling = (Config::Data::General::ConflictHandling)cConflictHandling->currentIndex();
    config->data.general.numFiles = iNumFiles->value();
    config->data.general.waitForAlbumGain = cWaitForAlbumGain->isChecked();
    config->data.general.replayGainGrouping = (Config::Data::General::ReplayGainGrouping)cReplayGainGrouping->currentIndex();
    config->data.general.numReplayGainFiles = iNumReplayGainFiles->value();
}

void ConfigGeneralPage::somethingChanged()
{
    const bool changed = cStartTab->currentIndex() != config->data.general.startTab ||
                         cDefaultProfile->currentText() != config->data.general.defaultProfile ||
                         cDefaultFormat->currentText() != config->data.general.defaultFormat ||
                         cConflictHandling->currentIndex() != (int)config->data.general.conflictHandling ||
                         iNumFiles->value() != config->data.general.numFiles ||
                         cWaitForAlbumGain->isChecked() != config->data.general.waitForAlbumGain ||
                         cReplayGainGrouping->currentIndex() != (int)config->data.general.replayGainGrouping ||
                         iNumReplayGainFiles->value() != config->data.general.numReplayGainFiles;

    emit configChanged( changed );
}

void ConfigGeneralPage::profileChanged()
{
    const QString profile = cDefaultProfile->currentText();
    QString lastFormat = cDefaultFormat->currentText();
    if( lastFormat.isEmpty() )
        lastFormat = config->data.general.defaultFormat;

    cDefaultFormat->clear();

    if( profile == i18n("Last used") )
    {
        cDefaultFormat->addItem( i18n("Last used") );
    }
    else if( profile == i18n("Very low") || profile == i18n("Low") || profile == i18n("Medium") || profile == i18n("High") || profile == i18n("Very high") )
    {
        cDefaultFormat->addItem( i18n("Last used") );
        cDefaultFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossy) );
    }
    else if( profile == i18n("Lossless") )
    {
        cDefaultFormat->addItem( i18n("Last used") );
        cDefaultFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossless) );
    }
    else if( profile == i18n("Hybrid") )
    {
        cDefaultFormat->addItem( i18n("Last used") );
        cDefaultFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Hybrid) );
    }
    else
    {
        ConversionOptions *conversionOptions = config->data.profiles.value( profile );
        if( conversionOptions )
            cDefaultFormat->addItem( conversionOptions->codecName );
    }

    if( cDefaultFormat->findText(lastFormat) != -1 )
        cDefaultFormat->setCurrentIndex( cDefaultFormat->findText(lastFormat) );
}
