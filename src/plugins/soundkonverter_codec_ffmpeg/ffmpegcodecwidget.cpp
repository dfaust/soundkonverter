
#include "ffmpegcodecglobal.h"

#include "ffmpegcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>
#include <KComboBox>
#include <KLineEdit>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>


FFmpegCodecWidget::FFmpegCodecWidget()
    : CodecWidget(),
    currentFormat( "ogg vorbis" )
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

    cBitrate = new KComboBox( this );
    cBitrate->addItem( "32 kbps" );
    cBitrate->addItem( "40 kbps" );
    cBitrate->addItem( "48 kbps" );
    cBitrate->addItem( "56 kbps" );
    cBitrate->addItem( "64 kbps" );
    cBitrate->addItem( "80 kbps" );
    cBitrate->addItem( "96 kbps" );
    cBitrate->addItem( "112 kbps" );
    cBitrate->addItem( "128 kbps" );
    cBitrate->addItem( "160 kbps" );
    cBitrate->addItem( "192 kbps" );
    cBitrate->addItem( "224 kbps" );
    cBitrate->addItem( "256 kbps" );
    cBitrate->addItem( "320 kbps" );
    cBitrate->addItem( "384 kbps" );
    cBitrate->addItem( "448 kbps" );
    cBitrate->addItem( "512 kbps" );
    cBitrate->addItem( "576 kbps" );
    cBitrate->addItem( "640 kbps" );
    cBitrate->setCurrentIndex( 10 );
    cBitrate->hide();
    connect( cBitrate, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cBitrate );

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

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 2, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments")+":", this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 3, 1 );
}

FFmpegCodecWidget::~FFmpegCodecWidget()
{}

ConversionOptions *FFmpegCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Bitrate;
    if( currentFormat == "ac3" )
        options->bitrate = cBitrate->currentText().replace(" kbps","").toInt();
    else
        options->bitrate = iBitrate->value();
    options->quality = -1000;
    options->bitrateMode = ConversionOptions::Cbr;
    options->bitrateMin = 0;
    options->bitrateMax = 0;
    if( chSamplerate->isChecked() ) options->samplingRate = cSamplerate->currentText().replace(" Hz","").toInt();
    else options->samplingRate = 0;
    if( chChannels->isChecked() ) options->channels = 1;
    else options->channels = 0;
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";

    return options;
}

bool FFmpegCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;

    if( currentFormat == "ac3" )
        cBitrate->setCurrentIndex( cBitrate->findText(QString::number(options->bitrate)+" kbps") );
    else
        iBitrate->setValue( options->bitrate );
    chSamplerate->setChecked( options->samplingRate != 0 );
    if( options->samplingRate != 0 )
        cSamplerate->setCurrentIndex( cSamplerate->findText(QString::number(options->samplingRate)+" Hz") );
    chChannels->setChecked( options->channels != 0 );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() )
        lCmdArguments->setText( options->cmdArguments );

    return true;
}

void FFmpegCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;

    if( currentFormat == "ac3" )
    {
        sBitrate->hide();
        iBitrate->hide();
        cBitrate->show();
    }
    else
    {
        sBitrate->show();
        iBitrate->show();
        cBitrate->hide();
    }

    setEnabled( currentFormat != "wav" && currentFormat != "flac" && currentFormat != "alac" );
}

