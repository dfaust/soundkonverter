//
// C++ Implementation: opener
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "urlopener.h"
#include "../options.h"
#include "../config.h"

#include <KLocale>
#include <KPushButton>
#include <QLabel>
#include <QLayout>
#include <KMessageBox>

#include <KUrlRequester>
#include <QDir>


// TODO enable proceed button only if at least one file got selected // copy'n'paste error ???

// TODO message box if url can't be added -> maybe in file list

UrlOpener::UrlOpener( Config *_config, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    config( _config )
{
    setCaption( i18n("Add url") );
    setWindowIcon( KIcon("network-workgroup") );
    setButtons( 0 );

    page = FileOpenPage;

    QWidget *widget = new QWidget();
    setMainWidget( widget );

    QGridLayout *mainGrid = new QGridLayout( widget );
    QGridLayout *topGrid = new QGridLayout( widget );
    mainGrid->addLayout( topGrid, 0, 0 );

    lSelector = new QLabel( i18n("1. Enter url"), widget );
    QFont font;
    font.setBold( true );
    lSelector->setFont( font );
    topGrid->addWidget( lSelector, 0, 0 );
    lOptions = new QLabel( i18n("2. Set conversion options"), widget );
    topGrid->addWidget( lOptions, 0, 1 );

    // draw a horizontal line
    QFrame *lineFrame = new QFrame( widget );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    mainGrid->addWidget( lineFrame, 1, 0 );

    QVBoxLayout *urlBox = new QVBoxLayout();
    mainGrid->addLayout( urlBox, 2, 0 );
    urlBox->addSpacing( 60 );
    urlRequester = new KUrlRequester( widget );
    urlRequester->setMode( KFile::File | KFile::ExistingOnly );
    urlBox->addWidget( urlRequester );
    urlBox->addStretch();

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    mainGrid->addWidget( options, 2, 0 );
    adjustSize();
    options->hide();


    // add a horizontal box layout for the control elements
    QHBoxLayout *controlBox = new QHBoxLayout();
    mainGrid->addLayout( controlBox, 5, 0 );
    controlBox->addStretch();

    pProceed = new KPushButton( KIcon("go-next"), i18n("Proceed"), widget );
    controlBox->addWidget( pProceed );
    connect( pProceed, SIGNAL(clicked()), this, SLOT(proceedClickedSlot()) );
    pAdd = new KPushButton( KIcon("dialog-ok"), i18n("Ok"), widget );
    controlBox->addWidget( pAdd );
    pAdd->hide();
    connect( pAdd, SIGNAL(clicked()), this, SLOT(okClickedSlot()) );
    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), widget );
    controlBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()) );


        // Prevent the dialog from beeing too wide because of the directory history
    if( parent && width() > parent->width() )
        setInitialSize( QSize(parent->width()-10,sizeHint().height()) );
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "UrlOpener" );
    restoreDialogSize( group );
}

UrlOpener::~UrlOpener()
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "UrlOpener" );
    saveDialogSize( group );
}

void UrlOpener::proceedClickedSlot()
{
    if( page == FileOpenPage )
    {
        if( !urlRequester->url().isValid() )
        {
            KMessageBox::information( this, i18n("The Url you entered is invalid. Please try again.") );
            return;
        }

        urls = urlRequester->url();

        urlRequester->hide();
        options->show();
        page = ConversionOptionsPage;
        QFont font;
        font.setBold( false );
        lSelector->setFont( font );
        font.setBold( true );
        lOptions->setFont( font );
        pProceed->hide();
        pAdd->show();
    }
}

void UrlOpener::okClickedSlot()
{
    if( page == ConversionOptionsPage )
    {
        ConversionOptions *conversionOptions = options->currentConversionOptions();
        if( conversionOptions )
        {
            options->accepted();
            emit open( urls, conversionOptions );
            accept();
        }
        else
        {
            KMessageBox::error( this, i18n("No conversion options selected.") );
        }
    }
}
