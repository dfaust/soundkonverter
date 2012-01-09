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
#include "configcoverartpage.h"

#include "../config.h"

#include <KLocale>
#include <KIntSpinBox>

#include <QLayout>
#include <QLabel>
#include <QRadioButton>


ConfigCoverArtPage::ConfigCoverArtPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QLabel* lUWriteCovers = new QLabel( i18n("Save embedded covers to output directory"), this );
    box->addWidget( lUWriteCovers );
    rWriteCoversAlways = new QRadioButton( i18n("Always"), this );
    box->addWidget( rWriteCoversAlways );
    rWriteCoversAuto = new QRadioButton( i18n("Only if embedding the covers into the output files is not possible"), this );
    box->addWidget( rWriteCoversAuto );
    rWriteCoversNever = new QRadioButton( i18n("Never"), this );
    box->addWidget( rWriteCoversNever );

    if( config->data.coverArt.writeCovers == 0 )
        rWriteCoversAlways->setChecked( true );
    else if( config->data.coverArt.writeCovers == 1 )
        rWriteCoversAuto->setChecked( true );
    else if( config->data.coverArt.writeCovers == 2 )
        rWriteCoversNever->setChecked( true );


//     QGroupBox *coverGroup = new QGroupBox( i18n("CD covers"), this );
//     box->addWidget( coverGroup );
//     QVBoxLayout *coverBox = new QVBoxLayout( 0 ); // TODO destroy
//
//     cCopyCover = new QCheckBox( i18n("Copy covers from the source directoy to output directoy"), this );
//     coverBox->addWidget( cCopyCover );
//     connect( cCopyCover, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );
//
//     cEmbedCover = new QCheckBox( i18n("Embed covers from the source directoy into meta tags if possible"), this );
//     coverBox->addWidget( cEmbedCover );
//     connect( cEmbedCover, SIGNAL(toggled(bool)), this, SIGNAL(configChanged()) );
//
//     lCoverList = new QListView( this );
//     coverBox->addWidget( lCoverList );
//
//     coverGroup->setLayout( coverBox );
//
//     QHBoxLayout *startTabBox = new QHBoxLayout( 0 ); // TODO destroy
//     box->addLayout( startTabBox );
//
//     box->addSpacing( 5 );
//
//     QGroupBox *userScriptGroup = new QGroupBox( i18n("User scripts"), this );
//     box->addWidget( userScriptGroup );
//     QVBoxLayout *userScriptBox = new QVBoxLayout( 0 ); // TODO destroy
//
//     QLabel *lUserScript = new QLabel( i18n("Check all scripts that should be executed after a conversion has finished"), this );
//     userScriptBox->addWidget( lUserScript );
//
//     QListView *lUserScriptList = new QListView( this );
//     userScriptBox->addWidget( lUserScriptList );
//
//     userScriptGroup->setLayout( userScriptBox );

    box->addStretch();
}

ConfigCoverArtPage::~ConfigCoverArtPage()
{}

void ConfigCoverArtPage::resetDefaults()
{
    rWriteCoversAuto->setChecked( true );

    emit configChanged( true );
}

void ConfigCoverArtPage::saveSettings()
{
    if( rWriteCoversAlways->isChecked() )
        config->data.coverArt.writeCovers = 0;
    else if( rWriteCoversAuto->isChecked() )
        config->data.coverArt.writeCovers = 1;
    else if( rWriteCoversNever->isChecked() )
        config->data.coverArt.writeCovers = 2;
}

