
#include "soundkonverterapp.h"
#include "soundkonverter.h"

// #include <qstringlist.h>
// #include <qfile.h>
// #include <qmovie.h>

#include <kglobal.h>
// #include <kstartupinfo.h>
#include <kcmdlineargs.h>
// #include <dcopclient.h>
// #include <ksystemtray.h>
// #include <kstandarddirs.h>
#include <stdio.h>
#include <KMessageBox>
#include <KStandardDirs>

#include <kurl.h>

#include <QFile>


soundKonverterApp::soundKonverterApp()
    : KUniqueApplication()
{
    mainWindow = new soundKonverter();
//     mainWindow->show();
    setActiveWindow( mainWindow );
}

soundKonverterApp::~soundKonverterApp()
{}

int soundKonverterApp::newInstance()
{
    //KCmdLineArgs::setCwd(QDir::currentPath().toUtf8());
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    bool visible = true;
    bool autoclose = false;
    bool autostart = false;
    bool activateMainWindow = true;

    const QString device = args->getOption( "rip" );
    if( !device.isEmpty() )
    {
        mainWindow->ripCd( device );
    }

    autoclose = args->isSet( "autoclose" );
    autostart = args->isSet( "autostart" );

    if( args->isSet( "invisible" ) )
    {
        autoclose = true;
        autostart = true;
        visible = false;
        mainWindow->showSystemTray();
    }

    if( visible )
        mainWindow->show();

    mainWindow->setAutoClose( autoclose );

    if( QFile::exists(KStandardDirs::locateLocal("data","soundkonverter/filelist_autosave.xml")) )
    {
        kapp->processEvents();
        mainWindow->loadAutosaveFileList();
    }

    const QString profile = args->getOption( "profile" );
    const QString format = args->getOption( "format" );
    const QString directory = args->getOption( "output" );
    const QString notifyCommand = args->getOption( "command" );

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

    return 0;



/*
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if( !mainWidget() )
        {
            soundKonverter *widget = new soundKonverter();
            setMainWidget(widget);
            //widget->show();
        }
        else
            KStartupInfo::setNewStartupId( mainWidget(), kapp->startupId());

        soundKonverter *widget = ::qt_cast<soundKonverter*>( mainWidget() );

        widget->increaseInstances();

        QCString device = args->getOption( "rip" );
        if( device ) {
            if( !args->isSet( "invisible" ) ) {
                widget->visible = true;
                widget->show();
                widget->systemTray->hide();
                widget->systemTray->setPixmap( 0 );
            }
            widget->device = device;
            widget->showCdDialog( false );
        }
*/
}

