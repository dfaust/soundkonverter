//
// C++ Implementation: configpagebase
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configpagebase.h"

#include <QApplication>


ConfigPageBase::ConfigPageBase( QWidget *parent )
 : QWidget( parent )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    spacingOffset = 2 * fontHeight;
    spacingSmall  = 0.5*fontHeight;
    spacingMedium =     fontHeight;
    spacingBig    = 2 * fontHeight;
}

ConfigPageBase::~ConfigPageBase()
{}

void ConfigPageBase::resetDefaults()
{}

void ConfigPageBase::saveSettings()
{}
