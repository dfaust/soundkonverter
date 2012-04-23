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
#include <QLabel>
#include <QCheckBox>
#include <QDir>

#include <solid/device.h>
#include <solid/storagevolume.h>


ConfigGeneralPage::ConfigGeneralPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QHBoxLayout *startTabBox = new QHBoxLayout( 0 ); // TODO destroy
    box->addLayout( startTabBox );
    QLabel *lStartTab = new QLabel( i18n("Start in Mode")+":", this );
    startTabBox->addWidget( lStartTab );
    cStartTab = new KComboBox( this );
    cStartTab->addItem( i18n("Last used") );
    cStartTab->addItem( i18n("Simple") );
    cStartTab->addItem( i18n("Detailed") );
    cStartTab->setCurrentIndex( config->data.general.startTab );
    startTabBox->addWidget( cStartTab );
    connect( cStartTab, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *defaultProfileBox = new QHBoxLayout( 0 );
    box->addLayout( defaultProfileBox );
    QLabel *lDefaultProfile = new QLabel( i18n("Default profile")+":", this );
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
    connect( cDefaultProfile, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );
    QLabel *lDefaultFormat = new QLabel( i18n("Default format")+":", this );
    defaultProfileBox->addWidget( lDefaultFormat );
    cDefaultFormat = new KComboBox( this );
    cDefaultFormat->setCurrentIndex( cDefaultFormat->findText(config->data.general.defaultFormat) );
    defaultProfileBox->addWidget( cDefaultFormat );
    connect( cDefaultFormat, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );
    profileChanged();

    box->addSpacing( 5 );

//     QHBoxLayout *priorityBox = new QHBoxLayout( 0 );
//     box->addLayout( priorityBox );
//     QLabel *lPriority = new QLabel( i18n("Process priority of the backends")+":", this );
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

    QHBoxLayout *conflictHandlingBox = new QHBoxLayout( 0 );
    box->addLayout( conflictHandlingBox );
    QLabel *lConflictHandling = new QLabel( i18n("Conflict handling")+":", this );
    conflictHandlingBox->addWidget( lConflictHandling );
    cConflictHandling = new KComboBox( this );
    cConflictHandling->addItem( i18n("Generate new file name") );
    cConflictHandling->addItem( i18n("Skip file") );
//     cConflictHandling->addItem( i18n("Overwrite file") );
    cConflictHandling->setToolTip( i18n("Do that if the output file already exists") );
    cConflictHandling->setCurrentIndex( (int)config->data.general.conflictHandling );
    conflictHandlingBox->addWidget( cConflictHandling );
    connect( cConflictHandling, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *numFilesBox = new QHBoxLayout( 0 );
    box->addLayout( numFilesBox );
    QLabel *lNumFiles = new QLabel( i18n("Number of files to convert at once")+":", this );
    numFilesBox->addWidget( lNumFiles );
    iNumFiles = new KIntSpinBox( 1, 100, 1, 3, this );
    QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
    iNumFiles->setToolTip( i18n("You shouldn't set this number higher than the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processors.count()) );
    iNumFiles->setValue( config->data.general.numFiles );
    numFilesBox->addWidget( iNumFiles );
    connect( iNumFiles, SIGNAL(valueChanged(int)), this, SIGNAL(configChanged()) );
    numFilesBox->setStretch( 0, 3 );
    numFilesBox->setStretch( 1, 1 );

    box->addSpacing( 5 );

    QHBoxLayout *waitForAlbumGainBox = new QHBoxLayout( 0 );
    box->addLayout( waitForAlbumGainBox );
    cWaitForAlbumGain = new QCheckBox( i18n("Apply album gain to converted files"), this );
    cWaitForAlbumGain->setToolTip( i18n("Keep songs of the same album waiting in file list in order to apply album gain to all files.") );
    cWaitForAlbumGain->setChecked( config->data.general.waitForAlbumGain );
    waitForAlbumGainBox->addWidget( cWaitForAlbumGain );
    connect( cWaitForAlbumGain, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *createActionsMenuBox = new QHBoxLayout( 0 );
    box->addLayout( createActionsMenuBox );
    cCreateActionsMenu = new QCheckBox( i18n("Create actions menus for the file manager"), this );
    cCreateActionsMenu->setToolTip( i18n("These service menus won't get removed if you uninstall soundKonverter.\nBut you can remove them by diableing this option.") );
    cCreateActionsMenu->setChecked( config->data.general.createActionsMenu );
    createActionsMenuBox->addWidget( cCreateActionsMenu );
    connect( cCreateActionsMenu, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );

    box->addSpacing( 20 );

    QHBoxLayout *replayGainGroupingBox = new QHBoxLayout( 0 );
    box->addLayout( replayGainGroupingBox );
    QLabel* lReplayGainGrouping = new QLabel( i18n("Group files in the Replay Gain tool by")+":", this );
    replayGainGroupingBox->addWidget( lReplayGainGrouping );
    cReplayGainGrouping = new KComboBox( this );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Album tags and directories") );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Album tags only") );
    cReplayGainGrouping->addItem( i18nc("Group files in the Replay Gain tool by","Directories only") );
    cReplayGainGrouping->setCurrentIndex( (int)config->data.general.replayGainGrouping );
    replayGainGroupingBox->addWidget( cReplayGainGrouping );
    connect( cReplayGainGrouping, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );

    box->addStretch();
}

ConfigGeneralPage::~ConfigGeneralPage()
{}

void ConfigGeneralPage::resetDefaults()
{
    cStartTab->setCurrentIndex( 0 );
    cDefaultProfile->setCurrentIndex( 0 );
    cDefaultFormat->setCurrentIndex( 0 );
//     cPriority->setCurrentIndex( 1 );
    cConflictHandling->setCurrentIndex( 0 );
    QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
    iNumFiles->setValue( ( processors.count() > 0 ) ? processors.count() : 1 );
    cWaitForAlbumGain->setChecked( true );
    cCreateActionsMenu->setChecked( true );
    cReplayGainGrouping->setCurrentIndex( 0 );

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
    config->data.general.createActionsMenu = cCreateActionsMenu->isChecked();
    config->data.general.replayGainGrouping = (Config::Data::General::ReplayGainGrouping)cReplayGainGrouping->currentIndex();
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
        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == cDefaultProfile->currentText() )
            {
                cDefaultFormat->addItem( config->data.profiles.at(i).codecName );
            }
        }
    }

    if( cDefaultFormat->findText(lastFormat) != -1 )
        cDefaultFormat->setCurrentIndex( cDefaultFormat->findText(lastFormat) );
}
