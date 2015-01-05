
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
#include <QApplication>
#include <KActionMenu>
#include <KLocalizedString>
#include <KToolBar>
#include <QIcon>
#include <QStandardPaths>
#include <QMenu>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include <KStatusNotifierItem>

SoundKonverter::SoundKonverter() :
    KXmlGuiWindow(),
    autoCloseEnabled(false)
{
    qDebug() << "soundKonverter";

    setAcceptDrops(true);

    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    logger = new Logger(this);
    logger->log(1000, i18n("This is soundKonverter %1").arg(SOUNDKONVERTER_VERSION_STRING));

    logger->log(1000, "\n" + i18n("Compiled with TagLib %1.%2.%3").arg(TAGLIB_MAJOR_VERSION).arg(TAGLIB_MINOR_VERSION).arg(TAGLIB_PATCH_VERSION));

    config = new Config(logger, this);
    config->load();

    soundKonverterView = new SoundKonverterView(logger, config, this);
    connect(soundKonverterView, SIGNAL(signalConversionStarted()),       this, SLOT(conversionStarted()));
    connect(soundKonverterView, SIGNAL(signalConversionStopped(bool)),   this, SLOT(conversionStopped(bool)));
    connect(soundKonverterView, SIGNAL(progressChanged(const QString&)), this, SLOT(progressChanged(const QString&)));
    connect(soundKonverterView, SIGNAL(showLog(int)),                    this, SLOT(showLogViewer(int)));

    setCentralWidget(soundKonverterView);

    setupActions();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and asks the mainwindow to automatically save settings if changed:
    // window size, toolbar position, icon size, etc.
    setupGUI(QSize(70*fontHeight,45*fontHeight), ToolBar | Keys | Save | Create);
}

SoundKonverter::~SoundKonverter()
{
    if( logViewer )
        delete logViewer;

    if( replayGainScanner )
        delete replayGainScanner;

    if( systemTray )
        delete systemTray;
}

void SoundKonverter::saveProperties( KConfigGroup& configGroup )
{
    Q_UNUSED(configGroup)

    soundKonverterView->killConversion();

    soundKonverterView->saveFileList( false );
}

void SoundKonverter::showSystemTray()
{
    systemTray = new KStatusNotifierItem( this );
    systemTray->setCategory( KStatusNotifierItem::ApplicationStatus );
    systemTray->setStatus( KStatusNotifierItem::Active );
    systemTray->setIconByName( "soundkonverter" );
    systemTray->setToolTip( "soundkonverter", i18n("Waiting"), "" );
}

void SoundKonverter::addConvertFiles( const QList<QUrl>& urls, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand )
{
    soundKonverterView->addConvertFiles( urls, profile, format, directory, notifyCommand );
}

void SoundKonverter::addReplayGainFiles( const QList<QUrl>& urls )
{
    showReplayGainScanner();
    replayGainScanner->addFiles( urls );
}

bool SoundKonverter::ripCd( const QString& device, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand )
{
    return soundKonverterView->showCdDialog( device != "auto" ? device : "", profile, format, directory, notifyCommand );
}

void SoundKonverter::setupActions()
{
    KStandardAction::quit( this, SLOT(close()), actionCollection() );
    KStandardAction::preferences( this, SLOT(showConfigDialog()), actionCollection() );

    QAction *logviewer = actionCollection()->addAction("logviewer");
    logviewer->setText(i18n("View logs..."));
    logviewer->setIcon(QIcon::fromTheme("view-list-text"));
    connect( logviewer, SIGNAL(triggered()), this, SLOT(showLogViewer()) );

    QAction *replaygainscanner = actionCollection()->addAction("replaygainscanner");
    replaygainscanner->setText(i18n("Replay Gain tool..."));
    replaygainscanner->setIcon(QIcon::fromTheme("soundkonverter-replaygain"));
    connect( replaygainscanner, SIGNAL(triggered()), this, SLOT(showReplayGainScanner()) );

    QAction *aboutplugins = actionCollection()->addAction("aboutplugins");
    aboutplugins->setText(i18n("About plugins..."));
    aboutplugins->setIcon(QIcon::fromTheme("preferences-plugin"));
    connect( aboutplugins, SIGNAL(triggered()), this, SLOT(showAboutPlugins()) );

    QAction *add_files = actionCollection()->addAction("add_files");
    add_files->setText(i18n("Add files..."));
    add_files->setIcon(QIcon::fromTheme("audio-x-generic"));
    connect( add_files, SIGNAL(triggered()), soundKonverterView, SLOT(showFileDialog()) );

    QAction *add_folder = actionCollection()->addAction("add_folder");
    add_folder->setText(i18n("Add folder..."));
    add_folder->setIcon(QIcon::fromTheme("folder"));
    connect( add_folder, SIGNAL(triggered()), soundKonverterView, SLOT(showDirDialog()) );

    QAction *add_audiocd = actionCollection()->addAction("add_audiocd");
    add_audiocd->setText(i18n("Add CD tracks..."));
    add_audiocd->setIcon(QIcon::fromTheme("media-optical-audio"));
    connect( add_audiocd, SIGNAL(triggered()), soundKonverterView, SLOT(showCdDialog()) );

    QAction *add_url = actionCollection()->addAction("add_url");
    add_url->setText(i18n("Add url..."));
    add_url->setIcon(QIcon::fromTheme("network-workgroup"));
    connect( add_url, SIGNAL(triggered()), soundKonverterView, SLOT(showUrlDialog()) );

    QAction *add_playlist = actionCollection()->addAction("add_playlist");
    add_playlist->setText(i18n("Add playlist..."));
    add_playlist->setIcon(QIcon::fromTheme("view-media-playlist"));
    connect( add_playlist, SIGNAL(triggered()), soundKonverterView, SLOT(showPlaylistDialog()) );

    QAction *load = actionCollection()->addAction("load");
    load->setText(i18n("Load file list"));
    load->setIcon(QIcon::fromTheme("document-open"));
    connect( load, SIGNAL(triggered()), soundKonverterView, SLOT(loadFileList()) );

    QAction *save = actionCollection()->addAction("save");
    save->setText(i18n("Save file list"));
    save->setIcon(QIcon::fromTheme("document-save"));
    connect( save, SIGNAL(triggered()), soundKonverterView, SLOT(saveFileList()) );

    actionCollection()->addAction("start", soundKonverterView->getStartAction());
    actionCollection()->addAction("stop_menu", soundKonverterView->getStopActionMenu());
}

