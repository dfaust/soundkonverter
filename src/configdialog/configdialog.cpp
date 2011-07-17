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
#include "configbackendspage.h"

#include <KLocale>
#include <KIcon>
#include <KPushButton>

ConfigDialog::ConfigDialog( Config *_config, QWidget *parent/*, Page startPage*/ )
    : KPageDialog( parent ),
    config( _config )
{
    setButtons( KDialog::Help | KDialog::Default | KDialog::Apply | KDialog::Ok | KDialog::Cancel );
    setCaption( i18n("Settings") );
    configChanged(false);

    connect( this, SIGNAL(applyClicked()), this, SLOT(applyClicked()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(okClicked()) );
    connect( this, SIGNAL(defaultClicked()), this, SLOT(defaultClicked()) );

    configGeneralPage = new ConfigGeneralPage( config, this );
    generalPage = addPage( (QWidget*)configGeneralPage, i18n("General") );
    generalPage->setIcon( KIcon("configure") );
    connect( configGeneralPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configAdvancedPage = new ConfigAdvancedPage( config, this );
    advancedPage = addPage( (QWidget*)configAdvancedPage, i18n("Advanced") );
    advancedPage->setIcon( KIcon("preferences-desktop-gaming") );
    connect( configAdvancedPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    configBackendsPage = new ConfigBackendsPage( config, this );
    backendsPage = addPage( (QWidget*)configBackendsPage, i18n("Backends") );
    backendsPage->setIcon( KIcon("applications-system") );
    connect( configBackendsPage, SIGNAL(configChanged(bool)), this, SLOT(configChanged(bool)) );

    connect( this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), this, SLOT(pageChanged(KPageWidgetItem*,KPageWidgetItem*)) );

    lastUseVFATNames = config->data.general.useVFATNames;
    lastConflictHandling = (int)config->data.general.conflictHandling;
}

ConfigDialog::~ConfigDialog()
{}

void ConfigDialog::pageChanged( KPageWidgetItem *current, KPageWidgetItem *before )
{
    Q_UNUSED(before)

    button(KDialog::Default)->setEnabled( current != backendsPage );
}

void ConfigDialog::configChanged( bool state )
{
    button(KDialog::Apply)->setEnabled(state);
}

void ConfigDialog::applyClicked()
{
    okClicked();
    configChanged(false);
}

void ConfigDialog::okClicked()
{
    configGeneralPage->saveSettings();
    configAdvancedPage->saveSettings();
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
    else if( currentPage() == backendsPage )
    {
        configBackendsPage->resetDefaults();
    }
}


