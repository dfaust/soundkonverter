
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

#include <kurl.h>


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
    
    QString device = args->getOption( "rip" );
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

    mainWindow->setAutoClose( autoclose );
    
    QString profile = args->getOption( "profile" );
    QString format = args->getOption( "format" );
    QString directory = args->getOption( "output" );
    
    if( args->isSet( "replaygain" ) )
    {
        KUrl::List urls;
        for( int i=0; i<args->count(); i++ )
        {
            urls.append( args->arg(i) );
        }
        if( !urls.isEmpty() ) mainWindow->addReplayGainFiles( urls );
    }
    else
    {
        KUrl::List urls;
        for( int i=0; i<args->count(); i++ )
        {
            urls.append( args->arg(i) );
        }
        if( !urls.isEmpty() ) mainWindow->addConvertFiles( urls, profile, format, directory );
    }
    args->clear();
    
    if( visible ) mainWindow->show();
    
    mainWindow->activateWindow();
    
    if( autostart ) mainWindow->startConversion();

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

        QCString notify = args->getOption( "command" );
        if( notify ) {
            widget->setNotify( notify );
        }

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