void SoundKonverter::showConfigDialog()
{
    ConfigDialog *dialog = new ConfigDialog( config, this/*, ConfigDialog::Page(configStartPage)*/ );
    connect( dialog, SIGNAL(updateFileList()), soundKonverterView, SLOT(updateFileList()) );

    dialog->resize( size() );
    dialog->exec();

    delete dialog;
}

void SoundKonverter::showLogViewer(const int logId)
{
    if( !logViewer )
        logViewer = new LogViewer(logger, 0);

    if( logId )
        logViewer->showLog(logId);

    logViewer->show();
    logViewer->raise();
}

void SoundKonverter::showReplayGainScanner()
{
    if( !replayGainScanner )
    {
        replayGainScanner = new ReplayGainScanner( config, logger, !isVisible(), 0 );
        connect( replayGainScanner, SIGNAL(finished()), this, SLOT(replayGainScannerClosed()) );
        connect( replayGainScanner, SIGNAL(showMainWindow()), this, SLOT(showMainWindow()) );
    }

    replayGainScanner->setAttribute( Qt::WA_DeleteOnClose );

    replayGainScanner->show();
    replayGainScanner->raise();
    replayGainScanner->activateWindow();
}

void SoundKonverter::replayGainScannerClosed()
{
    if( !isVisible() )
        qApp->quit();
}

void SoundKonverter::showMainWindow()
{
    show();
}

void SoundKonverter::showAboutPlugins()
{
    AboutPlugins *dialog = new AboutPlugins( config, this );
    dialog->exec();
    dialog->deleteLater();
}

void SoundKonverter::startConversion()
{
    soundKonverterView->startConversion();
}

void SoundKonverter::loadAutosaveFileList()
{
    soundKonverterView->loadAutosaveFileList();
}

void SoundKonverter::startupChecks()
{
    // check if codec plugins could be loaded
    if( config->pluginLoader()->getAllCodecPlugins().count() == 0 )
    {
        QMessageBox::critical(this, "soundKonverter", i18n("No codec plugins could be loaded. Without codec plugins soundKonverter can't work.\nThis problem can have two causes:\n1. You just installed soundKonverter and the KDE System Configuration Cache is not up-to-date, yet.\nIn this case, run kbuildsycoca4 and restart soundKonverter to fix the problem.\n2. Your installation is broken.\nIn this case try reinstalling soundKonverter."));
    }

//     // remove old KDE4 action menus created by soundKonverter 0.3 - don't change the paths, it's what soundKonverter 0.3 used
//     if( config->data.app.configVersion < 1001 )
//     {
//         if( QFile::exists(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop") )
//         {
//             QFile::remove(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop");
//             logger->log( 1000, i18n("Removing old file: %1").arg(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/convert_with_soundkonverter.desktop") );
//         }
//         if( QFile::exists(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop") )
//         {
//             QFile::remove(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop");
//             logger->log( 1000, i18n("Removing old file: %1").arg(QDir::homePath()+"/.kde4/share/kde4/services/ServiceMenus/add_replaygain_with_soundkonverter.desktop") );
//         }
//     }

    // clean up log directory
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/log/");
    dir.setFilter(QDir::Files | QDir::Writable);

    foreach( const QString file, dir.entryList() )
    {
        if( file != "1000.log" && file.endsWith(".log") )
        {
            QFile::remove( dir.absolutePath() + "/" + file );
            logger->log( 1000, i18n("Removing old file: %1").arg(dir.absolutePath()+"/"+file) );
        }
    }

    // check if new backends got installed and the backend settings can be optimized
    QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations();
    if( !optimizationList.isEmpty() )
    {
        CodecOptimizations *optimizationsDialog = new CodecOptimizations( optimizationList, this );
        connect( optimizationsDialog, SIGNAL(solutions(const QList<CodecOptimizations::Optimization>&)), config, SLOT(doOptimizations(const QList<CodecOptimizations::Optimization>&)) );
        optimizationsDialog->open();
    }
}

void SoundKonverter::conversionStarted()
{
    if( systemTray )
    {
        systemTray->setToolTip( "soundkonverter", i18n("Converting") + ": 0%", "" );
    }
}

void SoundKonverter::conversionStopped( bool failed )
{
    if( autoCloseEnabled && !failed /*&& !m_view->isVisible()*/ )
        qApp->quit(); // close app on conversion stop unless the conversion was stopped by the user or the window is shown

    if( systemTray )
    {
        systemTray->setToolTip( "soundkonverter", i18n("Finished"), "" );
    }
}

void SoundKonverter::progressChanged( const QString& progress )
{
    setWindowTitle( progress + " - soundKonverter" );

    if( systemTray )
    {
        systemTray->setToolTip( "soundkonverter", i18n("Converting") + ": " + progress, "" );
    }
}


#include "soundkonverter.moc"
