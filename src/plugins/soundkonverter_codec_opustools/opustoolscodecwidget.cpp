
#include "opustoolscodecglobal.h"

#include "opustoolscodecwidget.h"
#include "../../core/conversionoptions.h"
#include "opustoolsconversionoptions.h"

#include <math.h>

#include <KLocale>
#include <KComboBox>
#include <QLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>


OpusToolsCodecWidget::OpusToolsCodecWidget()
    : CodecWidget(),
    currentFormat( "opus" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    sQuality = new QSlider( Qt::Horizontal, this );
    sQuality->setRange( 600, 51200 );
    sQuality->setSingleStep( 100 );
    sQuality->setValue( 16000 );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QDoubleSpinBox( this );
    dQuality->setRange( 6, 512 );
    dQuality->setSingleStep( 1 );
    dQuality->setDecimals( 2 );
    dQuality->setSuffix( " kbps" );
    dQuality->setValue( 160 );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );

    topBox->addSpacing( 12 );

    QLabel *lBitrateMode = new QLabel( i18n("Bitrate mode:"), this );
    topBox->addWidget( lBitrateMode );
    cBitrateMode = new KComboBox( this );
    cBitrateMode->addItem( i18n("Average") );
    cBitrateMode->addItem( i18n("Constant") );
    cBitrateMode->setCurrentIndex( 0 );
    cBitrateMode->setFixedWidth( cBitrateMode->sizeHint().width() );
    connect( cBitrateMode, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cBitrateMode );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );
}

OpusToolsCodecWidget::~OpusToolsCodecWidget()
{}

// TODO optimize
int OpusToolsCodecWidget::bitrateForQuality( double quality )
{
    return quality*100/3;
}

// TODO optimize
double OpusToolsCodecWidget::qualityForBitrate( int bitrate )
{
    return (double)bitrate*3/100;
}

ConversionOptions *OpusToolsCodecWidget::currentConversionOptions()
{
    OpusToolsConversionOptions *options = new OpusToolsConversionOptions();

    options->qualityMode = ConversionOptions::Bitrate;
    options->bitrate = (int)dQuality->value();
    options->data.floatBitrate = dQuality->value();
    options->quality = qualityForBitrate( options->bitrate );
    options->bitrateMode = ( cBitrateMode->currentText()==i18n("Average") ) ? ConversionOptions::Abr : ConversionOptions::Cbr;

    return options;
}

bool OpusToolsCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options )
        return false;

    ConversionOptions *options = _options;
    OpusToolsConversionOptions *opusToolsOptions = 0;
    if( options->pluginName == global_plugin_name )
    {
        opusToolsOptions = static_cast<OpusToolsConversionOptions*>(options);
    }

    if( opusToolsOptions )
        dQuality->setValue( opusToolsOptions->data.floatBitrate );
    else
        dQuality->setValue( options->bitrate );

    if( options->bitrateMode == ConversionOptions::Abr )
        cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Average")) );
    else
        cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Constant")) );

    return true;
}

void OpusToolsCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString OpusToolsCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( dQuality->value() == 80 )
    {
        return i18n("Very low");
    }
    else if( dQuality->value() == 128 )
    {
        return i18n("Low");
    }
    else if( dQuality->value() == 192 )
    {
        return i18n("Medium");
    }
    else if( dQuality->value() == 240 )
    {
        return i18n("High");
    }
    else if( dQuality->value() == 320 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool OpusToolsCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        sQuality->setValue( 8000 );
        dQuality->setValue( 80 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        sQuality->setValue( 12800 );
        dQuality->setValue( 128 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        sQuality->setValue( 19200 );
        dQuality->setValue( 192 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("High") )
    {
        sQuality->setValue( 24000 );
        dQuality->setValue( 240 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        sQuality->setValue( 32000 );
        dQuality->setValue( 320 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }

    return false;
}

int OpusToolsCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        dataRate = dQuality->value()/8*60*1000;
    }

    return dataRate;
}

void OpusToolsCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( double(quality)/100.0 );
}

void OpusToolsCodecWidget::qualitySpinBoxChanged( double quality )
{
    sQuality->setValue( round(quality*100.0) );
}


