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

/*
 * soundkonverter.h
 *
 * Copyright (C) 2007 Daniel Faust <hessijames@gmail.com>
 */
#ifndef SOUNDKONVERTER_H
#define SOUNDKONVERTER_H


#include <KXmlGuiWindow>
#include <KUrl>
#include <kdeversion.h>

class soundKonverterView;
class KToggleAction;
class KUrl;
class Config;
class Logger;
class LogViewer;
class CDManager;
class ReplayGainScanner;

#if KDE_IS_VERSION(4,4,0)
    class KStatusNotifierItem;
#else
    class KSystemTrayIcon;
#endif


/**
 * This class serves as the main window for soundKonverter.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class soundKonverter : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundKonverter();

    /** Default Destructor */
    ~soundKonverter();

    virtual void saveProperties( KConfigGroup& configGroup );

    void showSystemTray();
    void addConvertFiles( const KUrl::List& urls, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand );
    void addReplayGainFiles( const KUrl::List& urls );
    bool ripCd( const QString& device, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand );
    void setAutoClose( bool enabled ) { autoclose = enabled; }
    void startConversion();
    void loadAutosaveFileList();
    void startupChecks();

private slots:
    void showConfigDialog();
    void showLogViewer( const int logId = 0 );
    void showReplayGainScanner();
    void replayGainScannerClosed();
    void showMainWindow();
    void showAboutPlugins();
    void progressChanged( const QString& progress );

    /** The conversion has started */
    void conversionStarted();
    /** The conversion has stopped */
    void conversionStopped( bool failed );

private:
    Config *config;
    Logger *logger;
    CDManager *cdManager;
    QWeakPointer<ReplayGainScanner> replayGainScanner;

    soundKonverterView *m_view;
    LogViewer *logViewer;

    #if KDE_IS_VERSION(4,4,0)
        KStatusNotifierItem *systemTray;
    #else
        KSystemTrayIcon *systemTray;
    #endif

    /// exit soundkonverter after all files have been converted
    bool autoclose;

    void setupActions();

};

#endif // _SOUNDKONVERTER_H_
