
#include "optionslayer.h"
#include "options.h"
#include "config.h"

#include "optionseditor.h"

#include <QLayout>
#include <QFrame>
#include <QPropertyAnimation>

#include <KLocale>

#include <KPushButton>
#include <KIcon>
#include <KMessageBox>


OptionsLayer::OptionsLayer( Config *config, QWidget *parent )
    : QWidget( parent )
{
    QGridLayout *gridLayout = new QGridLayout( this );
    gridLayout->setContentsMargins( 20, 20, 20, 20 );

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
    frameLayout->setContentsMargins( 3, 3, 3, 3 );
//     frameLayout->setSpacing( 0 );

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), this );
    frameLayout->addWidget( options );


    QHBoxLayout *buttonBox = new QHBoxLayout();
    buttonBox->setContentsMargins( 0, 0, 6, 6 );
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

    aAnimation = new QPropertyAnimation( this, "opacity", this );
    aAnimation->setEasingCurve( QEasingCurve::Linear );
//     aAnimation->setDuration( 5000 );
    connect( aAnimation, SIGNAL(finished()), this, SLOT(animationFinished()) );
    
    rOpacity = 1.0;
}

OptionsLayer::~OptionsLayer()
{}

void OptionsLayer::fadeIn()
{
    show();
    frame->hide();
//     aAnimation->setEasingCurve( QEasingCurve::OutQuad );
    aAnimation->setStartValue( 0.0 );
    aAnimation->setEndValue( 1.0 );
    aAnimation->start();
}

void OptionsLayer::fadeOut()
{
    urls.clear();
    frame->hide();
//     aAnimation->setEasingCurve( QEasingCurve::InQuad );
    aAnimation->setStartValue( 1.0 );
    aAnimation->setEndValue( 0.0 );
    aAnimation->start();
}

void OptionsLayer::animationFinished()
{
    if( qFuzzyCompare(rOpacity,0.0) )
        hide();
    else
        frame->show();
}

qreal OptionsLayer::opacity()
{
    return rOpacity;
}

void OptionsLayer::setOpacity( qreal opacity )
{
    rOpacity = opacity;
    
    QPalette newPalette = palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192 * rOpacity ) );
    setPalette( newPalette );

//     newPalette = frame->palette();
//     newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 230 * rOpacity ) );
//     frame->setPalette( newPalette );
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
    if( options->currentConversionOptions() )
    {
        emit done( urls, options->currentConversionOptions(), command );
        fadeOut();
    }
    else
    {
        KMessageBox::error( this, i18n("No conversion options selected.") );
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

