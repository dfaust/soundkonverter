
#include "mplayercodecglobal.h"

#include "mplayercodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <QLayout>
#include <QLabel>
// #include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <QDoubleSpinBox>
// #include <QGroupBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>


MPlayerCodecWidget::MPlayerCodecWidget()
    : CodecWidget(),
    currentFormat( "ogg" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lBitrate = new QLabel( i18n("Bitrate")+":", this );
    topBox->addWidget( lBitrate );

    sBitrate = new QSlider( Qt::Horizontal, this );
    sBitrate->setRange( 8, 320 );
    sBitrate->setValue( 160 );
    connect( sBitrate, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sBitrate, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sBitrate );

    iBitrate = new QSpinBox( this );
    iBitrate->setRange( 8, 320 );
    iBitrate->setValue( 160 );
    iBitrate->setSuffix( " kbps" );
    iBitrate->setFixedWidth( iBitrate->sizeHint().width() );
    connect( iBitrate, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( iBitrate, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( iBitrate );

    topBox->addStretch();

    QHBoxLayout *midBox = new QHBoxLayout();
    grid->addLayout( midBox, 1, 0 );

    chChannels = new QCheckBox( i18n("Channels")+":", this );
    connect( chChannels, SIGNAL(toggled(bool)), this, SLOT(channelsToggled(bool)) );
    connect( chChannels, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    midBox->addWidget( chChannels );
    cChannels = new KComboBox( this );
    cChannels->addItem( i18n("Mono") );
    midBox->addWidget( cChannels );
    channelsToggled( false );

    midBox->addSpacing( 12 );

    chSamplerate = new QCheckBox( i18n("Resample")+":", this );
    connect( chSamplerate, SIGNAL(toggled(bool)), this, SLOT(samplerateToggled(bool)) );
    connect( chSamplerate, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    midBox->addWidget( chSamplerate );
    cSamplerate = new KComboBox( this );
    cSamplerate->addItem( "8000 Hz" );
    cSamplerate->addItem( "11025 Hz" );
    cSamplerate->addItem( "12000 Hz" );
    cSamplerate->addItem( "16000 Hz" );
    cSamplerate->addItem( "22050 Hz" );
    cSamplerate->addItem( "24000 Hz" );
    cSamplerate->addItem( "32000 Hz" );
    cSamplerate->addItem( "44100 Hz" );
    cSamplerate->addItem( "48000 Hz" );
    cSamplerate->setCurrentIndex( 4 );
    connect( cSamplerate, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    midBox->addWidget( cSamplerate );
    samplerateToggled( false );

    midBox->addStretch();

    grid->setRowStretch( 2, 1 );
}

MPlayerCodecWidget::~MPlayerCodecWidget()
{}

ConversionOptions *MPlayerCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->pluginName = global_plugin_name;
    options->qualityMode = ConversionOptions::Bitrate;
    options->bitrate = iBitrate->value();
    options->quality = -1000;
    options->bitrateMode = ConversionOptions::Cbr;
    options->bitrateMin = 0;
    options->bitrateMax = 0;
    if( chSamplerate->isChecked() ) options->samplingRate = cSamplerate->currentText().replace(" Hz","").toInt();
    else options->samplingRate = 0;
    if( chChannels->isChecked() ) options->channels = 1;
    else options->channels = 0;

    return options;
}

bool MPlayerCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;
    
    ConversionOptions *options = _options;

    iBitrate->setValue( options->bitrate );
    chSamplerate->setChecked( options->samplingRate != 0 );
    if( options->samplingRate != 0 ) cSamplerate->setCurrentIndex( cSamplerate->findText(QString::number(options->samplingRate)+" Hz") );
    chChannels->setChecked( options->channels != 0 );
    
    return true;
}

void MPlayerCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" && currentFormat != "flac" );
}

QString MPlayerCodecWidget::currentProfile()
{
    if( currentFormat == "wav" || currentFormat == "flac" )
    {
        return i18n("Lossless");
    }
    else if( iBitrate->value() == 64 && chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Very low");
    }
    else if( iBitrate->value() == 128 && !chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Low");
    }
    else if( iBitrate->value() == 160 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("Medium");
    }
    else if( iBitrate->value() == 240 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("High");
    }
    else if( iBitrate->value() == 320 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool MPlayerCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        sBitrate->setValue( 64 );
        iBitrate->setValue( 64 );
        chChannels->setChecked( true );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        sBitrate->setValue( 128 );
        iBitrate->setValue( 128 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        sBitrate->setValue( 160 );
        iBitrate->setValue( 160 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        sBitrate->setValue( 240 );
        iBitrate->setValue( 240 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        sBitrate->setValue( 320 );
        iBitrate->setValue( 320 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        return true;
    }

    return false;
}

QDomDocument MPlayerCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode","1");
    encodingOptions.setAttribute("quality",iBitrate->value());
    encodingOptions.setAttribute("bitrateMode","1");
    encodingOptions.setAttribute("channelsEnabled",chChannels->isChecked() && chChannels->isEnabled());
    encodingOptions.setAttribute("channels",cChannels->currentIndex());
    encodingOptions.setAttribute("samplerateEnabled",chSamplerate->isChecked() && chSamplerate->isEnabled());
    encodingOptions.setAttribute("samplerate",cSamplerate->currentIndex());
    root.appendChild(encodingOptions);
    return profile;
}

bool MPlayerCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    sBitrate->setValue( encodingOptions.attribute("quality").toInt() );
    iBitrate->setValue( encodingOptions.attribute("quality").toInt() );
    chChannels->setChecked( encodingOptions.attribute("channelsEnabled").toInt() );
    cChannels->setCurrentIndex( encodingOptions.attribute("channels").toInt() );
    chSamplerate->setChecked( encodingOptions.attribute("samplerateEnabled").toInt() );
    cSamplerate->setCurrentIndex( encodingOptions.attribute("samplerate").toInt() );
    return true;
}

int MPlayerCodecWidget::currentDataRate()
{
    int dataRate;
    
    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else if( currentFormat == "flac" )
    {
        dataRate = 6520000;
    }
    else
    {
        dataRate = iBitrate->value()/8*60*1000;
        
        if( chChannels->isChecked() )
        {
            dataRate *= 0.9f;
        }
        if( chSamplerate->isChecked() && cSamplerate->currentText().replace(" Hz","").toInt() <= 22050 )
        {
            dataRate *= 0.9f;
        }
    }
    
    return dataRate;
}

void MPlayerCodecWidget::qualitySliderChanged( int bitrate )
{
    iBitrate->setValue( bitrate );
}

void MPlayerCodecWidget::qualitySpinBoxChanged( int bitrate )
{
    sBitrate->setValue( bitrate );
}

void MPlayerCodecWidget::channelsToggled( bool enabled )
{
    cChannels->setEnabled( enabled );
}

void MPlayerCodecWidget::samplerateToggled( bool enabled )
{
    cSamplerate->setEnabled( enabled );
}


