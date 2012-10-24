
#include "soxfilterglobal.h"

#include "soxeffectwidget.h"

#include <KLocale>
#include <QLayout>
#include <QBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include <KComboBox>
#include <KPushButton>


SoxEffectWidget::SoxEffectWidget( QWidget *parent )
    : QWidget( parent )
{
    QHBoxLayout *box = new QHBoxLayout( this );
    box->setMargin( 0 );

    QLabel *lEffect = new QLabel( i18n("Effect:") );
    box->addWidget( lEffect );

    cEffect = new KComboBox( this );
    connect( cEffect, SIGNAL(activated(int)), this, SLOT(effectChanged(int)) );
    cEffect->addItem( i18n("Disabled") );
//     cEffect->addItem( "allpass" );
//     cEffect->addItem( "band" );
//     cEffect->addItem( "bandpass" );
//     cEffect->addItem( "bandreject" );
    cEffect->addItem( "bass" );
//     cEffect->addItem( "bend" );
//     cEffect->addItem( "biquad" );
//     cEffect->addItem( "chorus" );
//     cEffect->addItem( "channels" );
//     cEffect->addItem( "compand" );
//     cEffect->addItem( "contrast" );
//     cEffect->addItem( "dcshift" );
//     cEffect->addItem( "deemph" );
//     cEffect->addItem( "delay" );
//     cEffect->addItem( "dither" );
// //     cEffect1->addItem( "divide" ); // experimental
//     cEffect->addItem( "downsample" );
//     cEffect->addItem( "earwax" );
//     cEffect->addItem( "echo" );
//     cEffect->addItem( "echos" );
//     cEffect->addItem( "equalizer" );
//     cEffect->addItem( "fade" );
//     cEffect->addItem( "fir" );
// //     cEffect1->addItem( "firfit" ); // experimental
//     cEffect->addItem( "flanger" );
//     cEffect->addItem( "gain" );
//     cEffect->addItem( "highpass" );
//     cEffect->addItem( "hilbert" );
// //     cEffect1->addItem( "input" ); // libSoX-only
//     cEffect->addItem( "ladspa" );
//     cEffect->addItem( "loudness" );
//     cEffect->addItem( "lowpass" );
//     cEffect->addItem( "mcompand" );
// //     cEffect1->addItem( "mixer" ); // deprecated
//     cEffect->addItem( "noiseprof" );
//     cEffect->addItem( "noisered" );
    cEffect->addItem( "norm" );
//     cEffect->addItem( "oops" );
// //     cEffect1->addItem( "output" ); // libSoX-only
//     cEffect->addItem( "overdrive" );
//     cEffect->addItem( "pad" );
//     cEffect->addItem( "phaser" );
//     cEffect->addItem( "pitch" );
//     cEffect->addItem( "rate" );
//     cEffect->addItem( "remix" );
//     cEffect->addItem( "repeat" );
//     cEffect->addItem( "reverb" );
//     cEffect->addItem( "reverse" );
//     cEffect->addItem( "riaa" );
//     cEffect->addItem( "silence" );
//     cEffect->addItem( "sinc" );
//     cEffect->addItem( "spectrogram" );
//     cEffect->addItem( "speed" );
//     cEffect->addItem( "splice" );
//     cEffect->addItem( "stat" );
//     cEffect->addItem( "stats" );
//     cEffect->addItem( "stretch" );
//     cEffect->addItem( "swap" );
//     cEffect->addItem( "synth" );
//     cEffect->addItem( "tempo" );
    cEffect->addItem( "treble" );
//     cEffect->addItem( "tremolo" );
//     cEffect->addItem( "trim" );
//     cEffect->addItem( "upsample" );
//     cEffect->addItem( "vad" );
//     cEffect->addItem( "vol" );
    box->addWidget( cEffect );

    widgetsBox = new QHBoxLayout();
    box->addLayout( widgetsBox );

    box->addStretch();

    pRemove = new KPushButton( KIcon("list-remove"), i18n("Remove"), this );
    pRemove->setToolTip( i18n("Remove this effect") );
    box->addWidget( pRemove );
    connect( pRemove, SIGNAL(clicked()), this, SLOT(removeClicked()) );

    pAdd = new KPushButton( KIcon("list-add"), i18n("Add"), this );
    pAdd->setToolTip( i18n("Add another effect") );
    box->addWidget( pAdd );
    connect( pAdd, SIGNAL(clicked()), SIGNAL(addEffectWidgetClicked()) );
    pAdd->setEnabled( false );
}

SoxEffectWidget::~SoxEffectWidget()
{}

void SoxEffectWidget::setAddButtonShown( bool shown )
{
    pAdd->setShown( shown );
}

void SoxEffectWidget::setRemoveButtonShown( bool shown )
{
    pRemove->setShown( shown );
}

