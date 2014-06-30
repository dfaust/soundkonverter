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


#ifndef RIPPERPLUGIN_H
#define RIPPERPLUGIN_H

#include "backendplugin.h"

#include <KUrl>

class RipperPlugin;


class KDE_EXPORT RipperPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    RipperPluginItem( QObject *parent=0 );
    virtual ~RipperPluginItem();

    struct Data {
        int fromSector; // cd paranoia
        int toSector;

        int fileCount; // icedax
        int processedFiles;
        float lastFileProgress;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT RipperPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    RipperPlugin( QObject *parent=0 );
    virtual ~RipperPlugin();

    virtual QString type();

    virtual QList<ConversionPipeTrunk> codecTable() = 0;

    /** rips a track */
    virtual unsigned int rip( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
    /** returns a command for ripping a track through a pipe; "" if pipes aren't supported */
    virtual QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
};

#define K_EXPORT_SOUNDKONVERTER_RIPPER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_ripper_##libname, KGenericFactory<classname>("soundkonverter_ripper_" #libname) )

#endif // RIPPERPLUGIN_H

