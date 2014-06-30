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


#ifndef SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_replaygain_wvgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_wvgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_replaygain_wvgain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

};

K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( wvgain, soundkonverter_replaygain_wvgain )


#endif // _SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H_


