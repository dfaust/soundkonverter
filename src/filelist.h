

#ifndef FILELIST_H
#define FILELIST_H

#include <QTreeWidget>
#include "filelistitem.h"

#include <QTime>
#include <QDebug>

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
    virtual ~FileList();

    FileListItem *topLevelItem( int index ) const { return static_cast<FileListItem*>( QTreeWidget::topLevelItem(index) ); }

    void setOptionsLayer( OptionsLayer *_optionsLayer ) { optionsLayer = _optionsLayer; }

    void load( bool user = false );
    void save( bool user = false );
    void updateAllItems();
    void updateItem( FileListItem *item );

private:
    /** Lists all file in a directory and adds them to the file list, if fast is false. The number of listed files is returned */
    int listDir( const QString& directory, const QStringList& filter, bool recursive, int conversionOptionsId, bool fast = false, int count = 0 );
    /** A progressbar, that is shown, when a directory is added recursive */
    QProgressBar *pScanStatus;
    /** Update timer for the scan status */
    QTime tScanStatus;

    int TimeCount;
    QTime Time;

    void convertNextItem();
    int waitingCount();
    int convertingCount();

//     qulonglong spaceLeftForDirectory( const QString& dir );

    QList<FileListItem*> selectedFiles;

    void dragEnterEvent( QDragEnterEvent *event );
    void dropEvent( QDropEvent *event );

    void resizeEvent( QResizeEvent *event );

    bool queue;

//     QStringList fullDiscs; // a list of mount points with volumes that don't have enougth space left

    /**
     * A command that should be executed after the conversion of a file is complete
     * %i will be replaced by the input file path
     * %o "    "  "        "  "   output "   "
     */
    QString notifyCommand;

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

    void updateItems( QList<FileListItem*> items );

    // connected to OptionsEditor
    void selectPreviousItem();
    void selectNextItem();

public slots:
    // connected to soundKonverterView
    void addFiles( const KUrl::List& fileList, ConversionOptions *conversionOptions, const QString& notifyCommand = "", const QString& _codecName = "", int conversionOptionsId = -1, FileListItem *after = 0, bool enabled = false );
    void addDir( const KUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions );
//     void addTracks( int cdId, QList< int > trackList, ConversionOptions* conversionOptions );
    void addTracks( const QString& device, QList<int> trackList, int tracks, QList<TagData*> tagList, ConversionOptions *conversionOptions );
    void startConversion();
    void killConversion();
    void stopConversion();
    void continueConversion();

    // connected to Convert
    /**
     * The conversion of an item has finished and the state is reported:
     * 0   = ok
     * -1  = error
     * 1   = aborted
     * 100 = backend needs configuration
     * 101 = disc is full
     */
    void itemFinished( FileListItem*, int );
    /** The ripping of a track has finished, so the device is free for ripping the next track */
    void rippingFinished( const QString& device );

signals:
    // connected to ProgressIndicator
    void timeChanged( float time );
    void finished( float time );
    // connected to soundKonverterView
    void fileCountChanged( int count );
    void conversionStarted();
    void conversionStopped( int state );
    void queueModeChanged( bool enabled );

//     void stopClicked();
//     void continueClicked();

    // connected to Convert
    void convertItem( FileListItem* );
    void killItem( FileListItem* );

    // connected to OptionsEditor
    void editItems( QList<FileListItem*> );
    void setPreviousItemEnabled( bool );
    void setNextItemEnabled( bool );
    void itemRemoved( FileListItem* );
};

#endif // FILELIST_H
