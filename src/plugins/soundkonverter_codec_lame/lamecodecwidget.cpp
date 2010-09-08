
#include "lamecodecglobal.h"

#include "lamecodecwidget.h"
#include "lameconversionoptions.h"

#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <QSpinBox>
// #include <QSlider>

#include <KLineEdit>


LameCodecWidget::LameCodecWidget()
    : CodecWidget(),
    currentFormat( "mp3" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up preset selection

    QHBoxLayout *presetBox = new QHBoxLayout();
    grid->addLayout( presetBox, 0, 0 );

    QLabel *lPreset = new QLabel( i18n("Preset")+":", this );
    presetBox->addWidget( lPreset );
    cPreset = new KComboBox( this );
    cPreset->addItem( i18n("Medium") );
    cPreset->addItem( i18n("Standard") );
    cPreset->addItem( i18n("Extreme") );
    cPreset->addItem( i18n("Insane") );
    cPreset->addItem( i18n("Specify bitrate") );
    cPreset->addItem( i18n("User defined") );
    connect( cPreset, SIGNAL(activated(const QString&)), this, SLOT(presetChanged(const QString&)) );
    connect( cPreset, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    presetBox->addWidget( cPreset );
//     cPreset->setToolTip( i18n("Either use one of lames's presets or your own settings.") );

    iPresetBitrate = new QSpinBox( this );
    iPresetBitrate->setRange( 8, 320 );
    iPresetBitrate->setSuffix( " kbps" );
    iPresetBitrate->setValue( 192 );
    connect( iPresetBitrate, SIGNAL(valueChanged(int)), this, SLOT(presetBitrateChanged(int)) );
    connect( iPresetBitrate, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    presetBox->addWidget( iPresetBitrate );

    cPresetBitrateCbr = new QCheckBox( i18n("cbr"), this );
    connect( cPresetBitrateCbr, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    presetBox->addWidget( cPresetBitrateCbr );
    cPresetBitrateCbr->setToolTip( i18n("Encode using a constant bitrate.\nOnly works with 80, 96, 112, 128, 160, 192, 224, 256 and 320 kbps") );

    presetBox->addSpacing( 12 );

    cPresetFast = new QCheckBox( i18n("Fast encoding"), this );
    connect( cPresetFast, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    presetBox->addWidget( cPresetFast );
    cPresetFast->setToolTip( i18n("Use a faster encoding alogrithm (results in a slightly lower output quality).") );

    presetBox->addStretch();

    // set up user defined options

    userdefinedBox = new QGroupBox( /*i18n("User defined"),*/ this );
    grid->addWidget( userdefinedBox, 1, 0 );

    QVBoxLayout *userdefinedBoxLayout = new QVBoxLayout();
    userdefinedBox->setLayout( userdefinedBoxLayout );

    // top box ----------------------------------------

    QHBoxLayout *userdefinedTopBox = new QHBoxLayout();
    userdefinedBoxLayout->addLayout( userdefinedTopBox );

    QLabel *lMode = new QLabel( i18n("Mode")+":", userdefinedBox );
    userdefinedTopBox->addWidget( lMode );

    cMode = new KComboBox( userdefinedBox );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    userdefinedTopBox->addWidget( cMode );

    sQuality = new QSlider( Qt::Horizontal, userdefinedBox );
    sQuality->setRange( 8, 320 );
//     sQuality->setTickInterval( 32 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    userdefinedTopBox->addWidget( sQuality );

    iQuality = new QSpinBox( userdefinedBox );
    iQuality->setRange( 8, 320 );
    iQuality->setSuffix( " kbps" );
    iQuality->setFixedWidth( iQuality->sizeHint().width() );
    connect( iQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( iQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    userdefinedTopBox->addWidget( iQuality );

    userdefinedTopBox->addSpacing( 12 );

    QLabel *lBitrateMode = new QLabel( i18n("Bitrate mode")+":", this );
    userdefinedTopBox->addWidget( lBitrateMode );
    cBitrateMode = new KComboBox( this );
    cBitrateMode->addItem( i18n("Variable") );
    cBitrateMode->addItem( i18n("Avarage") );
    cBitrateMode->addItem( i18n("Constant") );
    cBitrateMode->setFixedWidth( cBitrateMode->sizeHint().width() );
    connect( cBitrateMode, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    userdefinedTopBox->addWidget( cBitrateMode );
    
    /*
    QLabel *lBitrateRange = new QLabel( i18n("Bitrate range")+":", userdefinedBox );
    userdefinedTopBox->addWidget( lBitrateRange );
    QSpinBox *iMinBitrate = new QSpinBox( userdefinedBox );
    iMinBitrate->setRange( 8, 320 );
//     iMinBitrate->setSuffix( " kbps" );
//     iMinBitrate->setFixedHeight( cMode->minimumSizeHint().height() );
    userdefinedTopBox->addWidget( iMinBitrate );
    QLabel *lBitrateRangeTo = new QLabel( "-", userdefinedBox );
    userdefinedTopBox->addWidget( lBitrateRangeTo );
    QSpinBox *iMaxBitrate = new QSpinBox( userdefinedBox );
    iMaxBitrate->setRange( 8, 320 );
//     iMaxBitrate->setSuffix( " kbps" );
//     iMaxBitrate->setFixedHeight( cMode->minimumSizeHint().height() );
    userdefinedTopBox->addWidget( iMaxBitrate );
    */

    userdefinedTopBox->addStretch();

    // mid box ----------------------------------------

    QHBoxLayout *userdefinedMidBox = new QHBoxLayout();
    userdefinedBoxLayout->addLayout( userdefinedMidBox );

    chChannels = new QCheckBox( i18n("Channels")+":", userdefinedBox );
    connect( chChannels, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    userdefinedMidBox->addWidget( chChannels );
    cChannels = new KComboBox( userdefinedBox );
    cChannels->addItem( i18n("Mono") );
    cChannels->addItem( i18n("Joint Stereo") );
    cChannels->addItem( i18n("Simple Stereo") );
    cChannels->addItem( i18n("Forced Joint Stereo") );
    cChannels->addItem( i18n("Dual Mono") );
    cChannels->setEnabled( false );
    connect( cChannels, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    userdefinedMidBox->addWidget( cChannels );
    connect( chChannels, SIGNAL(toggled(bool)), cChannels, SLOT(setEnabled(bool)) );

    userdefinedMidBox->addSpacing( 12 );

    chSamplerate = new QCheckBox( i18n("Resample")+":", userdefinedBox );
    connect( chSamplerate, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    userdefinedMidBox->addWidget( chSamplerate );
    cSamplerate = new KComboBox( userdefinedBox );
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
    userdefinedMidBox->addWidget( cSamplerate );
    connect( chSamplerate, SIGNAL(toggled(bool)), cSamplerate, SLOT(setEnabled(bool)) );

    userdefinedMidBox->addStretch();

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

    presetChanged( cPreset->currentText() );
    modeChanged( 0 );
}

LameCodecWidget::~LameCodecWidget()
{}

// TODO optimize
int LameCodecWidget::bitrateForQuality( int quality )
{
    return 320-quality*32;
}

// TODO optimize
int LameCodecWidget::qualityForBitrate( int bitrate )
{
    return (320-bitrate)/32;
}

ConversionOptions *LameCodecWidget::currentConversionOptions()
{
    LameConversionOptions *options = new LameConversionOptions();
    options->data.preset = (LameConversionOptions::Data::Preset)cPreset->currentIndex();
    options->data.presetBitrate = iPresetBitrate->value();
    options->data.presetBitrateCbr = cPresetBitrateCbr->isEnabled() && cPresetBitrateCbr->isChecked();
    options->data.presetFast = cPresetFast->isEnabled() && cPresetFast->isChecked();
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
        options->bitrateMode = ( cBitrateMode->currentText()==i18n("Avarage") ) ? ConversionOptions::Abr : ConversionOptions::Cbr;
        options->bitrateMin = 0;
        options->bitrateMax = 0;
    }
    if( chSamplerate->isChecked() ) options->samplingRate = cSamplerate->currentText().replace(" Hz","").toInt();
    else options->samplingRate = 0;
    if( chChannels->isChecked() ) options->channels = cChannels->currentIndex() + 1;
    else options->channels = 0;
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";

    return options;
}

bool LameCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    LameConversionOptions *options = static_cast<LameConversionOptions*>(_options);
    cPreset->setCurrentIndex( (int)options->data.preset );
    presetChanged( cPreset->currentText() );
    iPresetBitrate->setValue( options->data.presetBitrate );
    cPresetBitrateCbr->setChecked( options->data.presetBitrateCbr );
    cPresetFast->setChecked( options->data.presetFast );
    if( options->qualityMode == ConversionOptions::Quality )
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Quality")) );
        modeChanged( cMode->currentIndex() );
        iQuality->setValue( options->quality );
        cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Variable")) );
    }
    else
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Bitrate")) );
        modeChanged( cMode->currentIndex() );
        iQuality->setValue( options->bitrate );
        if( options->bitrateMode == ConversionOptions::Abr ) cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Avarage")) );
        else cBitrateMode->setCurrentIndex( cBitrateMode->findText(i18n("Constant")) );
    }
    chSamplerate->setChecked( options->samplingRate != 0 );
    if( options->samplingRate != 0 ) cSamplerate->setCurrentIndex( cSamplerate->findText(QString::number(options->samplingRate)+" Hz") );
    chChannels->setChecked( options->channels != 0 );
    if( options->channels != 0 ) cChannels->setCurrentIndex( options->channels - 1 );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() ) lCmdArguments->setText( options->cmdArguments );

    return true;
}

void LameCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString LameCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cPreset->currentIndex() == 5 && cMode->currentIndex() == 0 && iQuality->value() == 6 && chChannels->isChecked() && cChannels->currentIndex() == 0 && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Very low");
    }
    else if( cPreset->currentIndex() == 5 && cMode->currentIndex() == 0 && iQuality->value() == 5 && !chChannels->isChecked() && chSamplerate->isChecked() && cSamplerate->currentIndex() == 4 )
    {
        return i18n("Low");
    }
    else if( cPreset->currentIndex() == 5 && cMode->currentIndex() == 0 && iQuality->value() == 4 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("Medium");
    }
    else if( cPreset->currentIndex() == 5 && cMode->currentIndex() == 0 && iQuality->value() == 3 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("High");
    }
    else if( cPreset->currentIndex() == 5 && cMode->currentIndex() == 0 && iQuality->value() == 2 && !chChannels->isChecked() && !chSamplerate->isChecked() )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool LameCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cPreset->setCurrentIndex( 5 );
        presetChanged( cPreset->currentText() );
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 6 );
        iQuality->setValue( 6 );
        cBitrateMode->setCurrentIndex( 0 );
        chChannels->setChecked( true );
        cChannels->setCurrentIndex( 0 );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cPreset->setCurrentIndex( 5 );
        presetChanged( cPreset->currentText() );
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
        cBitrateMode->setCurrentIndex( 0 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( true );
        cSamplerate->setCurrentIndex( 4 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cPreset->setCurrentIndex( 5 );
        presetChanged( cPreset->currentText() );
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 4 );
        iQuality->setValue( 4 );
        cBitrateMode->setCurrentIndex( 0 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cPreset->setCurrentIndex( 5 );
        presetChanged( cPreset->currentText() );
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 3 );
        iQuality->setValue( 3 );
        cBitrateMode->setCurrentIndex( 0 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cPreset->setCurrentIndex( 5 );
        presetChanged( cPreset->currentText() );
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 2 );
        iQuality->setValue( 2 );
        cBitrateMode->setCurrentIndex( 0 );
        chChannels->setChecked( false );
        chSamplerate->setChecked( false );
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

QDomDocument LameCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode",cMode->currentIndex());
    encodingOptions.setAttribute("quality",iQuality->value());
    encodingOptions.setAttribute("bitrateMode",cBitrateMode->currentIndex());
    encodingOptions.setAttribute("channelsEnabled",chChannels->isChecked() && chChannels->isEnabled());
    encodingOptions.setAttribute("channels",cChannels->currentIndex());
    encodingOptions.setAttribute("samplerateEnabled",chSamplerate->isChecked() && chSamplerate->isEnabled());
    encodingOptions.setAttribute("samplerate",cSamplerate->currentIndex());
    encodingOptions.setAttribute("cmdArgumentsEnabled",cCmdArguments->isChecked() && cCmdArguments->isEnabled());
    encodingOptions.setAttribute("cmdArguments",lCmdArguments->text());
    root.appendChild(encodingOptions);
    QDomElement data = profile.createElement("data");
    data.setAttribute("preset",cPreset->currentIndex());
    data.setAttribute("presetBitrate",iPresetBitrate->value());
    data.setAttribute("presetBitrateCbr",cPresetBitrateCbr->isChecked() && cPresetBitrateCbr->isEnabled());
    data.setAttribute("presetFast",cPresetFast->isChecked() && cPresetFast->isEnabled());
    encodingOptions.appendChild(data);
    return profile;
}

bool LameCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    cPreset->setCurrentIndex( data.attribute("preset").toInt() );
    presetChanged( cPreset->currentText() );
    iPresetBitrate->setValue( data.attribute("presetBitrate").toInt() );
    presetBitrateChanged( iPresetBitrate->value() );
    cPresetBitrateCbr->setChecked( data.attribute("presetBitrateCbr").toInt() );
    cPresetFast->setChecked( data.attribute("presetFast").toInt() );
    cMode->setCurrentIndex( encodingOptions.attribute("qualityMode").toInt() );
    modeChanged( cMode->currentIndex() );
    sQuality->setValue( 9 - encodingOptions.attribute("quality").toInt() );
    iQuality->setValue( encodingOptions.attribute("quality").toInt() );
    cBitrateMode->setCurrentIndex( encodingOptions.attribute("bitrateMode").toInt() );
    chChannels->setChecked( encodingOptions.attribute("channelsEnabled").toInt() );
    cChannels->setCurrentIndex( encodingOptions.attribute("channels").toInt() );
    chSamplerate->setChecked( encodingOptions.attribute("samplerateEnabled").toInt() );
    cSamplerate->setCurrentIndex( encodingOptions.attribute("samplerate").toInt() );
    cCmdArguments->setChecked( encodingOptions.attribute("cmdArgumentsEnabled").toInt() );
    lCmdArguments->setText( encodingOptions.attribute("cmdArguments") );
    return true;
}

int LameCodecWidget::currentDataRate()
{
    int dataRate;
    
    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        if( cPreset->currentIndex() == 0 )
        {
            dataRate = 1090000;
        }
        else if( cPreset->currentIndex() == 1 )
        {
            dataRate = 1140000;
        }
        else if( cPreset->currentIndex() == 2 )
        {
            dataRate = 1400000;
        }
        else if( cPreset->currentIndex() == 3 )
        {
            dataRate = 2360000;
        }
        else if( cPreset->currentIndex() == 4 )
        {
            dataRate = iPresetBitrate->value()/8*60*1000;
        }
        else if( cPreset->currentIndex() == 5 )
        {
            if( cMode->currentIndex() == 0 )
            {
                dataRate = 1500000 - iQuality->value()*100000;
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
                else if( cChannels->currentIndex() == 4 )
                {
                    dataRate *= 1.5f;
                }
            }
            if( chSamplerate->isChecked() && cSamplerate->currentText().replace(" Hz","").toInt() <= 22050 )
            {
                dataRate *= 0.9f;
            }
        }
        if( cPresetFast->isEnabled() && cPresetFast->isChecked() )
        {
            dataRate *= 1.1f;
        }
    }
    
    return dataRate;
}

void LameCodecWidget::presetChanged( const QString& preset )
{
    cPreset->setToolTip( "" );

    if( preset == i18n("Medium") )
    {
        iPresetBitrate->setEnabled( false );
        cPresetFast->setEnabled( true );
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("This preset should provide near transparency to most people on most music (~150 kbps abr).") );
    }
    else if( preset == i18n("Standard") )
    {
        iPresetBitrate->setEnabled( false );
        cPresetFast->setEnabled( true );
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("This preset should generally be transparent to most people on most music\nand is already quite high in quality (~230 kbps abr).") );
    }
    else if( preset == i18n("Extreme") )
    {
        iPresetBitrate->setEnabled( false );
        cPresetFast->setEnabled( true );
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("If you have extremely good hearing and similar equipment, this preset will generally provide\nslightly higher quality than the standard mode (~280 kbps abr).") );
    }
    else if( preset == i18n("Insane") )
    {
        iPresetBitrate->setEnabled( false );
        cPresetFast->setEnabled( false );
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("This preset will usually be overkill for most people and most situations, but if you must have\nthe absolute highest quality with no regard to filesize, this is the way to go (320 kbps cbr).") );
    }
    else if( preset == i18n("Specify bitrate") )
    {
        iPresetBitrate->setEnabled( true );
        cPresetFast->setEnabled( false );
        userdefinedBox->setEnabled( false );
    }
    else // "User defined"
    {
        iPresetBitrate->setEnabled( false );
        cPresetFast->setEnabled( false );
        userdefinedBox->setEnabled( true );
    }
    presetBitrateChanged( iPresetBitrate->value() );
}

