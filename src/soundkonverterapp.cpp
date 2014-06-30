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


#include "soundkonverterapp.h"
#include "soundkonverter.h"

#include <KCmdLineArgs>
#include <KStandardDirs>
#include <KUrl>
#include <QFile>


soundKonverterApp::soundKonverterApp()
    : KUniqueApplication()
{
    mainWindow = new soundKonverter();
    setActiveWindow( mainWindow );
}

soundKonverterApp::~soundKonverterApp()
{}

int soundKonverterApp::newInstance()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    static bool first = true;
    bool visible = true;
    bool autoclose = false;
    bool autostart = false;
    bool activateMainWindow = true;
    
    if( ( first || !mainWindow->isVisible() ) && args->isSet("replaygain") && args->count() > 0 )
        visible = false;

    autoclose = args->isSet( "autoclose" );
    autostart = args->isSet( "autostart" );

    const QString profile = args->getOption( "profile" );
    const QString format = args->getOption( "format" );
    const QString directory = args->getOption( "output" );
    const QString notifyCommand = args->getOption( "command" );

    if( args->isSet( "invisible" ) )
    {
        autoclose = true;
        autostart = true;
        visible = false;
        mainWindow->showSystemTray();
    }

    if( first && QFile::exists(KStandardDirs::locateLocal("data","soundkonverter/filelist_autosave.xml")) )
    {
        if( !visible )
        {
            visible = true;
            autoclose = false;
            autostart = false;
            mainWindow->show();
        }
        mainWindow->show();
        kapp->processEvents();
        mainWindow->loadAutosaveFileList();
    }

    const QString device = args->getOption( "rip" );
    if( !device.isEmpty() )
    {
        const bool success = mainWindow->ripCd( device, profile, format, directory, notifyCommand );
        if( !success && first )
        {
            kapp->quit();
            return 0;
        }
    }

    if( visible )
        mainWindow->show();

    mainWindow->setAutoClose( autoclose );

    if( args->isSet( "replaygain" ) )
    {
        KUrl::List urls;
        for( int i=0; i<args->count(); i++ )
        {
            urls.append( args->arg(i) );
        }
        if( !urls.isEmpty() )
        {
            mainWindow->addReplayGainFiles( urls );
            activateMainWindow = false;
        }
    }
    else
    {
        KUrl::List urls;
        for( int i=0; i<args->count(); i++ )
        {
            urls.append( args->arg(i) );
        }
        if( !urls.isEmpty() )
            mainWindow->addConvertFiles( urls, profile, format, directory, notifyCommand );
    }
    args->clear();

    if( activateMainWindow )
        mainWindow->activateWindow();

    if( autostart )
        mainWindow->startConversion();

    if( first )
        mainWindow->startupChecks();

    first = false;

    return 0;
}

