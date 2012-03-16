/*
 * soundkonverterview.h
 *
 * Copyright (C) 2007 Daniel Faust <hessijames@gmail.com>
 */
#ifndef _SOUNDKONVERTERVIEW_H_
#define _SOUNDKONVERTERVIEW_H_

#include <QWidget>
#include <KUrl>

class KPushButton;
class QMenu;
class KAction;
class KActionMenu;
class QToolButton;

class ProgressIndicator;
class ComboButton;
class Config;
class Logger;
class CDManager;
class FileList;
class OptionsLayer;

// class QPainter;
// class KUrl;

/**
 * This is the main view class for soundKonverter.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */

class soundKonverterView : public QWidget
{
    Q_OBJECT
public:
    /** Default constructor */
    soundKonverterView( Logger *_logger, Config *_config, CDManager *_cdManager, QWidget *parent );

    /** Destructor */
    virtual ~soundKonverterView();

    void addConvertFiles( const KUrl::List& urls, QString _profile, QString _format, const QString& directory, const QString& notifyCommand = "" );
    void loadAutosaveFileList();

    KAction *start() { return startAction; }
    KActionMenu *stopMenu() { return stopActionMenu; }

    void startConversion();
    void killConversion();

signals:
    /** Use this signal to change the content of the statusbar */
//     void signalChangeStatusbar(const QString& text);

    /** Use this signal to change the content of the caption */
//     void signalChangeCaption( const QString& text );

public slots:
    void showCdDialog( const QString& device = "", bool intern = true );
    void loadFileList( bool user = true );
    void saveFileList( bool user = true );
    void updateFileList();

private slots:
    void addClicked( int index );
    void showFileDialog();
    void showDirDialog();
    void showUrlDialog();
    void showPlaylistDialog();

    // connected to fileList
    /** The count of items in the file list has changed to @p count */
    void fileCountChanged( int count );
    /** The conversion has started */
    void conversionStarted();
    /** The conversion has stopped */
    void conversionStopped( int state );
    /** Conversion will continue/stop after current files have been converted */
    void queueModeChanged( bool enabled );

private:
    Config *config;
    Logger *logger;
    CDManager *cdManager;

    FileList *fileList;
    OptionsLayer *optionsLayer;

    /** The combobutton for adding files */
    ComboButton *cAdd;

    /** The button to start the conversion */
    KPushButton *pStart;
    /** Tha start action */
    KAction *startAction;

    /** The button to stop the conversion */
    KPushButton *pStop;
    /** The menu for the stop button */
    KActionMenu *stopActionMenu;
    KAction *killAction;
    KAction *stopAction;
    KAction *continueAction;

    /** Displays the current progress */
    ProgressIndicator *progressIndicator;

signals:
    void progressChanged( const QString& progress );
    void signalConversionStarted();
    void signalConversionStopped( int state );
};

#endif // _soundKonverterVIEW_H_
