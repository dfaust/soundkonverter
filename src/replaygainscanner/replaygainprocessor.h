

#ifndef REPLAYGAINPROCESSOR_H
#define REPLAYGAINPROCESSOR_H

#include "replaygainfilelistitem.h"

#include <QList>
#include <QObject>
#include <QTimer>
#include <QTime>
#include <QWeakPointer>

class ReplayGainPlugin;
class Config;
class Logger;
class ReplayGainFileList;


class ReplayGainProcessorItem
{
public:
    ReplayGainProcessorItem( ReplayGainFileListItem *item );
    ~ReplayGainProcessorItem();

    /** a reference to the file list item, in case it's a convert item */
    ReplayGainFileListItem *fileListItem;

    /** a list of replaygain pipes that are suitable for this item */
    QList<ReplayGainPipe> replaygainPipes;
    /** number of the current attempt to add replaygain (for the pipes lists) */
    int take;

    ReplayGainPlugin::ApplyMode mode;

    /** the active plugin */
    ReplayGainPlugin *backendPlugin;
    /** the id from the active plugin (-1 if false) */
    int backendID;
    /** has the process been killed on purpose? */
    bool killed;

    /** the id with that the item is registered at the logger */
    int logID;

    int time;

    QTime progressedTime;
};


class ReplayGainProcessor : public QObject
{
    Q_OBJECT
public:
    ReplayGainProcessor( Config *_config, ReplayGainFileList *_fileList, Logger *_logger );
    ~ReplayGainProcessor();

private:
    /** Calculate replaygain tags of the file with the convert item @p item */
    void replaygain( ReplayGainProcessorItem *item );

    /** Remove item @p item and emit the state @p state */
    void remove( ReplayGainProcessorItem *item, ReplayGainFileListItem::ReturnCode returnCode = ReplayGainFileListItem::Succeeded );

    /** holds all active files */
    QList<ReplayGainProcessorItem*> items;

    Config *config;
    ReplayGainFileList *fileList;
    Logger *logger;

    struct LogQueue {
        int id;
        BackendPlugin *plugin;
        QStringList messages;
    };

    QList<LogQueue> pluginLogQueue;

    QTimer updateTimer;

private slots:
    /** A plugin has finished converting a file */
    void pluginProcessFinished( int id, int exitCode );
    /** A plugin has something to log */
    void pluginLog( int id, const QString& message );

    /** sums up the progresses of all processes and sends it to the ProgressIndicator */
    void updateProgress();

public slots:
    // connected to ReplayGainFileList
    /** Add a new @p fileListItem to the item list and start */
    void add( ReplayGainFileListItem *fileListItem, ReplayGainPlugin::ApplyMode mode );
    /** Stop the item with the file list item @p fileListItem in the item list and remove it */
    void kill( ReplayGainFileListItem *fileListItem );

signals:
    // connected to FileList
    /** The conversion of an item has finished and the state is reported */
    void finished( ReplayGainFileListItem *fileListItem, ReplayGainFileListItem::ReturnCode returnCode );

    // connected to Logger
    /** Tell the logger that the process has finished */
    void finishedProcess( int id, bool succeeded );

    // connected to ProgressIndicator
    void updateTime( float timeProgress );
    void timeFinished( float timeDelta );
};

#endif // REPLAYGAINPROCESSOR_H
