
#include "configcoverartpage.h"

#include "../config.h"

#include <KLocale>
#include <KLineEdit>

#include <QLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>


ConfigCoverArtPage::ConfigCoverArtPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QFont groupFont;
    groupFont.setBold( true );

    QLabel *lCoverSaving = new QLabel( i18n("Saving covers to output directory"), this );
    lCoverSaving->setFont( groupFont );
    box->addWidget( lCoverSaving );

    box->addSpacing( spacingSmall );

    QHBoxLayout *coverSavingBox = new QHBoxLayout();
    coverSavingBox->addSpacing( spacingOffset );
    box->addLayout( coverSavingBox );
    QVBoxLayout *writeCoversBox = new QVBoxLayout();
    coverSavingBox->addLayout( writeCoversBox );

    QLabel *lWriteCovers = new QLabel( i18n("Save embedded covers to output directory"), this );
    writeCoversBox->addWidget( lWriteCovers );
    rWriteCoversAlways = new QRadioButton( i18n("Always"), this );
    writeCoversBox->addWidget( rWriteCoversAlways );
    rWriteCoversAuto = new QRadioButton( i18n("Only if embedding the covers into the output files is not possible"), this );
    writeCoversBox->addWidget( rWriteCoversAuto );
    rWriteCoversNever = new QRadioButton( i18n("Never"), this );
    writeCoversBox->addWidget( rWriteCoversNever );

    QButtonGroup *writeCoversGroup = new QButtonGroup( this );
    writeCoversGroup->addButton( rWriteCoversAlways );
    writeCoversGroup->addButton( rWriteCoversAuto );
    writeCoversGroup->addButton( rWriteCoversNever );

    connect( writeCoversGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(writeCoversChanged(QAbstractButton*)) );
    connect( writeCoversGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(somethingChanged()) );

    if( config->data.coverArt.writeCovers == 0 )
        rWriteCoversAlways->setChecked( true );
    else if( config->data.coverArt.writeCovers == 1 )
        rWriteCoversAuto->setChecked( true );
    else if( config->data.coverArt.writeCovers == 2 )
        rWriteCoversNever->setChecked( true );

    writeCoversBox->addSpacing( spacingMedium );

    lWriteCoverName = new QLabel( i18n("File name for the cover (when saving to output directory)"), this );
    writeCoversBox->addWidget( lWriteCoverName );
    rWriteCoverNameTitle = new QRadioButton( i18n("The embedded cover description"), this );
    writeCoversBox->addWidget( rWriteCoverNameTitle );
    rWriteCoverNameDefault = new QRadioButton( i18n("The default cover file name"), this );
    writeCoversBox->addWidget( rWriteCoverNameDefault );

    QHBoxLayout *writeCoverNameTextBox = new QHBoxLayout( this );
    lWriteCoverNameDefaultLabel = new QLabel( i18n("Default cover file name:"), this );
    writeCoverNameTextBox->addWidget( lWriteCoverNameDefaultLabel );
    lWriteCoverNameDefaultEdit = new KLineEdit( this );
    lWriteCoverNameDefaultEdit->setToolTip( i18n("The following strings are wildcards that will be replaced\nby the information in the meta data:\n\n%a - Artist\n%b - Album\n%d - Cover description\n%r - Cover role") );
    writeCoverNameTextBox->addWidget( lWriteCoverNameDefaultEdit );
    writeCoverNameTextBox->addStretch();
    writeCoversBox->addLayout( writeCoverNameTextBox );
    connect( lWriteCoverNameDefaultEdit, SIGNAL(textChanged(QString)), this, SLOT(somethingChanged()) );

    QButtonGroup *writeCoverNameGroup = new QButtonGroup( this );
    writeCoverNameGroup->addButton( rWriteCoverNameTitle );
    writeCoverNameGroup->addButton( rWriteCoverNameDefault );

    connect( writeCoverNameGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(somethingChanged()) );

    if( config->data.coverArt.writeCoverName == 0 )
        rWriteCoverNameTitle->setChecked( true );
    else if( config->data.coverArt.writeCoverName == 1 )
        rWriteCoverNameDefault->setChecked( true );
    lWriteCoverNameDefaultEdit->setText( config->data.coverArt.writeCoverDefaultName );

//     QGroupBox *coverGroup = new QGroupBox( i18n("CD covers"), this );
//     writeCoversBox->addWidget( coverGroup );
//     QVBoxLayout *coverBox = new QVBoxLayout();
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
//     QHBoxLayout *startTabBox = new QHBoxLayout();
//     writeCoversBox->addLayout( startTabBox );
//
//     writeCoversBox->addSpacing( 5 );
//
//     QGroupBox *userScriptGroup = new QGroupBox( i18n("User scripts"), this );
//     writeCoversBox->addWidget( userScriptGroup );
//     QVBoxLayout *userScriptBox = new QVBoxLayout();
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
    rWriteCoverNameTitle->setChecked( true );
    lWriteCoverNameDefaultEdit->setText( i18nc("cover file name","cover") );

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

    if( rWriteCoverNameTitle->isChecked() )
        config->data.coverArt.writeCoverName = 0;
    else if( rWriteCoverNameDefault->isChecked() )
        config->data.coverArt.writeCoverName = 1;

    config->data.coverArt.writeCoverDefaultName = lWriteCoverNameDefaultEdit->text();
}

void ConfigCoverArtPage::somethingChanged()
{
    const bool changed = ( rWriteCoversAlways->isChecked() && config->data.coverArt.writeCovers != 0 ) ||
                         ( rWriteCoversAuto->isChecked() && config->data.coverArt.writeCovers != 1 ) ||
                         ( rWriteCoversNever->isChecked() && config->data.coverArt.writeCovers != 2 ) ||
                         ( rWriteCoverNameTitle->isChecked() && config->data.coverArt.writeCoverName != 0 ) ||
                         ( rWriteCoverNameDefault->isChecked() && config->data.coverArt.writeCoverName != 1 ) ||
                         lWriteCoverNameDefaultEdit->text() != config->data.coverArt.writeCoverDefaultName;

    emit configChanged( changed );
}

void ConfigCoverArtPage::writeCoversChanged( QAbstractButton *button )
{
    if( button == rWriteCoversNever )
    {
        lWriteCoverName->setEnabled( false );
        rWriteCoverNameTitle->setEnabled( false );
        rWriteCoverNameDefault->setEnabled( false );
        lWriteCoverNameDefaultLabel->setEnabled( false );
        lWriteCoverNameDefaultEdit->setEnabled( false );
    }
    else
    {
        lWriteCoverName->setEnabled( true );
        rWriteCoverNameTitle->setEnabled( true );
        rWriteCoverNameDefault->setEnabled( true );
        lWriteCoverNameDefaultLabel->setEnabled( true );
        lWriteCoverNameDefaultEdit->setEnabled( true );
    }
}

