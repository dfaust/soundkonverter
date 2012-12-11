
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

    chSampleRate = new QCheckBox( i18n("Sample rate:"), this );
    connect( chSampleRate, SIGNAL(toggled(bool)), SIGNAL(optionsChanged()) );
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
    cSampleRate->setEnabled( false );
    connect( cSampleRate, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    box1->addWidget( cSampleRate );
    connect( chSampleRate, SIGNAL(toggled(bool)), cSampleRate, SLOT(setEnabled(bool)) );

    box1->addSpacing( 12 );

    chSampleSize = new QCheckBox( i18n("Sample size:"), this );
    connect( chSampleSize, SIGNAL(toggled(bool)), SIGNAL(optionsChanged()) );
    box1->addWidget( chSampleSize );
    cSampleSize = new KComboBox( this );
    cSampleSize->addItem( "8 bit" );
    cSampleSize->addItem( "16 bit" );
    cSampleSize->addItem( "24 bit" );
    cSampleSize->addItem( "32 bit" );
    cSampleSize->setCurrentIndex( 1 );
    cSampleSize->setEditable( true );
    cSampleSize->setEnabled( false );
    box1->addWidget( cSampleSize );
    connect( chSampleSize, SIGNAL(toggled(bool)), cSampleSize, SLOT(setEnabled(bool)) );

    box1->addSpacing( 12 );

    chChannels = new QCheckBox( i18n("Channels:"), this );
    connect( chChannels, SIGNAL(toggled(bool)), SIGNAL(optionsChanged()) );
    box1->addWidget( chChannels );
    cChannels = new KComboBox( this );
    cChannels->addItem( i18n("Mono") );
    cChannels->addItem( i18n("Stereo") );
    cChannels->setEnabled( false );
    box1->addWidget( cChannels );
    connect( chChannels, SIGNAL(toggled(bool)), cChannels, SLOT(setEnabled(bool)) );

    box1->addStretch();

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
        options->data.sampleRate = 0;
    }

    if( chSampleSize->isChecked() )
    {
        options->data.sampleSize = cSampleSize->currentText().replace(" bit","").toInt();
        valid = true;
    }
    else
    {
        options->data.sampleSize = 0;
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

    foreach( SoxEffectWidget *effectWidget, effectWidgets )
    {
        const SoxFilterOptions::EffectData data = effectWidget->currentEffectOptions();
        if( data.effectName != i18n("Disabled") )
        {
            options->data.effects.append( data );
            valid = true;
        }
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
    // reset effect widgets
    for( int i=1; i<effectWidgets.count(); i++ )
    {
        effectWidgetsBox->removeWidget( effectWidgets.at(i) );
        effectWidgets.at(i)->deleteLater();
        effectWidgets.removeAt( i );
        i--;
    }
    if( !effectWidgets.isEmpty() && effectWidgets.last() ) // really should always be true
    {
        SoxFilterOptions::EffectData effectData;
        effectWidgets.last()->setEffectOptions( effectData );
        effectWidgets.last()->setAddButtonShown( true );
        if( effectWidgets.count() == 1 )
            effectWidgets.last()->setRemoveButtonShown( false );
    }

    if( !_options )
    {
        chSampleRate->setChecked( false );
        chSampleSize->setChecked( false );
        chChannels->setChecked( false );

        return true;
    }

    if( _options->pluginName != global_plugin_name )
        return false;

    SoxFilterOptions *options = dynamic_cast<SoxFilterOptions*>(_options);

    chSampleRate->setChecked( options->data.sampleRate > 0 );
    if( options->data.sampleRate > 0 )
    {
        cSampleRate->setCurrentItem( QString::number(options->data.sampleRate) + " Hz" );
    }
    chSampleSize->setChecked( options->data.sampleSize > 0 );
    if( options->data.sampleSize > 0 )
    {
        cSampleSize->setCurrentItem( QString::number(options->data.sampleSize) + " bit" );
    }
    chChannels->setChecked( options->data.channels > 0 );
    if( options->data.channels > 0 )
    {
        cChannels->setCurrentIndex( options->data.channels - 1 );
    }

    bool first = true;
    foreach( const SoxFilterOptions::EffectData effectData, options->data.effects )
    {
        if( !first )
            addEffectWidgetClicked();
        effectWidgets.last()->setEffectOptions( effectData );
        first = false;
    }

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


