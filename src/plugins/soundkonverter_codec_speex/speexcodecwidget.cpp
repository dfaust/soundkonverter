
#include "speexcodecglobal.h"

#include "speexcodecwidget.h"
#include "../../core/conversionoptions.h"

// #include <math.h>

#include <QLayout>
#include <QLabel>
#include <KLocale>
#include <KComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
// #include <QCheckBox>
#include <QLineEdit>


SpeexCodecWidget::SpeexCodecWidget()
    : CodecWidget(),
    currentFormat( "speex" )
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

    dQuality = new QDoubleSpinBox( this );
    dQuality->setRange( 8, 160 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );

    topBox->addStretch();

    QHBoxLayout *midBox = new QHBoxLayout();
    grid->addLayout( midBox, 1, 0 );

    midBox->addStretch();

    grid->setRowStretch( 2, 1 );

    modeChanged( 0 );
}

SpeexCodecWidget::~SpeexCodecWidget()
{}

// TODO optimize
int SpeexCodecWidget::bitrateForQuality( double quality )
{
    return quality*100/3;
}

// TODO optimize
double SpeexCodecWidget::qualityForBitrate( int bitrate )
{
    return (double)bitrate*3/100;
}

ConversionOptions *SpeexCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();

    if( cMode->currentText()==i18n("Quality") )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = dQuality->value();
        options->bitrate = bitrateForQuality( options->quality );
        options->bitrateMode = ConversionOptions::Vbr;
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = dQuality->value();
        options->quality = qualityForBitrate( options->bitrate );
        options->bitrateMode = ConversionOptions::Abr;
    }

    return options;
}

bool SpeexCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
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

void SpeexCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString SpeexCodecWidget::currentProfile()
{
    return i18n("User defined");
}

bool SpeexCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("User defined");
}

int SpeexCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }

    return dataRate;
}

void SpeexCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 10 );
        sQuality->setSingleStep( 1 );
        dQuality->setRange( 0, 10 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 8 );
        dQuality->setValue( 8 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 0, 10) );
        dQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 0, 10) );
    }
    else
    {
        sQuality->setRange( 8, 160 );
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 8, 160 );
        dQuality->setSingleStep( 8 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 64 );
        dQuality->setValue( 64 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( "" );
        dQuality->setToolTip( "" );
    }
}

void SpeexCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( quality );
}

void SpeexCodecWidget::qualitySpinBoxChanged( double quality )
{
    sQuality->setValue( quality );
}

