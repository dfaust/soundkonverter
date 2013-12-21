//
// C++ Interface: global
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

#define SOUNDKONVERTER_VERSION_NUMBER 1005
#define SOUNDKONVERTER_VERSION_STRING "2.0.5"

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class Global
{
public:
    Global();
    ~Global();

    static QString prettyNumber( double num, QString unit, short digits = 3 );

};

#endif
