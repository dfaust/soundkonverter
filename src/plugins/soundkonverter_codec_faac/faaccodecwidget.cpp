
#include "faaccodecglobal.h"

#include "faaccodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>
#include <KComboBox>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>


FaacCodecWidget::FaacCodecWidget()
    : CodecWidget(),
    currentFormat( "m4a/aac" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lMode = new QLabel( i18n("Mode:"), this );
    topBox->addWidget( lMode );
    cMode = new KComboBox( this );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cMode );

    sQuality = new QSlider( Qt::Horizontal, this );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QSpinBox( this );
    dQuality->setRange( 8, 320 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( dQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    modeChanged( 0 );
}

FaacCodecWidget::~FaacCodecWidget()
{}

// TODO optimize
int FaacCodecWidget::bitrateForQuality( int quality )
{
    return quality*100/3;
}

// TODO optimize
int FaacCodecWidget::qualityForBitrate( int bitrate )
{
    return bitrate*3/100;
}

ConversionOptions *FaacCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();

    if( cMode->currentText()==i18n("Quality") )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = dQuality->value();
        options->bitrate = bitrateForQuality( options->quality );
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = dQuality->value();
        options->quality = qualityForBitrate( options->bitrate );
    }

    return options;
}

bool FaacCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;

    if( options->qualityMode == ConversionOptions::Quality )
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Quality")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->quality );
    }
    else
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Bitrate")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->bitrate );
    }

    return true;
}

void FaacCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString FaacCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 60 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 80  )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 100 )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 140 )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 180 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool FaacCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 60 );
        dQuality->setValue( 60 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 80 );
        dQuality->setValue( 80 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 100 );
        dQuality->setValue( 100 );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 140 );
        dQuality->setValue( 140 );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 180 );
        dQuality->setValue( 180 );
        return true;
    }

    return false;
}

int FaacCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        // TODO calculate data rate for faac
        dataRate = 0;
    }

    return dataRate;
}

void FaacCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 10, 500 );
//         sQuality->setTickInterval( 100 );
        sQuality->setSingleStep( 10 );
        dQuality->setRange( 10, 500 );
        dQuality->setSingleStep( 1 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 100 );
        dQuality->setValue( 100 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 10, 500) );
        dQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 10, 500) );

//         cBitrateMode->clear();
//         cBitrateMode->addItem( i18n("Variable") );
//         cBitrateMode->setEnabled( false );
    }
    else
    {
        sQuality->setRange( 60, 152 ); // max. value 152 kbps/stereo with a 16 kHz cutoff
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 60, 152 );
        dQuality->setSingleStep( 1 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 128 );
        dQuality->setValue( 128 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( "" );
        dQuality->setToolTip( "" );

//         cBitrateMode->clear();
//         cBitrateMode->addItem( i18n("Average") );
//         cBitrateMode->addItem( i18n("Constant") );
//         cBitrateMode->setEnabled( true );
    }
}

void FaacCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( quality );
}

void FaacCodecWidget::qualitySpinBoxChanged( int quality )
{
    sQuality->setValue( quality );
}


