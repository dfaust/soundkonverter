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


#include "codecoptimizations.h"

#include <KLocale>
#include <KIcon>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>
#include <QButtonGroup>
#include <QRadioButton>


CodecOptimizations::CodecOptimizations( const QList<Optimization>& _optimizationList, QWidget* parent, Qt::WFlags f )
    : KDialog( parent, f ),
    optimizationList( _optimizationList )
{
    setCaption( i18n("Solutions for backend problems") );
    setWindowIcon( KIcon("help-about") );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setButtonFocus( KDialog::Cancel );
    connect( this, SIGNAL(okClicked()), this, SLOT(okClicked()) );

    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );

    QLabel *messageLabel = new QLabel( i18n("You have installed or removed backends and your soundKonverter settings can be optimized."), this );
    box->addWidget( messageLabel );

    QFrame *frame = new QFrame( widget );
    frame->setFrameShape( QFrame::StyledPanel );
    frame->setFrameShadow( QFrame::Sunken );
    box->addWidget( frame );

    QGridLayout *grid = new QGridLayout( frame );
    grid->setColumnStretch( 1, 0 );
    grid->setColumnStretch( 2, 0 );

    for( int i=0; i<optimizationList.count(); i++ )
    {
        const QString codecName = optimizationList.at(i).codecName;
        const Optimization::Mode mode = optimizationList.at(i).mode;
        const QString currentBackend = optimizationList.at(i).currentBackend;
        const QString betterBackend = optimizationList.at(i).betterBackend;
        const Optimization::Solution solution = optimizationList.at(i).solution;

        QLabel *solutionLabel = new QLabel( frame );
        grid->addWidget( solutionLabel, i, 0 );
        if( mode == Optimization::Encode )
        {
            solutionLabel->setText( i18n( "For encoding %1 files the backend '%2' can be replaced with '%3'.", codecName, currentBackend, betterBackend ) );
        }
        else if( mode == Optimization::Decode && codecName == "audio cd" )
        {
            solutionLabel->setText( i18n( "For ripping audio CDs the backend '%1' can be replaced with '%2'.", currentBackend, betterBackend ) );
        }
        else if( mode == Optimization::Decode )
        {
            solutionLabel->setText( i18n( "For decoding %1 files the backend '%2' can be replaced with '%3'.", codecName, currentBackend, betterBackend ) );
        }
        else if( mode == Optimization::ReplayGain )
        {
            solutionLabel->setText( i18n( "For applying Replay Gain to %1 files the backend '%2' can be replaced with '%3'.", codecName, currentBackend, betterBackend ) );
        }

        QRadioButton *solutionIgnore = new QRadioButton( i18n("Ignore"), frame );
        if( solution == Optimization::Ignore )
            solutionIgnore->setChecked( true );
        grid->addWidget( solutionIgnore, i, 1 );

        QRadioButton *solutionFix = new QRadioButton( i18n("Fix"), frame );
        solutionFixButtons.append( solutionFix );
        if( solution != Optimization::Ignore )
            solutionFix->setChecked( true );
        grid->addWidget( solutionFix, i, 2 );

        QButtonGroup *solutionButtonGroup = new QButtonGroup( frame );
        solutionButtonGroup->addButton( solutionIgnore );
        solutionButtonGroup->addButton( solutionFix );
    }
}

CodecOptimizations::~CodecOptimizations()
{}

void CodecOptimizations::okClicked()
{
    for( int i=0; i<optimizationList.count(); i++ )
    {
        if( solutionFixButtons.at(i) && solutionFixButtons.at(i)->isChecked() )
            optimizationList[i].solution = Optimization::Fix;
        else
            optimizationList[i].solution = Optimization::Ignore;
    }

    emit solutions( optimizationList );
}

