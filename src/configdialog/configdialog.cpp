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

#include <KLocalizedString>
#include <QIcon>
#include <QPushButton>

ConfigDialog::ConfigDialog( Config *_config, QWidget *parent/*, Page startPage*/ )
    : KPageDialog( parent ),
    config( _config )
{
//     setButtons( QDialog::Help | QDialog::Default | QDialog::Apply | QDialog::Ok | QDialog::Cancel );
//     setWindowTitle( i18n("Settings") );

    generalPageChanged = false;
    advancedlPageChanged = false;
    coverArtPageChanged = false;
    backendsPageChanged = false;
//     button(QDialog::Apply)->setEnabled( false );

    connect( this, SIGNAL(applyClicked()), this, SLOT(applyClicked()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(okClicked()) );
    connect( this, SIGNAL(defaultClicked()), this, SLOT(defaultClicked()) );

    configGeneralPage = new ConfigGeneralPage( config, this );
    generalPage = addPage( (QWidget*)configGeneralPage, i18n("General") );
    generalPage->setHeader( "" );
    generalPage->setIcon( QIcon::fromTheme("configure") );
    connect( configGeneralPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configAdvancedPage = new ConfigAdvancedPage( config, this );
    advancedPage = addPage( (QWidget*)configAdvancedPage, i18n("Advanced") );
    advancedPage->setHeader( "" );
    advancedPage->setIcon( QIcon::fromTheme("preferences-desktop-gaming") );
    connect( configAdvancedPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configCoverArtPage = new ConfigCoverArtPage( config, this );
    coverArtPage = addPage( (QWidget*)configCoverArtPage, i18n("Cover art") );
    coverArtPage->setHeader( "" );
    coverArtPage->setIcon( QIcon::fromTheme("image-x-generic") );
    connect( configCoverArtPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configBackendsPage = new ConfigBackendsPage( config, this );
    backendsPage = addPage( (QWidget*)configBackendsPage, i18n("Backends") );
    backendsPage->setHeader( "" );
    backendsPage->setIcon( QIcon::fromTheme("applications-system") );
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

//     button(QDialog::Apply)->setEnabled( changed );
}

void ConfigDialog::applyClicked()
{
    okClicked();

    generalPageChanged = false;
    advancedlPageChanged = false;
    coverArtPageChanged = false;
    backendsPageChanged = false;
//     button(QDialog::Apply)->setEnabled( false );
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


