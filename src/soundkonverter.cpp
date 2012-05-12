/*
 * soundkonverter.cpp
 *
 * Copyright (C) 2007 Daniel Faust <hessijames@gmail.com>
 */
#include "soundkonverter.h"
#include "soundkonverterview.h"
#include "global.h"
#include "config.h"
#include "configdialog/configdialog.h"
#include "logger.h"
#include "logviewer.h"
#include "replaygainscanner/replaygainscanner.h"
#include "aboutplugins.h"

#include <taglib.h>

#include <KActionCollection>
#include <KApplication>
#include <KActionMenu>
#include <KLocale>
#include <KToolBar>
#include <KIcon>
#include <KStandardDirs>
#include <KMenu>
#include <QDir>

#if KDE_IS_VERSION(4,4,0)
    #include <KStatusNotifierItem>
#else
    #include <KSystemTrayIcon>
#endif

soundKonverter::soundKonverter()
    : KXmlGuiWindow(),
      replayGainScanner( 0 ),
      logViewer( 0 ),
      systemTray( 0 ),
      autoclose( false )
{
    // accept dnd
    setAcceptDrops(true);

    logger = new Logger( this );
    logger->log( 1000, i18n("This is soundKonverter %1").arg(SOUNDKONVERTER_VERSION_STRING) );

    logger->log( 1000, "\n" + i18n("Compiled with TagLib %1.%2.%3").arg(TAGLIB_MAJOR_VERSION).arg(TAGLIB_MINOR_VERSION).arg(TAGLIB_PATCH_VERSION) );
    #if (TAGLIB_MAJOR_VERSION == 1 && TAGLIB_MINOR_VERSION < 7)
    logger->log( 1000, "<span style=\"color:red;\">" + i18n("Reading/writing covers is not supported for flac and asf/wma files. TagLib 1.7 is needed for that.") + "</span>" );
    #endif

    config = new Config( logger, this );
    config->load();

    m_view = new soundKonverterView( logger, config, cdManager, this );
    connect( m_view, SIGNAL(signalConversionStarted()), this, SLOT(conversionStarted()) );
    connect( m_view, SIGNAL(signalConversionStopped(int)), this, SLOT(conversionStopped(int)) );
    connect( m_view, SIGNAL(progressChanged(const QString&)), this, SLOT(progressChanged(QString)) );
    connect( m_view, SIGNAL(showLog(int)), this, SLOT(showLogViewer(int)) );

    // tell the KXmlGuiWindow that this is indeed the main widget
    setCentralWidget( m_view );

    // then, setup our actions
    setupActions();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI( ToolBar | Keys | Save | Create );

    // clean up old files from previous soundKonverter versions
    if( config->data.app.configVersion < 1001 )
    {
        if( QFile::exists(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop") )
        {
            QFile::remove(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop");
            logger->log( 1000, i18n("Removing old file: %1").arg(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop") );
        }
        if( QFile::exists(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop") )
        {
            QFile::remove(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop");
            logger->log( 1000, i18n("Removing old file: %1").arg(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop") );
        }
    }

    // clean up log directory
    QDir dir( KStandardDirs::locateLocal("data","soundkonverter/log/") );
    dir.setFilter( QDir::Files | QDir::Writable );

    QStringList list = dir.entryList();

    for( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
        if( *it != "1000.log" && (*it).endsWith(".log") )
        {
            QFile::remove( dir.absolutePath() + "/" + (*it) );
            logger->log( 1000, i18n("Removing old file: %1").arg(dir.absolutePath()+"/"+(*it)) );
        }
    }

    // Check if new backends got installed and the backend settings can be optimized
    QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations();
    if( !optimizationList.isEmpty() )
    {
        CodecOptimizations *optimizationsDialog = new CodecOptimizations( optimizationList, this );
        connect( optimizationsDialog, SIGNAL(solutions(const QList<CodecOptimizations::Optimization>&)), config, SLOT(doOptimizations(const QList<CodecOptimizations::Optimization>&)) );
        optimizationsDialog->open();
    }
}

soundKonverter::~soundKonverter()
{
    if( logViewer )
        delete logViewer;

    if( replayGainScanner )
        delete replayGainScanner;

    if( systemTray )
        delete systemTray;
}

void soundKonverter::saveProperties( KConfigGroup& configGroup )
{
    Q_UNUSED(configGroup)

    m_view->killConversion();

    m_view->saveFileList( false );
}

void soundKonverter::showSystemTray()
{
    #if KDE_IS_VERSION(4,4,0)
        systemTray = new KStatusNotifierItem( this );
        systemTray->setCategory( KStatusNotifierItem::ApplicationStatus );
        systemTray->setStatus( KStatusNotifierItem::Active );
        systemTray->setIconByName( "soundkonverter" );
        systemTray->setToolTip( "soundkonverter", i18n("Waiting"), "" );
    #else
        systemTray = new KSystemTrayIcon( this );
        systemTray->setIcon( KIcon("soundkonverter") );
        systemTray->setToolTip( i18n("Waiting") );
        systemTray->show();
    #endif
}

void soundKonverter::addConvertFiles( const KUrl::List& urls, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand )
{
    m_view->addConvertFiles( urls, profile, format, directory, notifyCommand );
}

void soundKonverter::addReplayGainFiles( const KUrl::List& urls )
{
    showReplayGainScanner();
    replayGainScanner->addFiles( urls );
    replayGainScanner->activateWindow();
}

void soundKonverter::ripCd( const QString& device )
{
    m_view->showCdDialog( device != "auto" ? device : "" );
}

void soundKonverter::setupActions()
{
    KStandardAction::quit( this, SLOT(close()), actionCollection() );
    KStandardAction::preferences( this, SLOT(showConfigDialog()), actionCollection() );

    KAction *logviewer = actionCollection()->addAction("logviewer");
    logviewer->setText(i18n("View logs..."));
    logviewer->setIcon(KIcon("view-list-text"));
    connect( logviewer, SIGNAL(triggered()), this, SLOT(showLogViewer()) );

    KAction *replaygainscanner = actionCollection()->addAction("replaygainscanner");
    replaygainscanner->setText(i18n("Replay Gain tool..."));
    replaygainscanner->setIcon(KIcon("soundkonverter-replaygain"));
    connect( replaygainscanner, SIGNAL(triggered()), this, SLOT(showReplayGainScanner()) );

    KAction *aboutplugins = actionCollection()->addAction("aboutplugins");
    aboutplugins->setText(i18n("About plugins..."));
    aboutplugins->setIcon(KIcon("preferences-plugin"));
    connect( aboutplugins, SIGNAL(triggered()), this, SLOT(showAboutPlugins()) );

    KAction *add_files = actionCollection()->addAction("add_files");
    add_files->setText(i18n("Add files..."));
    add_files->setIcon(KIcon("audio-x-generic"));
    connect( add_files, SIGNAL(triggered()), m_view, SLOT(showFileDialog()) );

    KAction *add_folder = actionCollection()->addAction("add_folder");
    add_folder->setText(i18n("Add folder..."));
    add_folder->setIcon(KIcon("folder"));
    connect( add_folder, SIGNAL(triggered()), m_view, SLOT(showDirDialog()) );

    KAction *add_audiocd = actionCollection()->addAction("add_audiocd");
    add_audiocd->setText(i18n("Add CD tracks..."));
    add_audiocd->setIcon(KIcon("media-optical-audio"));
    connect( add_audiocd, SIGNAL(triggered()), m_view, SLOT(showCdDialog()) );

    KAction *add_url = actionCollection()->addAction("add_url");
    add_url->setText(i18n("Add url..."));
    add_url->setIcon(KIcon("network-workgroup"));
    connect( add_url, SIGNAL(triggered()), m_view, SLOT(showUrlDialog()) );

    KAction *add_playlist = actionCollection()->addAction("add_playlist");
    add_playlist->setText(i18n("Add playlist..."));
    add_playlist->setIcon(KIcon("view-media-playlist"));
    connect( add_playlist, SIGNAL(triggered()), m_view, SLOT(showPlaylistDialog()) );

    KAction *load = actionCollection()->addAction("load");
    load->setText(i18n("Load file list"));
    load->setIcon(KIcon("document-open"));
    connect( load, SIGNAL(triggered()), m_view, SLOT(loadFileList()) );

    KAction *save = actionCollection()->addAction("save");
    save->setText(i18n("Save file list"));
    save->setIcon(KIcon("document-save"));
    connect( save, SIGNAL(triggered()), m_view, SLOT(saveFileList()) );

    actionCollection()->addAction("start", m_view->start());
    actionCollection()->addAction("stop_menu", m_view->stopMenu());
}

void soundKonverter::showConfigDialog()
{
    ConfigDialog *dialog = new ConfigDialog( config, this/*, ConfigDialog::Page(configStartPage)*/ );
    connect( dialog, SIGNAL(updateFileList()), m_view, SLOT(updateFileList()) );

    dialog->resize( size() );
    dialog->exec();

    delete dialog;
}

void soundKonverter::showLogViewer( const int logId )
{
    if( !logViewer )
        logViewer = new LogViewer( logger, 0 );

    if( logId )
        logViewer->showLog( logId );

    logViewer->show();
    logViewer->raise();
}

void soundKonverter::showReplayGainScanner()
{
    if( !replayGainScanner )
        replayGainScanner = new ReplayGainScanner( config, logger, 0 );

    replayGainScanner->show();
    replayGainScanner->raise();
}

void soundKonverter::showAboutPlugins()
{
    AboutPlugins *dialog = new AboutPlugins( config, this );
    dialog->exec();
    dialog->deleteLater();
}

void soundKonverter::startConversion()
{
    m_view->startConversion();
}

void soundKonverter::loadAutosaveFileList()
{
    m_view->loadAutosaveFileList();
}

void soundKonverter::conversionStarted()
{
    if( systemTray )
    {
        #if KDE_IS_VERSION(4,4,0)
            systemTray->setToolTip( "soundkonverter", i18n("Converting") + ": 0%", "" );
        #else
            systemTray->setToolTip( i18n("Converting") + ": 0%" );
        #endif
    }
}

void soundKonverter::conversionStopped( int state )
{
    if( autoclose && state != 1 /*&& !m_view->isVisible()*/ )
        KApplication::kApplication()->quit(); // close app on conversion stop unless the conversion was stopped by the user or the window is shown

    if( systemTray )
    {
        #if KDE_IS_VERSION(4,4,0)
            systemTray->setToolTip( "soundkonverter", i18n("Finished"), "" );
        #else
            systemTray->setToolTip( i18n("Finished") );
        #endif
    }
}

void soundKonverter::progressChanged( const QString& progress )
{
    setWindowTitle( progress + " - soundKonverter" );

    if( systemTray )
    {
        #if KDE_IS_VERSION(4,4,0)
            systemTray->setToolTip( "soundkonverter", i18n("Converting") + ": " + progress, "" );
        #else
            systemTray->setToolTip( i18n("Converting") + ": " + progress );
        #endif
    }
}


#include "soundkonverter.moc"
