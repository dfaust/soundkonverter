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


#include "ttaenccodecglobal.h"

#include "ttaenccodecwidget.h"
#include "../../core/conversionoptions.h"

#include <QLayout>
#include <QCheckBox>
#include <KLineEdit>
#include <KLocale>


TTAEncCodecWidget::TTAEncCodecWidget()
    : CodecWidget(),
    currentFormat( "tta" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 1, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments:"), this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 2, 1 );
}

TTAEncCodecWidget::~TTAEncCodecWidget()
{}

ConversionOptions *TTAEncCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    if( cCmdArguments->isChecked() )
        options->cmdArguments = lCmdArguments->text();
    else
        options->cmdArguments = "";

    return options;
}

bool TTAEncCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() )
        lCmdArguments->setText( options->cmdArguments );

    return true;
}

void TTAEncCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString TTAEncCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool TTAEncCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int TTAEncCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        dataRate = 6400000;
    }

    return dataRate;
}


