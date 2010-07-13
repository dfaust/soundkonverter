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

#include <klocale.h>
#include <kcombobox.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kfiledialog.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qregexp.h>
#include <solid/device.h>


// ### soundkonverter 0.4: add an option to use vfat save names when the output device is vfat

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
    sDefaultProfile += i18n("Last used");
    sDefaultProfile += i18n("Very low");
    sDefaultProfile += i18n("Low");
    sDefaultProfile += i18n("Medium");
    sDefaultProfile += i18n("High");
    sDefaultProfile += i18n("Very high");
    sDefaultProfile += i18n("Lossless");
    sDefaultProfile += i18n("Hybrid");
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

    QHBoxLayout *useVFATNamesBox = new QHBoxLayout( 0 );
    box->addLayout( useVFATNamesBox );
    cUseVFATNames = new QCheckBox( i18n("Use FAT compatible output file names"), this );
    cUseVFATNames->setToolTip( i18n("Replaces some special characters like \'?\' by \'_\'.") );
    cUseVFATNames->setChecked( config->data.general.useVFATNames );
    useVFATNamesBox->addWidget( cUseVFATNames );
    connect( cUseVFATNames, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *conflictHandlingBox = new QHBoxLayout( 0 );
    box->addLayout( conflictHandlingBox );
    QLabel *lConflictHandling = new QLabel( i18n("Conflict handling")+":", this );
    conflictHandlingBox->addWidget( lConflictHandling );
    cConflictHandling = new KComboBox( this );
    sConflictHandling += i18n("Generate new file name");
    sConflictHandling += i18n("Skip file");
//     sConflictHandling += i18n("Overwrite file");
    cConflictHandling->addItems( sConflictHandling );
    cConflictHandling->setToolTip( i18n("Do that if the output file already exists") );
    cConflictHandling->setCurrentIndex( config->data.general.conflictHandling );
    conflictHandlingBox->addWidget( cConflictHandling );
    connect( cConflictHandling, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *numFilesBox = new QHBoxLayout( 0 );
    box->addLayout( numFilesBox );
    QLabel *lNumFiles = new QLabel( i18n("Number of files to convert at once")+":", this );
    numFilesBox->addWidget( lNumFiles );
    iNumFiles = new KIntSpinBox( 1, 100, 1, 3, this );
    QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
    iNumFiles->setToolTip( i18n("You shouldn't set this number higher then the amount of installed processor cores.\nThere have been %1 processor cores detected.").arg(processors.count()) );
    iNumFiles->setValue( config->data.general.numFiles );
    numFilesBox->addWidget( iNumFiles );
    connect( iNumFiles, SIGNAL(valueChanged(int)), this, SIGNAL(configChanged()) );

    box->addSpacing( 5 );

    QHBoxLayout *updateDelayBox = new QHBoxLayout( 0 );
    box->addLayout( updateDelayBox );
    QLabel* lUpdateDelay = new QLabel( i18n("Status update delay")+":", this );
    updateDelayBox->addWidget( lUpdateDelay );
    iUpdateDelay = new KIntSpinBox( 50, 1000, 50, 100, parent );
    iUpdateDelay->setToolTip( i18n("Update the progress bar in this interval (time in milliseconds)") );
    iUpdateDelay->setSuffix( i18n("ms") );
    iUpdateDelay->setValue( config->data.general.updateDelay );
    updateDelayBox->addWidget( iUpdateDelay );
    connect( iUpdateDelay, SIGNAL(valueChanged(int)), this, SIGNAL(configChanged()) );

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
    cUseVFATNames->setChecked( true );
    cConflictHandling->setCurrentIndex( 0 );
    QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
    iNumFiles->setValue( ( processors.count() > 0 ) ? processors.count() : 1 );
    iUpdateDelay->setValue( 100 );

    emit configChanged( true );
}

void ConfigGeneralPage::saveSettings()
{
    config->data.general.startTab = cStartTab->currentIndex();
    config->data.general.defaultProfile = cDefaultProfile->currentText();
    config->data.general.defaultFormat = cDefaultFormat->currentText();
//     config->data.general.priority = cPriority->currentIndex() * 10; // NOTE that just works for 'normal' and 'low'
    config->data.general.useVFATNames = cUseVFATNames->isChecked();
    config->data.general.conflictHandling = (Config::Data::General::ConflictHandling)cConflictHandling->currentIndex();
    config->data.general.numFiles = iNumFiles->value();
    config->data.general.updateDelay = iUpdateDelay->value();
}

// int ConfigGeneralPage::profileIndex( const QString& string )
// {
//     return sDefaultProfile.indexOf( string );
// }
// 
// int ConfigGeneralPage::formatIndex( const QString& string )
// {
//     return sDefaultFormat.indexOf( string );
// }

// void ConfigGeneralPage::selectDir()
// {
//     QString startDir = lDir->text();
//     int i = startDir.find( QRegExp("%[aAbBcCdDgGnNpPtTyY]{1,1}") );
//     if( i != -1 ) {
//         i = startDir.findRev( "/", i );
//         startDir = startDir.left( i );
//     }
// 
//     QString directory = KFileDialog::getExistingDirectory( startDir, 0, i18n("Choose an output directory") );
//     if( !directory.isEmpty() ) {
//         QString dir = lDir->text();
//         i = dir.find( QRegExp("%[aAbBcCdDgGnNpPtTyY]{1,1}") );
//         if( i != -1 ) {
//             i = dir.findRev( "/", i );
//             lDir->setText( directory + dir.mid(i) );
//         }
//         else {
//             lDir->setText( directory );
//         }
//     }
// }

void ConfigGeneralPage::profileChanged()
{
    QString profile = cDefaultProfile->currentText();
    QString lastFormat = cDefaultFormat->currentText();
    if( lastFormat.isEmpty() ) lastFormat = config->data.general.defaultFormat;

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
    
    if( cDefaultFormat->findText(lastFormat) != -1 ) cDefaultFormat->setCurrentIndex( cDefaultFormat->findText(lastFormat) );
}









