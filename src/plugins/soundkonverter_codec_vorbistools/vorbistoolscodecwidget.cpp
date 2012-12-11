
#include "vorbistoolscodecglobal.h"

#include "vorbistoolscodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <KLocale>
#include <KComboBox>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>


VorbisToolsCodecWidget::VorbisToolsCodecWidget()
    : CodecWidget(),
    currentFormat( "ogg vorbis" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

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
    dQuality->setRange( 8, 320 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );

    topBox->addSpacing( 12 );

    QLabel *lBitrateMode = new QLabel( i18n("Bitrate mode:"), this );
    topBox->addWidget( lBitrateMode );
    cBitrateMode = new KComboBox( this );
    cBitrateMode->addItem( i18n("Variable") );
    cBitrateMode->addItem( i18n("Average") );
    cBitrateMode->addItem( i18n("Constant") );
    cBitrateMode->setFixedWidth( cBitrateMode->sizeHint().width() );
    connect( cBitrateMode, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cBitrateMode );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    modeChanged( 0 );
}

VorbisToolsCodecWidget::~VorbisToolsCodecWidget()
{}

// TODO optimize
int VorbisToolsCodecWidget::bitrateForQuality( double quality )
{
    return quality*100/3;
}

// TODO optimize
double VorbisToolsCodecWidget::qualityForBitrate( int bitrate )
{
    return (double)bitrate*3/100;
}

ConversionOptions *VorbisToolsCodecWidget::currentConversionOptions()
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
        options->bitrateMode = ( cBitrateMode->currentText()==i18n("Average") ) ? ConversionOptions::Abr : ConversionOptions::Cbr;
    }

    return options;
}

bool VorbisToolsCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    ConversionOptions *options = _options;

    if( options->qualityMode == ConversionOptions::Quality )
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Quality")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->quality );
        cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Variable")) );
    }
    else
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Bitrate")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->bitrate );
        if( options->bitrateMode == ConversionOptions::Abr ) cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Average")) );
        else cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Constant")) );
    }

    return true;
}

void VorbisToolsCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString VorbisToolsCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 2.0 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 3.0 )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 4.0 )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 5.0 )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 6.0 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool VorbisToolsCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 200 );
        dQuality->setValue( 2.0 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 300 );
        dQuality->setValue( 3.0 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 400 );
        dQuality->setValue( 4.0 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 500 );
        dQuality->setValue( 5.0 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 600 );
        dQuality->setValue( 6.0 );
        cBitrateMode->setCurrentIndex( 0 );
        return true;
    }

    return false;
}

int VorbisToolsCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        if( cMode->currentIndex() == 0 )
        {
            dataRate = 500000 + dQuality->value()*150000;
            if( dQuality->value() > 7 ) dataRate += (dQuality->value()-7)*250000;
            if( dQuality->value() > 9 ) dataRate += (dQuality->value()-9)*800000;
        }
        else
        {
            dataRate = dQuality->value()/8*60*1000;
        }
    }

    return dataRate;
}

void VorbisToolsCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( -100, 1000 );
        sQuality->setSingleStep( 50 );
        dQuality->setRange( -1, 10 );
        dQuality->setSingleStep( 0.01 );
        dQuality->setDecimals( 2 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 400 );
        dQuality->setValue( 4.0 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Variable") );
        cBitrateMode->setEnabled( false );
    }
    else
    {
        sQuality->setRange( 800, 32000 );
        sQuality->setSingleStep( 800 );
        dQuality->setRange( 8, 320 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 16000 );
        dQuality->setValue( 160 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Average") );
        cBitrateMode->addItem( i18n("Constant") );
        cBitrateMode->setEnabled( true );
    }
}

void VorbisToolsCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( double(quality)/100.0 );
}

void VorbisToolsCodecWidget::qualitySpinBoxChanged( double quality )
{
    sQuality->setValue( round(quality*100.0) );
}