void SoxEffectWidget::removeClicked()
{
    emit removeEffectWidgetClicked( this );
}

void SoxEffectWidget::effectChanged( int index )
{
    const QString effect = cEffect->itemText( index );

    foreach( QWidget *widget, widgets )
    {
        widgetsBox->removeWidget( widget );
        widget->deleteLater();
    }
    widgets.clear();

    cEffect->setToolTip( "" );

    if( effect == "norm" )
    {
        cEffect->setToolTip( i18n("Normalize:\nChange the volume so all files have the same loudness") );

        QDoubleSpinBox *dNormalizeVolume = new QDoubleSpinBox( this );
        dNormalizeVolume->setRange( -99, 99 );
        dNormalizeVolume->setSuffix( " " + i18nc("decibel","dB") );
        connect( dNormalizeVolume, SIGNAL(valueChanged(double)), this, SLOT(normalizeVolumeChanged(double)) );
        connect( dNormalizeVolume, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
        widgetsBox->addWidget( dNormalizeVolume );

        dNormalizeVolume->setValue( 0 );
        dNormalizeVolume->setPrefix( "+" );

        widgets.append( (QWidget*)dNormalizeVolume );
    }
    else if( effect == "bass" )
    {
        cEffect->setToolTip( i18n("Bass:\nAmplify the low frequencies") );

        QDoubleSpinBox *dBassGain = new QDoubleSpinBox( this );
        dBassGain->setRange( -99, 99 );
        dBassGain->setSuffix( " " + i18nc("decibel","dB") );
        connect( dBassGain, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
        widgetsBox->addWidget( dBassGain );

        dBassGain->setValue( 0 );

        widgets.append( (QWidget*)dBassGain );
    }
    else if( effect == "treble" )
    {
        cEffect->setToolTip( i18n("Treble:\nAmplify the high frequencies") );

        QDoubleSpinBox *dTrebleGain = new QDoubleSpinBox( this );
        dTrebleGain->setRange( -99, 99 );
        dTrebleGain->setSuffix( " " + i18nc("decibel","dB") );
        connect( dTrebleGain, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
        widgetsBox->addWidget( dTrebleGain );

        dTrebleGain->setValue( 0 );

        widgets.append( (QWidget*)dTrebleGain );
    }

    pAdd->setEnabled( cEffect->currentText() != i18n("Disabled") );
}

SoxFilterOptions::EffectData SoxEffectWidget::currentEffectOptions()
{
    SoxFilterOptions::EffectData effectData;

    const QString effect = cEffect->currentText();
    effectData.effectName = effect;

    if( effect == "norm" )
    {
        if( widgets.isEmpty() )
            return effectData;

        QDoubleSpinBox *dNormalizeVolume = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dNormalizeVolume )
            return effectData;

        effectData.data.append( dNormalizeVolume->value() );
    }
    else if( effect == "bass" )
    {
        if( widgets.isEmpty() )
            return effectData;

        QDoubleSpinBox *dBassGain = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dBassGain )
            return effectData;

        effectData.data.append( dBassGain->value() );
    }
    else if( effect == "treble" )
    {
        if( widgets.isEmpty() )
            return effectData;

        QDoubleSpinBox *dTrebleGain = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dTrebleGain )
            return effectData;

        effectData.data.append( dTrebleGain->value() );
    }

    return effectData;
}

bool SoxEffectWidget::setEffectOptions( SoxFilterOptions::EffectData effectData )
{
    int index = cEffect->findText( effectData.effectName );
    if( index == -1 )
        index = 0;

    cEffect->setCurrentIndex( index );
    effectChanged( index );

    if( effectData.effectName == "norm" )
    {
        if( widgets.isEmpty() )
            return false;

        QDoubleSpinBox *dNormalizeVolume = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dNormalizeVolume )
            return false;

        dNormalizeVolume->setValue( effectData.data.at(0).toDouble() );
    }
    else if( effectData.effectName == "bass" )
    {
        if( widgets.isEmpty() )
            return false;

        QDoubleSpinBox *dBassGain = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dBassGain )
            return false;

        dBassGain->setValue( effectData.data.at(0).toDouble() );
    }
    else if( effectData.effectName == "treble" )
    {
        if( widgets.isEmpty() )
            return false;

        QDoubleSpinBox *dTrebleGain = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
        if( !dTrebleGain )
            return false;

        dTrebleGain->setValue( effectData.data.at(0).toDouble() );
    }

    return true;
}

void SoxEffectWidget::normalizeVolumeChanged( double value )
{
    if( widgets.isEmpty() )
        return;

    QDoubleSpinBox *dNormalizeVolume = qobject_cast<QDoubleSpinBox*>(widgets.at(0));
    if( !dNormalizeVolume )
        return;

    if( value >= 0 )
        dNormalizeVolume->setPrefix( "+" );
    else
        dNormalizeVolume->setPrefix( "" );
}
