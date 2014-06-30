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


#ifndef FILTERPLUGIN_H
#define FILTERPLUGIN_H

#include "codecplugin.h"
#include "conversionoptions.h"

class QWidget;
class FilterPlugin;
class FilterWidget;
class TagData;


// struct FilterPipe
// {
//     FilterPlugin *plugin;
//     bool enabled; // can we use this conversion pipe? (all needed backends installed?)
//     QString problemInfo; // howto message, if a backend is missing
// };


class KDE_EXPORT FilterPluginItem : public CodecPluginItem
{
    Q_OBJECT
public:
    FilterPluginItem( QObject *parent=0 );
    virtual ~FilterPluginItem();
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT FilterPlugin : public CodecPlugin
{
    Q_OBJECT
public:
    FilterPlugin( QObject *parent=0 );
    virtual ~FilterPlugin();

    virtual QString type();

    virtual FilterWidget *newFilterWidget() = 0;
    virtual FilterWidget *deleteFilterWidget( FilterWidget *filterWidget );

//     /**
//      * starts the conversion and returns either a conversion id or an error code:
//      *
//      * -1   unknown error
//      * -100 plugin not configured
//      */
//     virtual int filter( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions ) = 0;
//     /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
//     virtual QStringList filterCommand( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions ) = 0;

    virtual FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

protected:
    FilterOptions *lastUsedFilterOptions;

};

#define K_EXPORT_SOUNDKONVERTER_FILTER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_filter_##libname, KGenericFactory<classname>("soundkonverter_filter_" #libname) )

#endif // FILTERPLUGIN_H

