

#ifndef FILELIST_H
#define FILELIST_H

#include <QTreeWidget>
#include "filelistitem.h"

#include <QTime>
// #include <QDebug>

class FileListItem;
class Config;
class Logger;
class TagEngine;
class OptionsEditor;
class OptionsLayer;
class ConversionOptions;

class QMenu;
class KAction;
class QProgressBar;

/**
 * @short The file list
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class FileList : public QTreeWidget
{
    Q_OBJECT
public:
    enum Columns {
        Column_State    = 0,
        Column_Input    = 1,
        Column_Output   = 2,
        Column_Quality  = 3
    };

    /** Constructor */
    FileList( Logger *_logger, Config *_config, QWidget *parent = 0 );

    /** Destructor */
    ~FileList();

    FileListItem *topLevelItem( int index ) const { return static_cast<FileListItem*>( QTreeWidget::topLevelItem(index) ); }
//     FileListItem *itemAbove( FileListItem* item ) const { return static_cast<FileListItem*>( QTreeWidget::itemAbove(item) ); }
//     FileListItem *itemBelow( FileListItem* item ) const { return static_cast<FileListItem*>( QTreeWidget::itemBelow(item) ); }

    void setOptionsLayer( OptionsLayer *_optionsLayer ) { optionsLayer = _optionsLayer; }

    void load( bool user = false );
    void updateAllItems(); // Gets triggered if the configuration changes and the file list needs to be updated
    void updateItem( FileListItem *item );

    bool waitForAlbumGain( FileListItem *item );

private:
    /** Counts all files in a directory */
    int countDir( const QString& directory, bool recursive, int count = 0 );
    /** Lists all files in a directory and adds them to the file list */
    int listDir( const QString& directory, const QStringList& filter, bool recursive, int conversionOptionsId, int count = 0 );
    /** A progressbar, that is shown, when a directory is added recursive */
    QProgressBar *pScanStatus;
    /** Update timer for the scan status */
    QTime tScanStatus;

// debug
//     int TimeCount;
//     QTime Time;

    void convertNextItem();
    int waitingCount();
    int convertingCount();

//     qulonglong spaceLeftForDirectory( const QString& dir );
//     QStringList fullDiscs; // a list of mount points with volumes that don't have enougth space left

    QList<FileListItem*> selectedFiles;

    void dragEnterEvent( QDragEnterEvent *event );
    void dropEvent( QDropEvent *event );

    void resizeEvent( QResizeEvent *event );

    bool queue;

    Logger *logger;
    Config *config;
    TagEngine *tagEngine;
    OptionsEditor *optionsEditor;
    OptionsLayer *optionsLayer;

    QMenu *contextMenu;
    KAction *editAction;
    KAction *startAction;
    KAction *stopAction;
    KAction *removeAction;
//     KAction* paste;

private slots:
    void showContextMenu( const QPoint& point );
    void showOptionsEditorDialog();
    void removeSelectedItems();
    void convertSelectedItems();
    void killSelectedItems();
    void itemsSelected();

    // connected to OptionsEditor
    void updateItems( QList<FileListItem*> items );
    void selectPreviousItem();
    void selectNextItem();

    void showLogClicked( const QString& logIdString );

public slots:
    // connected to soundKonverterView
    void addFiles( const KUrl::List& fileList, ConversionOptions *conversionOptions, const QString& notifyCommand = "", const QString& _codecName = "", int conversionOptionsId = -1 );
    void addDir( const KUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions );
    void addTracks( const QString& device, QList<int> trackList, int tracks, QList<TagData*> tagList, ConversionOptions *conversionOptions, const QString& notifyCommand = "" );
    void startConversion();
    void killConversion();
    void stopConversion();
    void continueConversion();

    void save( bool user = false );

    // connected to Convert
    /** The conversion of an item has finished and the state is reported */
    void itemFinished( FileListItem *item, FileListItem::ReturnCode returnCode, bool waitingForAlbumGain = false );
    /** The ripping of a track has finished, so the device is free for ripping the next track */
    void rippingFinished( const QString& device );

signals:
    // connected to ProgressIndicator
    void timeChanged( float timeDelta );
    void finished( bool );
    // connected to soundKonverterView
    void fileCountChanged( int count );
    void conversionStarted();
    void conversionStopped( bool failed );
    void queueModeChanged( bool enabled );
    void showLog( const int logId );

//     void stopClicked();
//     void continueClicked();

    // connected to Convert
    void convertItem( FileListItem* );
    void killItem( FileListItem* );
    void replaygainItems( QList<FileListItem*> );

    // connected to OptionsEditor
    void editItems( QList<FileListItem*> );
    void setPreviousItemEnabled( bool );
    void setNextItemEnabled( bool );
    void itemRemoved( FileListItem* );
};

#endif // FILELIST_H