QString FFmpegCodecWidget::currentProfile()
{
    if( currentFormat == "wav" || currentFormat == "flac" || currentFormat == "alac" )
    {
        return i18n("Lossless");
    }
    else if( currentFormat == "ac3" )
    {
        if( cBitrate->currentText() == "64 kbps" && chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
        {
            return i18n("Very low");
        }
        else if( cBitrate->currentText() == "128 kbps" && !chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
        {
            return i18n("Low");
        }
        else if( cBitrate->currentText() == "192 kbps" && !chChannels->isChecked() && !chSamplerate->isChecked() )
        {
            return i18n("Medium");
        }
        else if( cBitrate->currentText() == "320 kbps" && !chChannels->isChecked() && !chSamplerate->isChecked() )
        {
            return i18n("High");
        }
        else if( cBitrate->currentText() == "640 kbps" && !chChannels->isChecked() && !chSamplerate->isChecked() )
        {
            return i18n("Very high");
        }
    }
    else
    {
        if( iBitrate->value() == 64 && chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
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
    }

    return i18n("User defined");
}

bool FFmpegCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("64 kbps") );
        }
        else
        {
            sBitrate->setValue( 64 );
            iBitrate->setValue( 64 );
        }
        chChannels->setChecked( true );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("128 kbps") );
        }
        else
        {
            sBitrate->setValue( 128 );
            iBitrate->setValue( 128 );
        }
        chChannels->setChecked( false );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("192 kbps") );
        }
        else
        {
            sBitrate->setValue( 160 );
            iBitrate->setValue( 160 );
        }
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("320 kbps") );
        }
        else
        {
            sBitrate->setValue( 240 );
            iBitrate->setValue( 240 );
        }
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("640 kbps") );
        }
        else
        {
            sBitrate->setValue( 320 );
            iBitrate->setValue( 320 );
        }
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

QDomDocument FFmpegCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode","1");
    if( currentFormat == "ac3" )
    {
        encodingOptions.setAttribute("quality",cBitrate->currentText().replace(" kbps","").toInt());
    }
    else
    {
        encodingOptions.setAttribute("quality",iBitrate->value());
    }
    encodingOptions.setAttribute("bitrateMode","1");
    encodingOptions.setAttribute("channelsEnabled",chChannels->isChecked() && chChannels->isEnabled());
    encodingOptions.setAttribute("channels",cChannels->currentIndex());
    encodingOptions.setAttribute("samplerateEnabled",chSamplerate->isChecked() && chSamplerate->isEnabled());
    encodingOptions.setAttribute("samplerate",cSamplerate->currentIndex());
    encodingOptions.setAttribute("cmdArgumentsEnabled",cCmdArguments->isChecked() && cCmdArguments->isEnabled());
    encodingOptions.setAttribute("cmdArguments",lCmdArguments->text());
    root.appendChild(encodingOptions);
    return profile;
}

bool FFmpegCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    Q_UNUSED(profile)

    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    if( currentFormat == "ac3" )
    {
        cBitrate->setCurrentIndex( cBitrate->findText(encodingOptions.attribute("quality")+" kbps") );
    }
    else
    {
        sBitrate->setValue( encodingOptions.attribute("quality").toInt() );
        iBitrate->setValue( encodingOptions.attribute("quality").toInt() );
    }
    chChannels->setChecked( encodingOptions.attribute("channelsEnabled").toInt() );
    cChannels->setCurrentIndex( encodingOptions.attribute("channels").toInt() );
    chSamplerate->setChecked( encodingOptions.attribute("samplerateEnabled").toInt() );
    cSamplerate->setCurrentIndex( encodingOptions.attribute("samplerate").toInt() );
    cCmdArguments->setChecked( encodingOptions.attribute("cmdArgumentsEnabled").toInt() );
    lCmdArguments->setText( encodingOptions.attribute("cmdArguments") );
    return true;
}

int FFmpegCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else if( currentFormat == "flac" || currentFormat == "alac" )
    {
        dataRate = 6520000;
    }
    else if( currentFormat == "ac3" )
    {
        dataRate = cBitrate->currentText().replace(" kbps","").toInt()/8*60*1000;

        if( chChannels->isChecked() )
        {
            dataRate *= 0.9f;
        }
        if( chSamplerate->isChecked() && cSamplerate->currentText().replace(" Hz","").toInt() <= 22050 )
        {
            dataRate *= 0.9f;
        }
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

void FFmpegCodecWidget::qualitySliderChanged( int bitrate )
{
    iBitrate->setValue( bitrate );
}

void FFmpegCodecWidget::qualitySpinBoxChanged( int bitrate )
{
    sBitrate->setValue( bitrate );
}

void FFmpegCodecWidget::channelsToggled( bool enabled )
{
    cChannels->setEnabled( enabled );
}

void FFmpegCodecWidget::samplerateToggled( bool enabled )
{
    cSamplerate->setEnabled( enabled );
}


