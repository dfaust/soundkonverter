
#ifndef REPLAYGAINFILELIST_H
#define REPLAYGAINFILELIST_H

#include "replaygainfilelistitem.h"
#include "core/replaygainplugin.h"

#include <KUrl>
#include <QTime>

class Config;
class Logger;
class ConversionOptions;
class QProgressBar;
class KAction;
// class QMenu;


// NOTE Currently only one plugin can run at a time. If there should run more than one plugin simultaniously in the future seom things have to be rewritten.
// - Like identifying items not only by their processId but also by the plugin,
// - Qeue management


/**
 * @short The file list of the Replay Gain scanner
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class ReplayGainFileList : public QTreeWidget
{
    Q_OBJECT
public:
    enum Columns {
        Column_File     = 0,
        Column_Track    = 1,
        Column_Album    = 2
    };

    /** Constructor */
    ReplayGainFileList( Config *_config, Logger *_logger, QWidget *parent=0 );

    /** Destructor */
    ~ReplayGainFileList();

    ReplayGainFileListItem *topLevelItem( int index ) const { return static_cast<ReplayGainFileListItem*>( QTreeWidget::topLevelItem(index) ); }

    void addFiles( const KUrl::List& fileList, const QString& _codecName = "" );

    void startProcessing( ReplayGainPlugin::ApplyMode _mode );
    void removeAllReplayGain();
    void cancelProcess();

private:
    Config *config;
    Logger *logger;

    /** Counts all files in a directory */
    int countDir( const QString& directory, bool recursive, int count = 0 );
    /** Lists all files in a directory and adds them to the file list */
    int listDir( const QString& directory, const QStringList& filter, bool recursive, int count = 0 );
    /** A progressbar, that is shown, when a directory is added recursive */
    QProgressBar *pScanStatus;
    /** Update timer for the scan status */
    QTime tScanStatus;

    void dragEnterEvent( QDragEnterEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dropEvent( QDropEvent *event );

    void resizeEvent( QResizeEvent *event );

    bool queue;             // NOTE currently always true
    ReplayGainPlugin::ApplyMode mode;

    QTreeWidgetItem *lastAlbumItem;

    QMenu *contextMenu;
    KAction *collapseAction;
    KAction *expandAction;
//     KAction *processAddAction;
//     KAction *processRemoveAction;
//     KAction *killAction;
    KAction *moveAction;
    KAction *removeAction;

    void processNextItem();
    int waitingCount();
    int processingCount();

public slots:
    void addDir( const KUrl& directory, bool recursive, const QStringList& codecList );
    void updateItem( ReplayGainFileListItem *item, bool initialUpdate = false );

private slots:
    void showContextMenu( const QPoint& point );
//     void processAddSelectedItems();
//     void processRemoveSelectedItems();
//     void killSelectedItems();
    void moveSelectedItems();
    void removeSelectedItems();

    // connected to ReplayGainProcessor
    void itemFinished( ReplayGainFileListItem *item, ReplayGainFileListItem::ReturnCode returnCode );

signals:
    // connected to ProgressIndicator
    void timeChanged( float timeDelta );
    void finished( bool reset );

    // connected to ReplayGainScanner
    void processStarted();
    void processStopped();

    // connected to ReplayGainProcessor
    void processItem( ReplayGainFileListItem *item, ReplayGainPlugin::ApplyMode mode );
    void killItem( ReplayGainFileListItem *item );
};

#endif // REPLAYGAINFILELIST_H
