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



#ifndef SOUNDKONVERTERAPP_H
#define SOUNDKONVERTERAPP_H


#include <kuniqueapplication.h>

class soundKonverter;

/**
 * @short The soundKonverter application. It controlles ensures that there can only run one instance of soundKonverter.
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class soundKonverterApp : public KUniqueApplication
{
    Q_OBJECT
public:
    /** Constructor */
    soundKonverterApp();

    /** Destructor */
    ~soundKonverterApp();

    /** This function is called, when a new instance of soundKonverter should be created */
    virtual int newInstance();

// private:
    soundKonverter *mainWindow;
};

#endif // SOUNDKONVERTERAPP_H
