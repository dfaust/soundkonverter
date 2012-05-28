
#include "soxfilterglobal.h"

#include "soxfilterwidget.h"
#include "soxfilteroptions.h"

#include <KLocale>
#include <QCheckBox>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QLabel>

SoxFilterWidget::SoxFilterWidget()
    : FilterWidget()
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    cNormalize = new QCheckBox( i18n("Normalize:"), this );
    connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cNormalize );

    dNormalizeVolume = new QDoubleSpinBox( this );
    dNormalizeVolume->setRange( -99, 99 );
    dNormalizeVolume->setSuffix( " " + i18nc("decibel","dB") );
    connect( dNormalizeVolume, SIGNAL(valueChanged(double)), this, SLOT(normalizeVolumeChanged(double)) );
    connect( dNormalizeVolume, SIGNAL(valueChanged(double)), SIGNAL(somethingChanged()) );
    topBox->addWidget( dNormalizeVolume );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    cNormalize->setChecked( false );
    dNormalizeVolume->setValue( 0 );
    dNormalizeVolume->setPrefix( "+" );
}

SoxFilterWidget::~SoxFilterWidget()
{}

FilterOptions* SoxFilterWidget::currentFilterOptions()
{
    if( cNormalize->isChecked() )
    {
        SoxFilterOptions *options = new SoxFilterOptions();
        options->data.normalize = cNormalize->isChecked();
        options->data.normalizeVolume = dNormalizeVolume->value();
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



