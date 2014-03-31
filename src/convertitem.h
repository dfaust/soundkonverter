

#ifndef CONVERTITEM_H
#define CONVERTITEM_H

#include "pluginloader.h"

#include <kio/job.h>

#include <QList>
#include <QTime>
#include <QWeakPointer>

class FileListItem;
class KProcess;


/**
 * @short The items for the conversion (for every active file)
 * @author Daniel Faust <hessijames@gmail.com>
 */
class ConvertItem
{
public:
    /**
     * A list of flags for knowing what to do
     */
    enum Mode {
        initial            = 0x0000, // Initial state
        get                = 0x0001, // Copy the file to tmp
        convert            = 0x0002, // Convert the file
        rip                = 0x0004, // Ripping the file (only for the current state)
        decode             = 0x0008, // Decoding the file (only for the current state)
        filter             = 0x0010, // Applying filters (only for the current state)
        encode             = 0x0020, // Encoding the file (only for the current state)
        wait_replaygain    = 0x0040, // Wait for replaygain
        replaygain         = 0x0080  // Apply replaygain
//         write_tags         = 0x0100, // Write the tags to the file
//         execute_userscript = 0x0200  // Run the user script
    };

    /** Constructor, @p item a pointer to the file list item */
    ConvertItem( FileListItem *item );

    /** Destructor */
    ~ConvertItem();

    /** a reference to the file list item, in case it's a convert item */
    FileListItem *fileListItem;

    /** a list of conversion pipes that are suitable for this item */
    QList<ConversionPipe> conversionPipes;
    /** a list of replaygain pipes that are suitable for this item */
    QList<ReplayGainPipe> replaygainPipes;
    /** number of the current attempt to convert the file/add replaygain (for the pipes lists) */
    int take;
    /** number of the last try in case the conversion wasn't successful in the end */
    int lastTake; // TODO use cleaner solution

    /** for the conversion and moving the file to a temporary place */
    QWeakPointer<KProcess> process;
    /** for moving the file to the temporary directory */
    QWeakPointer<KIO::FileCopyJob> kioCopyJob;
    /** the active plugin */
    BackendPlugin *backendPlugin;
    /** the id from the active plugin (-1 if false) */
    int backendID;
    /** the current conversion step */
    int conversionPipesStep;
    /** has the process been killed on purpose? */
    bool killed;
    /** has the internal replaygain been used in this conversion take? */
    bool internalReplayGainUsed;

    /** the url from fileListItem or the download temp file */
    KUrl inputUrl;
    /** the path and the name of the output file */
    KUrl outputUrl;
    /** the downloaded input file */
    KUrl tempInputUrl;
    /** the temp files for the conversion */
    QList<KUrl> tempConvertUrls;

    KUrl generateTempUrl( const QString& prefix, const QString& extension, bool useSharedMemory = false );

    /** what shall we do with the file? */
    Mode mode;
    /** and what are we doing with the file? */
    Mode state;

    /** the id with that the item is registered at the logger */
    int logID;

    /** the time from the file list item splitted up */
    float getTime;
    QList<float> convertTimes;
    float replaygainTime;

    float finishedTime; // the time of the finished conversion steps

    void updateTimes();

    /** the current conversion progress */
    float progress;

    QTime progressedTime;
};

#endif // CONVERTITEM_H
