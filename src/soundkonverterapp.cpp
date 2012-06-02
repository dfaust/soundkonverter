
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

    if( first && QFile::exists(KStandardDirs::locateLocal("data","soundkonverter/filelist_autosave.xml")) )
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
    first = false;
    args->clear();

    if( activateMainWindow )
        mainWindow->activateWindow();

    if( autostart )
        mainWindow->startConversion();

    return 0;
}