void LameCodecWidget::presetBitrateChanged( int bitrate )
{
    if( iPresetBitrate->isEnabled() && (
        bitrate == 80 || bitrate == 96 || bitrate == 112 ||
        bitrate == 128 || bitrate == 160 || bitrate == 192 ||
        bitrate == 224 || bitrate == 256 || bitrate == 320 ) )
    {
        cPresetBitrateCbr->setEnabled( true );
    }
    else
    {
        cPresetBitrateCbr->setEnabled( false );
    }
}

void LameCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 9 );
//         sQuality->setTickInterval( 100 );
        sQuality->setSingleStep( 1 );
        iQuality->setRange( 0, 9 );
        iQuality->setSingleStep( 1 );
        iQuality->setSuffix( "" );
        sQuality->setValue( 5 );
        iQuality->setValue( 5 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        iQuality->setToolTip( i18n("Quality level from 9 to 0 where 0 is the highes quality.") );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Variable") );
        cBitrateMode->setEnabled( false );
    }
    else
    {
        sQuality->setRange( 8, 320 );
//         sQuality->setTickInterval( 800 );
        sQuality->setSingleStep( 8 );
        iQuality->setRange( 8, 320 );
        iQuality->setSingleStep( 8 );
        iQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        iQuality->setValue( 160 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        iQuality->setToolTip( i18n("Bitrate") );

        cBitrateMode->clear();
        cBitrateMode->addItem( i18n("Avarage") );
        cBitrateMode->addItem( i18n("Constant") );
        cBitrateMode->setEnabled( true );
    }
}

void LameCodecWidget::qualitySliderChanged( int quality )
{
    if( cMode->currentText() == i18n("Quality") )
    {
        iQuality->setValue( 9 - quality );
    }
    else
    {
        iQuality->setValue( quality );
    }
}

void LameCodecWidget::qualitySpinBoxChanged( int quality )
{
    if( cMode->currentText() == i18n("Quality") )
    {
        sQuality->setValue( 9 - quality );
    }
    else
    {
        sQuality->setValue( quality );
    }
}


