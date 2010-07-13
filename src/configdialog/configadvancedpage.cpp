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

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <QCheckBox>
#include <QListView>


ConfigAdvancedPage::ConfigAdvancedPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QGroupBox *coverGroup = new QGroupBox( i18n("CD covers"), this );
//     coverGroup->layout()->setSpacing( 6 );
//     coverGroup->layout()->setMargin( 6 );
    box->addWidget( coverGroup );
    QVBoxLayout *coverBox = new QVBoxLayout( 0 ); // TODO destroy

    cCopyCover = new QCheckBox( i18n("Copy covers from the source directoy to output directoy"), this );
    coverBox->addWidget( cCopyCover );
    connect( cCopyCover, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );

    cEmbedCover = new QCheckBox( i18n("Embed covers from the source directoy into meta tags if possible"), this );
    coverBox->addWidget( cEmbedCover );
    connect( cEmbedCover, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );

    lCoverList = new QListView( this );
    coverBox->addWidget( lCoverList );

    coverGroup->setLayout( coverBox );

    QHBoxLayout *startTabBox = new QHBoxLayout( 0 ); // TODO destroy
    box->addLayout( startTabBox );

    box->addSpacing( 5 );

    QGroupBox *userScriptGroup = new QGroupBox( i18n("User scripts"), this );
//     coverGroup->layout()->setSpacing( 6 );
//     coverGroup->layout()->setMargin( 6 );
    box->addWidget( userScriptGroup );
    QVBoxLayout *userScriptBox = new QVBoxLayout( 0 ); // TODO destroy

    QLabel *lUserScript = new QLabel( i18n("Check all scripts that should be executed after a conversion has finished"), this );
    userScriptBox->addWidget( lUserScript );

    QListView *lUserScriptList = new QListView( this );
    userScriptBox->addWidget( lUserScriptList );

    userScriptGroup->setLayout( userScriptBox );

    box->addStretch();
}

ConfigAdvancedPage::~ConfigAdvancedPage()
{}

void ConfigAdvancedPage::resetDefaults()
{

    emit configChanged( true );
}

void ConfigAdvancedPage::saveSettings()
{
}

