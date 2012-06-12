
#include "soxfilterglobal.h"

#include "soxcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <QLayout>
#include <QLabel>
#include <KLocale>
#include <QSpinBox>
#include <QSlider>
#include <KComboBox>


SoxCodecWidget::SoxCodecWidget()
    : CodecWidget(),
    currentFormat( "flac" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );


    // flac

    lCompressionLevel = new QLabel( i18n("Compression level")+":", this );
    topBox->addWidget( lCompressionLevel );

    sCompressionLevel = new QSlider( Qt::Horizontal, this );
    sCompressionLevel->setRange( 0, 8 );
    sCompressionLevel->setSingleStep( 1 );
    sCompressionLevel->setPageStep( 1 );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sCompressionLevel );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 0, 8 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( iCompressionLevel );

    iCompressionLevel->setValue( 5 );


    // mp3 and ogg vorbis

    lMode = new QLabel( i18n("Mode")+":", this );
    topBox->addWidget( lMode );
    cMode = new KComboBox( this );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cMode );

    lQuality = new QLabel( i18n("Quality")+":", this );
    topBox->addWidget( lQuality );

    sQuality = new QSlider( Qt::Horizontal, this );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QDoubleSpinBox( this );
    dQuality->setRange( 8, 320 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(somethingChanged()) );
    topBox->addWidget( dQuality );


    // amr nb and amr wb

    lBitratePreset = new QLabel( i18n("Bitrate")+":", this );
    topBox->addWidget( lBitratePreset );
    cBitratePreset = new KComboBox( this );
    cBitratePreset->addItem( "00.00 kbps" );
    topBox->addWidget( cBitratePreset );


    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    modeChanged( 0 );
}

SoxCodecWidget::~SoxCodecWidget()
{}

ConversionOptions *SoxCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    options->compressionLevel = iCompressionLevel->value();
    return options;
}

bool SoxCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;
    iCompressionLevel->setValue( options->compressionLevel );
    return true;
}

void SoxCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );

    if( currentFormat == "flac" )
    {
        lCompressionLevel->show();
        sCompressionLevel->show();
        iCompressionLevel->show();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "mp3" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->show();
        cMode->show();
        lQuality->hide();
        sQuality->show();
        dQuality->show();
        modeChanged( 0 );

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "ogg vorbis" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->show();
        sQuality->show();
        dQuality->show();
        sQuality->setRange( -100, 1000 );
        sQuality->setSingleStep( 50 );
        dQuality->setRange( -1, 10 );
        dQuality->setSingleStep( 0.01 );
        dQuality->setDecimals( 2 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 400 );
        dQuality->setValue( 4.0 );

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "amr nb" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->show();
        cBitratePreset->show();
        cBitratePreset->clear();
        cBitratePreset->addItem( "4.75 kbps", 0 );
        cBitratePreset->addItem( "5.15 kbps", 1 );
        cBitratePreset->addItem( "5.9 kbps", 2 );
        cBitratePreset->addItem( "6.7 kbps", 3 );
        cBitratePreset->addItem( "7.4 kbps", 4 );
        cBitratePreset->addItem( "7.95 kbps", 5 );
        cBitratePreset->addItem( "10.2 kbps", 6 );
        cBitratePreset->addItem( "12.2 kbps", 7 );
    }
    else if( currentFormat == "amr wb" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->show();
        cBitratePreset->show();
        cBitratePreset->clear();
        cBitratePreset->addItem( "6.6 kbps", 0 );
        cBitratePreset->addItem( "8.85 kbps", 1 );
        cBitratePreset->addItem( "12.65 kbps", 2 );
        cBitratePreset->addItem( "14.25 kbps", 3 );
        cBitratePreset->addItem( "15.85 kbps", 4 );
        cBitratePreset->addItem( "18.25 kbps", 5 );
        cBitratePreset->addItem( "19.85 kbps", 6 );
        cBitratePreset->addItem( "23.05 kbps", 7 );
        cBitratePreset->addItem( "23.85 kbps", 8 );
    }
}

QString SoxCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool SoxCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

QDomDocument SoxCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("compressionLevel",sCompressionLevel->value());
    root.appendChild(encodingOptions);
    return profile;
}

bool SoxCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    Q_UNUSED(profile)

    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    sCompressionLevel->setValue( encodingOptions.attribute("compressionLevel").toInt() );
    return true;
}

int SoxCodecWidget::currentDataRate() // TODO
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        dataRate = 6400000;
    }

    return dataRate;
}

void SoxCodecWidget::compressionLevelSliderChanged( int quality )
{
    iCompressionLevel->setValue( quality );
}

void SoxCodecWidget::compressionLevelSpinBoxChanged( int quality )
{
    sCompressionLevel->setValue( quality );
}

void SoxCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 9 );
        sQuality->setSingleStep( 1 );
        dQuality->setRange( 0, 9 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 5 );
        dQuality->setValue( 5 );
        //dQuality->setValue( qualityForBitrate(dQuality->value()) );
        //qualitySpinBoxChanged( dQuality->value() );
        dQuality->setToolTip( i18n("Quality level from 9 to 0 where 0 is the highes quality.") );
    }
    else
    {
        sQuality->setRange( 8, 320 );
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 8, 320 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        dQuality->setValue( 160 );
        //dQuality->setValue( bitrateForQuality(dQuality->value()) );
        //qualitySpinBoxChanged( dQuality->value() );
    }
}

void SoxCodecWidget::qualitySliderChanged( int quality )
{
    double value;
    if( currentFormat == "ogg vorbis" )
        value = double(quality)/100.0;
    else if( currentFormat == "mp3" && cMode->currentIndex() == 0 )
        value = 9 - quality;
    else
        value = quality;

    dQuality->setValue( value );
}

void SoxCodecWidget::qualitySpinBoxChanged( double quality )
{
    int value;
    if( currentFormat == "ogg vorbis" )
        value = round(quality*100.0);
    else if( currentFormat == "mp3" && cMode->currentIndex() == 0 )
        value = 9 - quality;
    else
        value = quality;

    sQuality->setValue( value );
}

