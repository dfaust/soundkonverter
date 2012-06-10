
#include "soxfilterglobal.h"

#include "soxfilterwidget.h"
#include "soxfilteroptions.h"

#include <KLocale>
#include <QCheckBox>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include <KComboBox>
#include <KPushButton>

SoxFilterWidget::SoxFilterWidget()
    : FilterWidget()
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    int gridRow = 0;

    // set up filter options selection

    QHBoxLayout *box1 = new QHBoxLayout();
    grid->addLayout( box1, gridRow++, 0 );

    chSampleRate = new QCheckBox( i18n("Sample rate")+":", this );
//     connect( chSamplerate, SIGNAL(toggled(bool)), this, SLOT(samplerateToggled(bool)) );
    connect( chSampleRate, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    box1->addWidget( chSampleRate );
    cSampleRate = new KComboBox( this );
    cSampleRate->addItem( "8000 Hz" );
    cSampleRate->addItem( "11025 Hz" );
    cSampleRate->addItem( "12000 Hz" );
    cSampleRate->addItem( "16000 Hz" );
    cSampleRate->addItem( "22050 Hz" );
    cSampleRate->addItem( "24000 Hz" );
    cSampleRate->addItem( "32000 Hz" );
    cSampleRate->addItem( "44100 Hz" );
    cSampleRate->addItem( "48000 Hz" );
    cSampleRate->setCurrentIndex( 7 );
    cSampleRate->setEditable( true );
    connect( cSampleRate, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    box1->addWidget( cSampleRate );
//     samplerateToggled( false );

    box1->addSpacing( 12 );

    chSampleSize = new QCheckBox( i18n("Sample size")+":", this );
//     connect( chSampleSize, SIGNAL(toggled(bool)), this, SLOT(channelsToggled(bool)) );
    connect( chSampleSize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    box1->addWidget( chSampleSize );
    cSampleSize = new KComboBox( this );
    cSampleSize->addItem( "8 bit" );
    cSampleSize->addItem( "16 bit" );
    cSampleSize->addItem( "24 bit" );
    cSampleSize->addItem( "32 bit" );
    cSampleSize->setCurrentIndex( 1 );
    cSampleSize->setEditable( true );
    box1->addWidget( cSampleSize );
//     channelsToggled( false );

    box1->addSpacing( 12 );

    chChannels = new QCheckBox( i18n("Channels")+":", this );
//     connect( chChannels, SIGNAL(toggled(bool)), this, SLOT(channelsToggled(bool)) );
    connect( chChannels, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    box1->addWidget( chChannels );
    cChannels = new KComboBox( this );
    cChannels->addItem( i18n("Mono") );
    cChannels->addItem( i18n("Stereo") );
    box1->addWidget( cChannels );
//     channelsToggled( false );

    box1->addStretch();


    QHBoxLayout *box2 = new QHBoxLayout();
    grid->addLayout( box2, gridRow++, 0 );

    cNormalize = new QCheckBox( i18n("Normalize:"), this );
    connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    box2->addWidget( cNormalize );

    dNormalizeVolume = new QDoubleSpinBox( this );
    dNormalizeVolume->setRange( -99, 99 );
    dNormalizeVolume->setSuffix( " " + i18nc("decibel","dB") );
    connect( dNormalizeVolume, SIGNAL(valueChanged(double)), this, SLOT(normalizeVolumeChanged(double)) );
    connect( dNormalizeVolume, SIGNAL(valueChanged(double)), SIGNAL(somethingChanged()) );
    box2->addWidget( dNormalizeVolume );

    box2->addStretch();

    cNormalize->setChecked( false );
    dNormalizeVolume->setValue( 0 );
    dNormalizeVolume->setPrefix( "+" );


    QHBoxLayout *effectBox1 = new QHBoxLayout();
    grid->addLayout( effectBox1, gridRow++, 0 );

    QLabel *lEffect1 = new QLabel( i18n("Effect:") );
    effectBox1->addWidget( lEffect1 );

    KComboBox *cEffect1 = new KComboBox( this );
//     connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    cEffect1->addItem( i18n("Disabled") );
    cEffect1->addItem( "allpass" );
    cEffect1->addItem( "band" );
    cEffect1->addItem( "bandpass" );
    cEffect1->addItem( "bandreject" );
    cEffect1->addItem( "bass" );
    cEffect1->addItem( "bend" );
    cEffect1->addItem( "biquad" );
    cEffect1->addItem( "chorus" );
    cEffect1->addItem( "channels" );
    cEffect1->addItem( "compand" );
    cEffect1->addItem( "contrast" );
    cEffect1->addItem( "dcshift" );
    cEffect1->addItem( "deemph" );
    cEffect1->addItem( "delay" );
    cEffect1->addItem( "dither" );
//     cEffect1->addItem( "divide" ); // experimental
    cEffect1->addItem( "downsample" );
    cEffect1->addItem( "earwax" );
    cEffect1->addItem( "echo" );
    cEffect1->addItem( "echos" );
    cEffect1->addItem( "equalizer" );
    cEffect1->addItem( "fade" );
    cEffect1->addItem( "fir" );
//     cEffect1->addItem( "firfit" ); // experimental
    cEffect1->addItem( "flanger" );
    cEffect1->addItem( "gain" );
    cEffect1->addItem( "highpass" );
    cEffect1->addItem( "hilbert" );
//     cEffect1->addItem( "input" ); // libSoX-only
    cEffect1->addItem( "ladspa" );
    cEffect1->addItem( "loudness" );
    cEffect1->addItem( "lowpass" );
    cEffect1->addItem( "mcompand" );
//     cEffect1->addItem( "mixer" ); // deprecated
    cEffect1->addItem( "noiseprof" );
    cEffect1->addItem( "noisered" );
    cEffect1->addItem( "norm" );
    cEffect1->addItem( "oops" );
//     cEffect1->addItem( "output" ); // libSoX-only
    cEffect1->addItem( "overdrive" );
    cEffect1->addItem( "pad" );
    cEffect1->addItem( "phaser" );
    cEffect1->addItem( "pitch" );
    cEffect1->addItem( "rate" );
    cEffect1->addItem( "remix" );
    cEffect1->addItem( "repeat" );
    cEffect1->addItem( "reverb" );
    cEffect1->addItem( "reverse" );
    cEffect1->addItem( "riaa" );
    cEffect1->addItem( "silence" );
    cEffect1->addItem( "sinc" );
    cEffect1->addItem( "spectrogram" );
    cEffect1->addItem( "speed" );
    cEffect1->addItem( "splice" );
    cEffect1->addItem( "stat" );
    cEffect1->addItem( "stats" );
    cEffect1->addItem( "stretch" );
    cEffect1->addItem( "swap" );
    cEffect1->addItem( "synth" );
    cEffect1->addItem( "tempo" );
    cEffect1->addItem( "treble" );
    cEffect1->addItem( "tremolo" );
    cEffect1->addItem( "trim" );
    cEffect1->addItem( "upsample" );
    cEffect1->addItem( "vad" );
    cEffect1->addItem( "vol" );
    effectBox1->addWidget( cEffect1 );

    effectBox1->addStretch();

    KPushButton *pEffect1Remove = new KPushButton( KIcon("list-remove"), i18n("Remove"), this );
    pEffect1Remove->setToolTip( i18n("Remove this effect") );
    effectBox1->addWidget( pEffect1Remove );

    KPushButton *pEffect1Add = new KPushButton( KIcon("list-add"), i18n("Add"), this );
    pEffect1Add->setToolTip( i18n("Add another effect") );
    effectBox1->addWidget( pEffect1Add );

}

SoxFilterWidget::~SoxFilterWidget()
{}

FilterOptions* SoxFilterWidget::currentFilterOptions()
{
    bool valid = false;

    SoxFilterOptions *options = new SoxFilterOptions();

    if( chSampleRate->isChecked() )
    {
        options->data.sampleRate = cSampleRate->currentText().replace(" Hz","").toInt();
        valid = true;
    }
    else
    {
        options->data.sampleRate = false;
    }

    if( chSampleSize->isChecked() )
    {
        options->data.sampleSize = cSampleSize->currentText().replace(" bit","").toInt();
        valid = true;
    }
    else
    {
        options->data.sampleSize = false;
    }

    if( chChannels->isChecked() )
    {
        options->data.channels = cChannels->currentIndex() + 1;
        valid = true;
    }
    else
    {
        options->data.channels = 0;
    }

    if( cNormalize->isChecked() )
    {
        options->data.normalize = true;
        options->data.normalizeVolume = dNormalizeVolume->value();
        valid = true;
    }
    else
    {
        options->data.normalize = false;
        options->data.normalizeVolume = 0;
    }

    if( valid )
    {
        return options;
    }
    else
    {
        return 0;
    }
}

bool SoxFilterWidget::setCurrentFilterOptions( FilterOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    SoxFilterOptions *options = dynamic_cast<SoxFilterOptions*>(_options);
    cNormalize->setChecked( options->data.normalize );
    dNormalizeVolume->setValue( options->data.normalizeVolume );

    return true;
}

void SoxFilterWidget::normalizeVolumeChanged( double value )
{
    if( value >= 0 )
        dNormalizeVolume->setPrefix( "+" );
    else
        dNormalizeVolume->setPrefix( "" );
}



