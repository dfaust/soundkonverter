

#ifndef CONVERT_H
#define CONVERT_H

#include "pluginloader.h"

//#include <kio/jobclasses.h>
// #include <kio/job.h>
// #include <kio/copyjob.h>
#include <kio/job.h>
#include <kio/jobclasses.h>

#include <QObject>
#include <QList>
#include <QTime>
#include <QTimer>

#include <KProcess>
#include <KTemporaryFile>

class Config;
class TagEngine;
class CDManager;
class FileList;
class FileListItem;
class ReplayGain;
class Logger;

//class KProcess;

/*class RatedListItem
{
public:
    RatedListItem( CodecPlugin *_plugin1, CodecPlugin *_plugin2, int _rating ) { plugin1 = _plugin1; plugin2 = _plugin2; rating = _rating; }
    ~RatedListItem() {}

    CodecPlugin *plugin1;
    CodecPlugin *plugin2;
    int rating;

    bool operator<(RatedListItem const& b)
    {
        return this->rating < b.rating;
    }
};*/

/**
 * @short The items for the conversion (for every active file)
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class ConvertItem
{
public:
    /**
     * A list of flags for knowing what to do
     */
    enum Mode {
        get               = 0x0001, // Copy the file to tmp
        //rip               = 0x0002, // Rip the file
        convert           = 0x0004, // Convert the file
        decode            = 0x0008, // Decode the file
        encode            = 0x0010, // Encode the file
        replaygain        = 0x0020, // Apply replaygain
        bpm               = 0x0040, // Apply replaygain
        write_tags        = 0x0080, // Write the tags to the file
        put               = 0x0100, // Copy the file to the destination directory
        execute_userscript= 0x0200 // Run the user script
        //remove_temp       = 0x0400  // Remove the downloaded temp file
    };

    /** Default Constructor */
    ConvertItem();

    /** Constructor, @p item A pointer to the file list item */
    ConvertItem( FileListItem *item );

    /** Destructor */
    virtual ~ConvertItem();

    /** a reference to the file list item */
    FileListItem *fileListItem;

    /** a list of conversion pipes that are suitable for this item */
    QList<ConversionPipe> conversionPipes;
    /** a list of conversion pipes that are suitable for this item */
    QList<ReplayGainPipe> replaygainPipes;
    /** number of the current attempt to convert the file/add replaygain (for the pipes lists) */
    int take;

    /** for the conversion and moving the file to a temporary place */
    KProcess *process;
    /** for moving the file to the temporary directory */
    KIO::FileCopyJob *kioCopyJob;
    /** the id from the plugin (-1 if false) */
    int convertID;
    /** the id from the plugin (-1 if false) */
    int replaygainID;
    /** the active plugin */
    BackendPlugin *convertPlugin;
    /** if the item gets decoded first, cache for 2nd conversion step */
    CodecPlugin *encodePlugin;
    /** the replay gain plugin */
    ReplayGainPlugin *replaygainPlugin;
    /** holds if the process has been killed on purpose  */
    bool killed;

    /** the url from fileListItem or the download temp file */
    KUrl inputUrl;
    /** the path and the name of the output file */
    KUrl outputUrl;
    /** the downloaded input file */
    KUrl tempInputUrl;
    /** the temp file for the pipe */
    KUrl tempConvertUrl;
    
//     void generateTempUrl( const QString& extension );
    KUrl generateTempUrl( const QString& prefix, const QString& extension );

    /** what shall we do with the file? */
    Mode mode;
    /** and what are we doing with the file? */
    Mode state;

    /** the id with that the item is registered at the logger */
    int logID;

    /** the time from the file list item splitted up */
    float getTime;
    float convertTime;
    float decodeTime;
    float encodeTime;
    float replaygainTime;
    float bpmTime;
    
    float finishedTime; // the time of the finished conversion steps
    
    void updateTimes();

    /** the current conversion progress */
    float progress;
    
    QTime progressedTime;
};


/**
 * @short The conversion engine
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
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
    
    /** Encode the file after it has been decoded in convert() */
    void encode( ConvertItem *item );

    /** Calculate replaygain tags of the file with the convert item @p item */
    void replaygain( ConvertItem *item );

    /** Write the tags of the file with the convert item @p item */
    void writeTags( ConvertItem *item );

    /** Copy the file with the convert item @p item to it's destination directory */
    void put( ConvertItem *item );

    /** Run the userscript for the convert item @p item */
    void executeUserScript( ConvertItem *item );

    /** Decide, what to do next with out item @p item */
    void executeNextStep( ConvertItem *item );

    /** Make another try for @p item */
    void executeSameStep( ConvertItem *item );

    /** Remove item @p item and emit the state @p state */
    void remove( ConvertItem *item, int state = 0 );

    /** build all possible conversion pipes */
    //void calcPipes( ConvertItem *item );

    /** holds all active files */
    QList<ConvertItem*> items;

    Config *config;
    CDManager* cdManager;
    FileList *fileList;
    Logger* logger;
    KProcess notify;
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

    /** Change the process priorities */
//     void priorityChanged( int );

signals:
    // connected to FileList
    /** The conversion of an item has finished and the state is reported ( 0 = ok, -1 = error, 1 = aborted ) */
    void finished( FileListItem *item, int state );
    /** The next track from the device can be ripped while the track is being encoded */
    void rippingFinished( const QString& device );

    // connected to Logger
    /** Tell the logger that the process has finished */
    void finishedProcess( int id, int state );

    // connected to ProgressIndicator
//     void countTime( float );
//     void uncountTime( float );
    void updateTime( float );
    void timeFinished( float );
};

#endif // CONVERT_H
