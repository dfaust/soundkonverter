

#ifndef CONVERT_H
#define CONVERT_H

#include <KProcess>
#include <QList>
#include <QMap>
#include <QObject>
#include <QTimer>
#include <QWeakPointer>

class BackendPlugin;
class CDManager;
class Config;
class ConvertItem;
class FileList;
class FileListItem;
class Logger;
class ReplayGain;
class TagEngine;

class KJob;


/**
 * @short The conversion engine
 * @author Daniel Faust <hessijames@gmail.com>
 */
class Convert : public QObject
{
    Q_OBJECT
public:
    /** Constructor */
    Convert( Config *_config, FileList *_fileList, Logger *_logger );

    /** Destructor */
    virtual ~Convert();

    void cleanUp();

private:
    /** Copy the file with the file list item @p item to a temporary directory and download if necessary */
    void get( ConvertItem *item );

    /** Convert the file */
    void convert( ConvertItem *item );

    /** Apply a filter to the file after it has been decoded in convert() */
    void filter( ConvertItem *item );

    /** Encode the file after it has been decoded in convert() (and maybe filtered in filter()) */
    void encode( ConvertItem *item );

    /** Calculate replaygain tags of the file with the convert item @p item */
    void replaygain( ConvertItem *item );

    /** Write the tags of the file with the convert item @p item */
    void writeTags( ConvertItem *item );

    /** Run the userscript for the convert item @p item */
    void executeUserScript( ConvertItem *item );

    /** Decide, what to do next with out item @p item */
    void executeNextStep( ConvertItem *item );

    /** Make another try for @p item */
    void executeSameStep( ConvertItem *item );

    /** Remove item @p item and emit the state @p state */
    void remove( ConvertItem *item, int state = 0 );

    /** holds all active files */
    QList<ConvertItem*> items;

    /** holds all items that are waiting for album gain QMap< album name,convert items list > */
    QMap< QString, QList<ConvertItem*> > albumGainItems;

    Config *config;
    CDManager* cdManager;
    FileList *fileList;
    Logger* logger;
    QMap<int,QString> usedOutputNames;

    struct LogQueue {
        int id;
        BackendPlugin *plugin;
        QStringList messages;
    };

    QList<LogQueue> pluginLogQueue;

    QTimer updateTimer;

private slots:
    /** The file is being moved */
    void kioJobProgress( KJob *job, unsigned long percent );

    /** The file has been moved */
    void kioJobFinished( KJob *job );

    /** Get the process' output */
    void processOutput();

    /** The process has exited */
    void processExit( int exitCode, QProcess::ExitStatus exitStatus );

    /** A plugin has finished convertin a file */
    void pluginProcessFinished( int id, int exitCode );
    /** A plugin has something to log */
    void pluginLog( int id, const QString& message );

    /** sums up the progresses of all processes and sends it to the ProgressIndicator */
    void updateProgress();

public slots:
    // connected to FileList
    /** Add a new @p item to the item list and start */
    void add( FileListItem *item );
    /** Stop the item with the file list item @p item in the item list and remove it */
    void kill( FileListItem *item );
    /** the file list item @p item will get removed */
    void itemRemoved( FileListItem *item );

    /** Change the process priorities */
//     void priorityChanged( int );

signals:
    // connected to FileList
    /**
     * The conversion of an item has finished and the state is reported:
     * 0   = ok
     * -1  = error
     * 1   = aborted
     * 100 = backend needs configuration
     * 101 = disc is full
     * 102 = waiting for album gain
     */
    void finished( FileListItem *item, int state );
    /** The next track from the device can be ripped while the track is being encoded */
    void rippingFinished( const QString& device );

    // connected to Logger
    /** Tell the logger that the process has finished */
    void finishedProcess( int id, int state );

    // connected to ProgressIndicator
    void updateTime( float );
    void timeFinished( float );
};

#endif // CONVERT_H
