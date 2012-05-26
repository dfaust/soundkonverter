
#include "normalizefilterglobal.h"

#include "normalizefilterwidget.h"
#include "../../core/conversionoptions.h"



#include <QLayout>
#include <KLocale>
#include <QCheckBox>


NormalizeFilterWidget::NormalizeFilterWidget()
    : FilterWidget()
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QCheckBox *cNormalize = new QCheckBox( i18n("Normalize"), this );
    connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cNormalize );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    cNormalize->setChecked( false );
}

NormalizeFilterWidget::~NormalizeFilterWidget()
{}

FilterOptions *NormalizeFilterWidget::currentFilterOptions()
{
    FilterOptions *options = new FilterOptions();
//     options->compressionLevel = iCompressionLevel->value(); TODO
    return options;
}

bool NormalizeFilterWidget::setCurrentFilterOptions( FilterOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

//     FilterOptions *options = _options;
//     iCompressionLevel->setValue( options->compressionLevel ); TODO
    return true;
}


