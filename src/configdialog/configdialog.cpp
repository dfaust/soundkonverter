//
// C++ Implementation: configdialog
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configdialog.h"

#include "../config.h"
#include "configgeneralpage.h"
#include "configadvancedpage.h"
#include "configcoverartpage.h"
#include "configbackendspage.h"

#include <KLocale>
#include <KIcon>
#include <KPushButton>

#ifdef SOUNDKONVERTER_KF5_BUILD
#include <QDialogButtonBox>
#endif

ConfigDialog::ConfigDialog( Config *_config, QWidget *parent/*, Page startPage*/ )
    : KPageDialog( parent ),
    config( _config )
{
#ifdef SOUNDKONVERTER_KF5_BUILD
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons( QDialogButtonBox::Help | QDialogButtonBox::Apply | QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    QPushButton *defaultButton = new QPushButton(i18n("Default"));
    buttonBox->addButton(defaultButton, QDialogButtonBox::ActionRole);
    setWindowTitle(i18n("Settings"));
#else
    setButtons( KDialog::Help | KDialog::Default | KDialog::Apply | KDialog::Ok | KDialog::Cancel );
    setCaption( i18n("Settings") );
#endif

    generalPageChanged = false;
    advancedlPageChanged = false;
    coverArtPageChanged = false;
    backendsPageChanged = false;
#ifdef SOUNDKONVERTER_KF5_BUILD
    //FIXME button(QDialogButtonBox::Apply)->setEnabled( false );
#else
    button(KDialog::Apply)->setEnabled( false );
#endif

    connect( this, SIGNAL(applyClicked()), this, SLOT(applyClicked()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(okClicked()) );
    connect( this, SIGNAL(defaultClicked()), this, SLOT(defaultClicked()) );

    configGeneralPage = new ConfigGeneralPage( config, this );
    generalPage = addPage( (QWidget*)configGeneralPage, i18n("General") );
    generalPage->setHeader( "" );
    generalPage->setIcon( KIcon("configure") );
    connect( configGeneralPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configAdvancedPage = new ConfigAdvancedPage( config, this );
    advancedPage = addPage( (QWidget*)configAdvancedPage, i18n("Advanced") );
    advancedPage->setHeader( "" );
    advancedPage->setIcon( KIcon("preferences-desktop-gaming") );
    connect( configAdvancedPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configCoverArtPage = new ConfigCoverArtPage( config, this );
    coverArtPage = addPage( (QWidget*)configCoverArtPage, i18n("Cover art") );
    coverArtPage->setHeader( "" );
    coverArtPage->setIcon( KIcon("image-x-generic") );
    connect( configCoverArtPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configBackendsPage = new ConfigBackendsPage( config, this );
    backendsPage = addPage( (QWidget*)configBackendsPage, i18n("Backends") );
    backendsPage->setHeader( "" );
    backendsPage->setIcon( KIcon("applications-system") );
    connect( configBackendsPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    lastUseVFATNames = config->data.general.useVFATNames;
    lastConflictHandling = (int)config->data.general.conflictHandling;
}

ConfigDialog::~ConfigDialog()
{}

void ConfigDialog::configChanged( bool state )
{
    if( QObject::sender() == configGeneralPage )
    {
        generalPageChanged = state;
    }
    else if( QObject::sender() == configAdvancedPage )
    {
        advancedlPageChanged = state;
    }
    else if( QObject::sender() == configCoverArtPage )
    {
        coverArtPageChanged = state;
    }
    else if( QObject::sender() == configBackendsPage )
    {
        backendsPageChanged = state;
    }

    const bool changed = ( generalPageChanged || advancedlPageChanged || coverArtPageChanged || backendsPageChanged );

#ifdef SOUNDKONVERTER_KF5_BUILD
    button(QDialogButtonBox::Apply)->setEnabled( changed );
#else
    button(KDialog::Apply)->setEnabled( changed );
#endif
}

void ConfigDialog::applyClicked()
{
    okClicked();

    generalPageChanged = false;
    advancedlPageChanged = false;
    coverArtPageChanged = false;
    backendsPageChanged = false;
#ifdef SOUNDKONVERTER_KF5_BUILD
    button(QDialogButtonBox::Apply)->setEnabled( false );
#else
    button(KDialog::Apply)->setEnabled( false );
#endif
}

void ConfigDialog::okClicked()
{
    configGeneralPage->saveSettings();
    configAdvancedPage->saveSettings();
    configCoverArtPage->saveSettings();
    configBackendsPage->saveSettings();
    config->save();

    if( lastUseVFATNames != config->data.general.useVFATNames || lastConflictHandling != (int)config->data.general.conflictHandling )
    {
        emit updateFileList();
    }
}

void ConfigDialog::defaultClicked()
{
    if( currentPage() == generalPage )
    {
        configGeneralPage->resetDefaults();
    }
    else if( currentPage() == advancedPage )
    {
        configAdvancedPage->resetDefaults();
    }
    else if( currentPage() == coverArtPage )
    {
        configCoverArtPage->resetDefaults();
    }
    else if( currentPage() == backendsPage )
    {
        configBackendsPage->resetDefaults();
    }
}


