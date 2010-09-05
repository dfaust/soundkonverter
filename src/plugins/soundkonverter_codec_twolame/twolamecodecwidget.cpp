
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
    currentFormat( "mp3" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // top box ----------------------------------------

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lMode = new QLabel( i18n("Mode")+":", this );
    topBox->addWidget( lMode );

    cMode = new KComboBox( this );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cMode );

    sQuality = new QSlider( Qt::Horizontal, this );
    sQuality->setRange( 8, 384 );
//     sQuality->setTickInterval( 32 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sQuality );

    iQuality = new QSpinBox( this );
    iQuality->setRange( 8, 384 );
    iQuality->setSuffix( " kbps" );
    iQuality->setFixedWidth( iQuality->sizeHint().width() );
    connect( iQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( iQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( iQuality );

    topBox->addStretch();

    // mid box ----------------------------------------

    QHBoxLayout *midBox = new QHBoxLayout();
    grid->addLayout( midBox, 1, 0 );

    chChannels = new QCheckBox( i18n("Channels")+":", this );
    connect( chChannels, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    midBox->addWidget( chChannels );
    cChannels = new KComboBox( this );
    cChannels->addItem( i18n("Mono") );
    cChannels->addItem( i18n("Joint") );
    cChannels->addItem( i18n("Stereo") );
    cChannels->addItem( i18n("Dual") );
    cChannels->setEnabled( false );
    connect( cChannels, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    midBox->addWidget( cChannels );
    connect( chChannels, SIGNAL(toggled(bool)), cChannels, SLOT(setEnabled(bool)) );

    midBox->addSpacing( 12 );

    chSamplerate = new QCheckBox( i18n("Resample")+":", this );
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
    cSamplerate->setEnabled( false );
    connect( cSamplerate, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    midBox->addWidget( cSamplerate );
    connect( chSamplerate, SIGNAL(toggled(bool)), cSamplerate, SLOT(setEnabled(bool)) );

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
        options->bitrateMin = 0;
        options->bitrateMax = 0;
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = iQuality->value();
        options->quality = qualityForBitrate( options->bitrate );
        options->bitrateMode = ConversionOptions::Cbr;
        options->bitrateMin = 0;
        options->bitrateMax = 0;
    }
    if( chSamplerate->isChecked() ) options->samplingRate = cSamplerate->currentText().replace(" Hz","").toInt();
    else options->samplingRate = 0;
    if( chChannels->isChecked() ) options->channels = ( cChannels->currentIndex() != 3 ) ? cChannels->currentIndex() + 1 : 5;
    else options->channels = 0;
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
    chSamplerate->setChecked( options->samplingRate != 0 );
    if( options->samplingRate != 0 ) cSamplerate->setCurrentIndex( cSamplerate->findText(QString::number(options->samplingRate)+" Hz") );
    chChannels->setChecked( options->channels != 0 );
    if( options->channels != 0 ) cChannels->setCurrentIndex( ( options->channels != 5 ) ? options->channels - 1 : 3 );
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
    else if( cMode->currentIndex() == 0 && iQuality->value() == -25 && chChannels->isChecked() && cChannels->currentIndex() == 0 && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == -10 && !chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 5 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 20 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && iQuality->value() == 35 && !chChannels->isChecked() && !chSamplerate->isChecked() )
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
        chChannels->setChecked( true );
        cChannels->setCurrentIndex( 0 );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( -10 );
        iQuality->setValue( -10 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 20 );
        iQuality->setValue( 20 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 35 );
        iQuality->setValue( 35 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

QDomDocument TwoLameCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode",cMode->currentIndex());
    encodingOptions.setAttribute("quality",iQuality->value());
    encodingOptions.setAttribute("channelsEnabled",chChannels->isChecked() && chChannels->isEnabled());
    encodingOptions.setAttribute("channels",cChannels->currentIndex());
    encodingOptions.setAttribute("samplerateEnabled",chSamplerate->isChecked() && chSamplerate->isEnabled());
    encodingOptions.setAttribute("samplerate",cSamplerate->currentIndex());
    encodingOptions.setAttribute("cmdArgumentsEnabled",cCmdArguments->isChecked() && cCmdArguments->isEnabled());
    encodingOptions.setAttribute("cmdArguments",lCmdArguments->text());
    root.appendChild(encodingOptions);
    return profile;
}

bool TwoLameCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    cMode->setCurrentIndex( encodingOptions.attribute("qualityMode").toInt() );
    modeChanged( cMode->currentIndex() );
    sQuality->setValue( encodingOptions.attribute("quality").toInt() );
    iQuality->setValue( encodingOptions.attribute("quality").toInt() );
    chChannels->setChecked( encodingOptions.attribute("channelsEnabled").toInt() );
    cChannels->setCurrentIndex( encodingOptions.attribute("channels").toInt() );
    chSamplerate->setChecked( encodingOptions.attribute("samplerateEnabled").toInt() );
    cSamplerate->setCurrentIndex( encodingOptions.attribute("samplerate").toInt() );
    cCmdArguments->setChecked( encodingOptions.attribute("cmdArgumentsEnabled").toInt() );
    lCmdArguments->setText( encodingOptions.attribute("cmdArguments") );
    return true;
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
        if( chChannels->isChecked() )
        {
            if( cChannels->currentIndex() == 0 )
            {
                dataRate *= 0.9f;
            }
            else if( cChannels->currentIndex() == 3 )
            {
                dataRate *= 1.5f;
            }
        }
        if( chSamplerate->isChecked() && cSamplerate->currentText().replace(" Hz","").toInt() <= 22050 )
        {
            dataRate *= 0.9f;
        }
    }
    
    return dataRate;
}

void TwoLameCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( -50, 50 );
//         sQuality->setTickInterval( 100 );
        sQuality->setSingleStep( 5 );
        iQuality->setRange( -50, 50 );
        iQuality->setSingleStep( 5 );
        iQuality->setSuffix( "" );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        iQuality->setToolTip( i18n("Quality level from -50 to 50.") );
    }
    else
    {
        sQuality->setRange( 8, 384 );
//         sQuality->setTickInterval( 800 );
        sQuality->setSingleStep( 8 );
        iQuality->setRange( 8, 384 );
        iQuality->setSingleStep( 8 );
        iQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        iQuality->setValue( 160 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        iQuality->setToolTip( i18n("Bitrate") );
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


