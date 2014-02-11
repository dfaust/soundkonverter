
#include "twolamecodecglobal.h"

#include "twolamecodecwidget.h"

#include "../../core/conversionoptions.h"

#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <QSpinBox>
// #include <QSlider>

#include <KLineEdit>


// TODO calibrate quality profiles
TwoLameCodecWidget::TwoLameCodecWidget()
    : CodecWidget(),
    currentFormat( "mp2" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // top box ----------------------------------------

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
    sQuality->setRange( 32, 384 );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sQuality );

    iQuality = new QSpinBox( this );
    iQuality->setRange( 32, 384 );
    iQuality->setSuffix( " kbps" );
    iQuality->setFixedWidth( iQuality->sizeHint().width() );
    connect( iQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( iQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( iQuality );

    topBox->addStretch();

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 1, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments:"), this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 2, 1 );

    modeChanged( 0 );
}

TwoLameCodecWidget::~TwoLameCodecWidget()
{}

// TODO optimize
int TwoLameCodecWidget::bitrateForQuality( int quality )
{
    return 384+(quality-50)*383/100;
}

// TODO optimize
int TwoLameCodecWidget::qualityForBitrate( int bitrate )
{
    return (bitrate-384)*100/384+50;
}

ConversionOptions *TwoLameCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    if( cMode->currentText() == i18n("Quality") )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = iQuality->value();
        options->bitrate = bitrateForQuality( options->quality );
        options->bitrateMode = ConversionOptions::Vbr;
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = iQuality->value();
        options->quality = qualityForBitrate( options->bitrate );
        options->bitrateMode = ConversionOptions::Cbr;
    }
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";

    return options;
}

bool TwoLameCodecWidget::setCurrentConversionOptions( ConversionOptions *options )
{
    if( !options || options->pluginName != global_plugin_name ) return false;

    if( options->qualityMode == ConversionOptions::Quality )
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Quality")) );
        modeChanged( cMode->currentIndex() );
        iQuality->setValue( options->quality );
    }
    else
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Bitrate")) );
        modeChanged( cMode->currentIndex() );
        iQuality->setValue( options->bitrate );
    }
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() ) lCmdArguments->setText( options->cmdArguments );

    return true;
}

void TwoLameCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString TwoLameCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == -25 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == -10 )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 5 )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 20 )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 35 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool TwoLameCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( -25 );
        iQuality->setValue( -25 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( -10 );
        iQuality->setValue( -10 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 20 );
        iQuality->setValue( 20 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 35 );
        iQuality->setValue( 35 );
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

int TwoLameCodecWidget::currentDataRate()
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
            dataRate = 1440000 + iQuality->value()*28800; // TODO calc properly
        }
        else
        {
            dataRate = iQuality->value()/8*60*1000;
        }
    }

    return dataRate;
}

void TwoLameCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( -50, 50 );
        sQuality->setSingleStep( 5 );
        iQuality->setRange( -50, 50 );
        iQuality->setSingleStep( 5 );
        iQuality->setSuffix( "" );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", -50, 50) );
        iQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", -50, 50) );
    }
    else
    {
        sQuality->setRange( 32, 384 );
        sQuality->setSingleStep( 8 );
        iQuality->setRange( 32, 384 );
        iQuality->setSingleStep( 8 );
        iQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        iQuality->setValue( 160 );
        iQuality->setToolTip( i18n("Bitrate") );
        sQuality->setToolTip( "" );
        iQuality->setToolTip( "" );
    }
}

void TwoLameCodecWidget::qualitySliderChanged( int quality )
{
    iQuality->setValue( quality );
}

void TwoLameCodecWidget::qualitySpinBoxChanged( int quality )
{
    sQuality->setValue( quality );
}


