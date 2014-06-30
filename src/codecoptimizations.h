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



#ifndef CODECOPTIMIZATIONS_H
#define CODECOPTIMIZATIONS_H

#include <KDialog>

class QRadioButton;


/**
 * @short Shows a message box with suggestions for optimizing the backend order
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class CodecOptimizations : public KDialog
{
    Q_OBJECT
public:
    struct Optimization {
        QString codecName;
        enum Mode {
            Encode,
            Decode,
            ReplayGain
        } mode;
        QString currentBackend;
        QString betterBackend;
        enum Solution {
            Fix,
            Ignore,
            Undecided
        } solution;
    };

    /** Default Constructor */
    CodecOptimizations( const QList<Optimization>& optimizationList, QWidget *parent=0, Qt::WFlags f=0 );

    /** Default Destructor */
    ~CodecOptimizations();

private:
    QList<Optimization> optimizationList;
    QList<QRadioButton*> solutionFixButtons;

private slots:
    void okClicked();

signals:
    void solutions( const QList<CodecOptimizations::Optimization>& solutions );
};

#endif // CODECOPTIMIZATIONS_H
