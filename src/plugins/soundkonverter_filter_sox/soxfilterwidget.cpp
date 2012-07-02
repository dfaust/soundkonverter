
#include "soxfilterglobal.h"

#include "soxfilterwidget.h"
#include "soxeffectwidget.h"
#include "soxfilteroptions.h"

#include <KLocale>
#include <QCheckBox>
#include <QLayout>
#include <QLabel>

#include <KComboBox>


SoxFilterWidget::SoxFilterWidget()
    : FilterWidget()
{
    int gridRow = 0;
    QGridLayout *grid = new QGridLayout( this );
    grid->setMargin( 0 );

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


//     QHBoxLayout *box2 = new QHBoxLayout();
//     grid->addLayout( box2, gridRow++, 0 );
//
//     cNormalize = new QCheckBox( i18n("Normalize:"), this );
//     connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
//     box2->addWidget( cNormalize );
//
//     dNormalizeVolume = new QDoubleSpinBox( this );
//     dNormalizeVolume->setRange( -99, 99 );
//     dNormalizeVolume->setSuffix( " " + i18nc("decibel","dB") );
//     connect( dNormalizeVolume, SIGNAL(valueChanged(double)), this, SLOT(normalizeVolumeChanged(double)) );
//     connect( dNormalizeVolume, SIGNAL(valueChanged(double)), SIGNAL(somethingChanged()) );
//     box2->addWidget( dNormalizeVolume );
//
//     box2->addStretch();
//
//     cNormalize->setChecked( false );
//     dNormalizeVolume->setValue( 0 );
//     dNormalizeVolume->setPrefix( "+" );

    effectWidgetsBox = new QVBoxLayout();
    grid->addLayout( effectWidgetsBox, gridRow++, 0 );

    SoxEffectWidget *effectWidget = new SoxEffectWidget( this );
    effectWidget->setAddButtonShown( true );
    effectWidget->setRemoveButtonShown( false );
    connect( effectWidget, SIGNAL(addEffectWidgetClicked()), this, SLOT(addEffectWidgetClicked()) );
    connect( effectWidget, SIGNAL(removeEffectWidgetClicked(SoxEffectWidget*)), this, SLOT(removeEffectWidgetClicked(SoxEffectWidget*)) );
    effectWidgetsBox->addWidget( effectWidget );
    effectWidgets.append( effectWidget );

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

//     if( cNormalize->isChecked() )
//     {
//         options->data.normalize = true;
//         options->data.normalizeVolume = dNormalizeVolume->value();
//         valid = true;
//     }
//     else
//     {
//         options->data.normalize = false;
//         options->data.normalizeVolume = 0;
//     }

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
//     cNormalize->setChecked( options->data.normalize );
//     dNormalizeVolume->setValue( options->data.normalizeVolume );

    return true;
}

void SoxFilterWidget::addEffectWidgetClicked()
{
    if( !effectWidgets.isEmpty() && effectWidgets.last() ) // really should alway be true
    {
        effectWidgets.last()->setAddButtonShown( false );
        effectWidgets.last()->setRemoveButtonShown( true );
    }

    SoxEffectWidget *effectWidget = new SoxEffectWidget( this );
    effectWidget->setAddButtonShown( true );
    effectWidget->setRemoveButtonShown( true );
    connect( effectWidget, SIGNAL(addEffectWidgetClicked()), this, SLOT(addEffectWidgetClicked()) );
    connect( effectWidget, SIGNAL(removeEffectWidgetClicked(SoxEffectWidget*)), this, SLOT(removeEffectWidgetClicked(SoxEffectWidget*)) );
    effectWidgetsBox->addWidget( effectWidget );
    effectWidgets.append( effectWidget );
}

void SoxFilterWidget::removeEffectWidgetClicked( SoxEffectWidget *widget )
{
    if( !widget )
        return;

    const int index = effectWidgets.indexOf( widget );

    if( index != -1 )
    {
        effectWidgetsBox->removeWidget( widget );
        widget->deleteLater();
        effectWidgets.removeAt( index );
    }

    if( !effectWidgets.isEmpty() && effectWidgets.last() ) // really should alway be true
    {
        effectWidgets.last()->setAddButtonShown( true );
        if( effectWidgets.count() == 1 )
            effectWidgets.last()->setRemoveButtonShown( false );
    }
}


