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


#ifndef REPLAYGAINPLUGIN_H
#define REPLAYGAINPLUGIN_H

#include "backendplugin.h"

#include <QMap>
#include <KUrl>

class ReplayGainPlugin;


struct ReplayGainPipe
{
    QString codecName;

    ReplayGainPlugin *plugin;
    bool enabled; // can we use this conversion pipe? (all needed backends installed?)
    int rating;
    QString problemInfo; // howto message, if a backend is missing
};


class KDE_EXPORT ReplayGainPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    ReplayGainPluginItem( QObject *parent=0 );
    virtual ~ReplayGainPluginItem();

    struct Data // additional data
    {
        int fileCount;
        int processedFiles;
        float lastFileProgress;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT ReplayGainPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    enum ApplyMode {
        Add = 0,
        Remove = 1,
        Force = 2
    };

    ReplayGainPlugin( QObject *parent=0 );
    virtual ~ReplayGainPlugin();

    virtual QString type();

    virtual QList<ReplayGainPipe> codecTable() = 0;

    /** adds replaygain to one or more files */
    virtual unsigned int apply( const KUrl::List& fileList, ApplyMode mode = Add ) = 0;

protected:
    int lastId;
};

#define K_EXPORT_SOUNDKONVERTER_REPLAYGAIN(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_##libname, KGenericFactory<classname>("soundkonverter_replaygain_" #libname) )

#endif // REPLAYGAINPLUGIN_H

