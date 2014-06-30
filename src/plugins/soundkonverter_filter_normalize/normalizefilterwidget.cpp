/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


#include "normalizefilterglobal.h"

#include "normalizefilterwidget.h"
#include "normalizefilteroptions.h"

#include <QLayout>
#include <QHBoxLayout>
#include <KLocale>
#include <QCheckBox>

NormalizeFilterWidget::NormalizeFilterWidget()
    : FilterWidget()
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    cNormalize = new QCheckBox( i18n("Normalize"), this );
    connect( cNormalize, SIGNAL(toggled(bool)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cNormalize );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    cNormalize->setChecked( false );
}

NormalizeFilterWidget::~NormalizeFilterWidget()
{}

FilterOptions* NormalizeFilterWidget::currentFilterOptions()
{
    if( cNormalize->isChecked() )
    {
        NormalizeFilterOptions *options = new NormalizeFilterOptions();
        options->data.normalize = cNormalize->isChecked();
        return options;
    }
    else
    {
        return 0;
    }
}

bool NormalizeFilterWidget::setCurrentFilterOptions( FilterOptions *_options )
{
    if( !_options )
    {
        cNormalize->setChecked( false );

        return true;
    }

    if( _options->pluginName != global_plugin_name )
        return false;

    NormalizeFilterOptions *options = dynamic_cast<NormalizeFilterOptions*>(_options);
    cNormalize->setChecked( options->data.normalize );

    return true;
}


