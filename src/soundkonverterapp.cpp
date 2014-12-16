
#include "soundkonverterapp.h"
#include "soundkonverter.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>
#include <QUrl>
#include <QFile>
#include <QDebug>

soundKonverterApp::soundKonverterApp(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    qDebug() << "soundKonverterApp";
    mainWindow = new SoundKonverter();
    mainWindow->show();
    setActiveWindow(mainWindow);
}

soundKonverterApp::~soundKonverterApp()
{
}

int soundKonverterApp::newInstance()
{
    qDebug() << "soundKonverterApp::newInstance";

    static bool first = true;
    bool visible = true;
    bool autoclose = false;
    bool autostart = false;
    bool activateMainWindow = true;

//     QCommandLineParser parser;
//
//     if( ( first || !mainWindow->isVisible() ) && parser.isSet("replaygain") && parser.positionalArguments().count() > 0 )
//         visible = false;
//
//     autoclose = parser.isSet( "autoclose" );
//     autostart = parser.isSet( "autostart" );
//
//     const QString profile = parser.value( "profile" );
//     const QString format = parser.value( "format" );
//     const QString directory = parser.value( "output" );
//     const QString notifyCommand = parser.value( "command" );
//
//     if( parser.isSet( "invisible" ) )
//     {
//         autoclose = true;
//         autostart = true;
//         visible = false;
//         mainWindow->showSystemTray();
//     }
//
//     if( first && QFile::exists(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/filelist_autosave.xml") )
//     {
//         if( !visible )
//         {
//             visible = true;
//             autoclose = false;
//             autostart = false;
//             mainWindow->show();
//         }
//         mainWindow->show();
//         qApp->processEvents();
//         mainWindow->loadAutosaveFileList();
//     }
//
//     const QString device = parser.value( "rip" );
//     if( !device.isEmpty() )
//     {
//         const bool success = mainWindow->ripCd( device, profile, format, directory, notifyCommand );
//         if( !success && first )
//         {
//             qApp->quit();
//             return 0;
//         }
//     }
//
//     if( visible )
//         mainWindow->show();
//
//     mainWindow->setAutoClose( autoclose );
//
//     if( parser.isSet( "replaygain" ) )
//     {
//         QList<QUrl> urls;
//         for( int i=0; i<parser.positionalArguments().count(); i++ )
//         {
//             urls.append( args->arg(i) );
//         }
//         if( !urls.isEmpty() )
//         {
//             mainWindow->addReplayGainFiles( urls );
//             activateMainWindow = false;
//         }
//     }
//     else
//     {
//         QList<QUrl> urls;
//         for( int i=0; i<parser.positionalArguments().count(); i++ )
//         {
//             urls.append( args->arg(i) );
//         }
//         if( !urls.isEmpty() )
//             mainWindow->addConvertFiles( urls, profile, format, directory, notifyCommand );
//     }


    if( activateMainWindow )
        mainWindow->activateWindow();

    if( autostart )
        mainWindow->startConversion();

    if( first )
        mainWindow->startupChecks();

    first = false;

    return 0;
}

