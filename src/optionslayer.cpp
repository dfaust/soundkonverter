
#include "optionslayer.h"
#include "options.h"
#include "config.h"

#include <QApplication>
#include <QLayout>
#include <QFrame>

#include <KLocale>

#include <KPushButton>
#include <KIcon>
#include <KMessageBox>
#include <KApplication>


OptionsLayer::OptionsLayer( Config *config, QWidget *parent )
    : QWidget( parent )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QGridLayout *gridLayout = new QGridLayout( this );
    gridLayout->setContentsMargins( 2*fontHeight, 2*fontHeight, 2*fontHeight, 2*fontHeight );

    frame = new QFrame( this );
    gridLayout->addWidget( frame, 0, 0 );
    frame->setFrameShape( QFrame::StyledPanel );
    frame->setFrameShadow( QFrame::Raised );
    frame->setAutoFillBackground( true );
    QPalette palette = frame->palette();
    QBrush brush = palette.window();
    QBrush oldBrush = brush;
//     brush.setColor( QColor(230,236,238) ); // 223,230,231
//     brush.setColor( QColor(223,230,231) );
    palette.setBrush( QPalette::Window, brush );
    frame->setPalette( palette );


    QVBoxLayout *frameLayout = new QVBoxLayout( frame );

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), this );
    frameLayout->addWidget( options );


    QHBoxLayout *buttonBox = new QHBoxLayout();
    frameLayout->addLayout( buttonBox );
    buttonBox->addStretch();
    pOk = new KPushButton( KIcon("dialog-ok"), i18n("Ok"), this );
    buttonBox->addWidget( pOk );
    connect( pOk, SIGNAL(clicked()), this, SLOT(ok()) );
    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), this );
    buttonBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(abort()) );

    palette = options->palette();
    brush = palette.window();
    palette.setBrush( QPalette::Window, brush );
    options->setPalette( palette );

    setAutoFillBackground( true );

    connect( &fadeTimer, SIGNAL(timeout()), this, SLOT(fadeAnim()) );
    fadeAlpha = 0.0f;
}

OptionsLayer::~OptionsLayer()
{}

void OptionsLayer::fadeIn()
{
    pOk->setDisabled( false );
    pCancel->setDisabled( false );

    fadeTimer.start( 50 );
    fadeMode = 1;
    QPalette newPalette = palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 0 ) );
    setPalette( newPalette );
    newPalette = frame->palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 0 ) );
    frame->setPalette( newPalette );
    frame->hide();
    show();
}

void OptionsLayer::fadeOut()
{
    urls.clear();

    fadeTimer.start( 50 );
    fadeMode = 2;
    frame->hide();
}

void OptionsLayer::fadeAnim()
{
    if( fadeMode == 1 ) fadeAlpha += 255.0f/50.0f*8.0f;
    else if( fadeMode == 2 ) fadeAlpha -= 255.0f/50.0f*8.0f;

    if( fadeAlpha <= 0.0f ) { fadeAlpha = 0.0f; fadeMode = 0; hide(); }
    else if( fadeAlpha >= 255.0f ) { fadeAlpha = 255.0f; fadeMode = 0; frame->show(); }
    else { fadeTimer.start( 50 ); }

    QPalette newPalette = palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192.0f/255.0f*fadeAlpha ) );
    setPalette( newPalette );

    newPalette = frame->palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 230.0f/255.0f*fadeAlpha ) );
    frame->setPalette( newPalette );
}

void OptionsLayer::addUrls( const KUrl::List& _urls )
{
    urls += _urls;
}

void OptionsLayer::abort()
{
    fadeOut();
}

void OptionsLayer::ok()
{
    ConversionOptions *conversionOptions = options->currentConversionOptions();
    if( conversionOptions )
    {
        options->accepted();
        pOk->setDisabled( true );
        pCancel->setDisabled( true );
        kapp->processEvents();
        emit done( urls, conversionOptions, command );
        emit saveFileList();
        fadeOut();
    }
    else
    {
        KMessageBox::error( this, i18n("No conversion options selected.") ); // possibly unneeded i18n string
    }
}

void OptionsLayer::setProfile( const QString& profile )
{
    options->setProfile( profile );
}

void OptionsLayer::setFormat( const QString& format )
{
    options->setFormat( format );
}

void OptionsLayer::setOutputDirectory( const QString& directory )
{
    options->setOutputDirectory( directory );
}

void OptionsLayer::setCommand( const QString& _command )
{
    command = _command;
}

void OptionsLayer::setCurrentConversionOptions( ConversionOptions *conversionOptions )
{
    options->setCurrentConversionOptions( conversionOptions );
}

