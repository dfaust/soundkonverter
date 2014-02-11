
#include "neroaaccodecglobal.h"

#include "neroaaccodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <KLocale>
#include <KComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>


NeroaacCodecWidget::NeroaacCodecWidget()
    : CodecWidget(),
    currentFormat( "m4a/aac" )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

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
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QDoubleSpinBox( this );
    dQuality->setRange( 8, 320 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
//     dQuality->setFixedHeight( cMode->minimumSizeHint().height() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );

    topBox->addSpacing( fontHeight );

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

NeroaacCodecWidget::~NeroaacCodecWidget()
{}

// TODO optimize
int NeroaacCodecWidget::bitrateForQuality( double quality )
{
    return quality*100/3;
}

// TODO optimize
double NeroaacCodecWidget::qualityForBitrate( int bitrate )
{
    return (double)bitrate*3/100;
}

ConversionOptions *NeroaacCodecWidget::currentConversionOptions()
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

bool NeroaacCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
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

void NeroaacCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString NeroaacCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 0.3 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 0.4 )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 0.5 )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 0.6  )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 0.7 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool NeroaacCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 60 );
        dQuality->setValue( 60 );
        cBitrateMode->setCurrentIndex( 0 );
//         chChannels->setChecked( true );
//         chSamplerate->setChecked( true );
//         cSamplerate->setCurrentIndex( 4 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 80 );
        dQuality->setValue( 80 );
        cBitrateMode->setCurrentIndex( 0 );
//         chChannels->setChecked( false );
//         chSamplerate->setChecked( true );
//         cSamplerate->setCurrentIndex( 4 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 100 );
        dQuality->setValue( 100 );
        cBitrateMode->setCurrentIndex( 0 );
//         chChannels->setChecked( false );
//         chSamplerate->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 120 );
        dQuality->setValue( 120 );
        cBitrateMode->setCurrentIndex( 0 );
//         chChannels->setChecked( false );
//         chSamplerate->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 140 );
        dQuality->setValue( 140 );
        cBitrateMode->setCurrentIndex( 0 );
//         chChannels->setChecked( false );
//         chSamplerate->setChecked( false );
        return true;
    }

    return false;
}

int NeroaacCodecWidget::currentDataRate()
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

//         if( chChannels->isChecked() )
//         {
//             dataRate *= 0.9f;
//         }
//         if( chSamplerate->isChecked() && cSamplerate->currentText().replace(" Hz","").toInt() <= 22050 )
//         {
//             dataRate *= 0.9f;
//         }
    }

    return dataRate;
}

void NeroaacCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 100 );
        sQuality->setSingleStep( 5 );
        dQuality->setRange( 0, 1 );
        dQuality->setSingleStep( 0.01 );
        dQuality->setDecimals( 2 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 50 );
        dQuality->setValue( 0.5 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 0, 1) );
        dQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 0, 1) );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Variable") );
        cBitrateMode->setEnabled( false );
    }
    else
    {
        sQuality->setRange( 1600, 40000 );
        sQuality->setSingleStep( 1600 );
        dQuality->setRange( 16, 400 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 16000 );
        dQuality->setValue( 160 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( "" );
        dQuality->setToolTip( "" );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Average") );
        cBitrateMode->addItem( i18n("Constant") );
        cBitrateMode->setEnabled( true );
    }
}

void NeroaacCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( double(quality)/100 );
}

void NeroaacCodecWidget::qualitySpinBoxChanged( double quality )
{
    sQuality->setValue( round(quality*100) );
}

// void NeroaacCodecWidget::channelsToggled( bool enabled )
// {
//     cChannels->setEnabled( enabled );
// }

// void NeroaacCodecWidget::samplerateToggled( bool enabled )
// {
//     cSamplerate->setEnabled( enabled );
// }


