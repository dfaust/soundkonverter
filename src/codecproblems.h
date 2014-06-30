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



#ifndef CODECPROBLEMS_H
#define CODECPROBLEMS_H

#include <KDialog>


/**
 * @short Shows a message box with possible solutions for backend problems
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class CodecProblems : public KDialog
{
    Q_OBJECT
public:
    enum Mode {
        Debug,
        Decode,
        ReplayGain,
        AudioCd
    };
    
    struct Problem {
        QString codecName;
        QStringList solutions;
        QStringList affectedFiles;
    };
    
    /** Default Constructor */
    CodecProblems( Mode mode, const QList<Problem>& problemList, QWidget *parent=0, Qt::WFlags f=0 );

    /** Default Destructor */
    ~CodecProblems();

};

#endif // CODECPROBLEMS_H
