
#include "replaygainprocessor.h"

#include "config.h"
#include "logger.h"
#include "core/replaygainplugin.h"
#include "replaygainfilelist.h"
#include "global.h"

#include <KLocale>

#include <QFile>
#include <QFileInfo>


ReplayGainProcessorItem::ReplayGainProcessorItem( ReplayGainFileListItem *_fileListItem )
    : fileListItem( _fileListItem )
{
    backendPlugin = 0;
    backendID = -1;

    take = 0;

    killed = false;

    time = 0;
}

ReplayGainProcessorItem::~ReplayGainProcessorItem()
{}


ReplayGainProcessor::ReplayGainProcessor( Config *_config, ReplayGainFileList *_fileList, Logger *_logger )
    : config( _config ),
    fileList( _fileList ),
    logger( _logger )
{
    connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateProgress()) );

    QList<ReplayGainPlugin*> replaygainPlugins = config->pluginLoader()->getAllReplayGainPlugins();
    for( int i=0; i<replaygainPlugins.size(); i++ )
    {
        connect( replaygainPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( replaygainPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
}

ReplayGainProcessor::~ReplayGainProcessor()
{}

void ReplayGainProcessor::replaygain( ReplayGainProcessorItem *item )
{
    if( !item )
        return;

    logger->log( item->logID, "<br>" + i18n("Applying Replay Gain") );

    if( item->take > item->replaygainPipes.count() - 1 )
    {
        logger->log( item->logID, "\t" + i18n("No more backends left to try :(") );
        remove( item, ReplayGainFileListItem::Failed );
        return;
    }

    item->backendPlugin = item->replaygainPipes.at(item->take).plugin;
    KUrl::List urlList;
    if( item->fileListItem->type == ReplayGainFileListItem::Track )
    {
        urlList.append( item->fileListItem->url );
//         item->fileListItem->state = ReplayGainFileListItem::Processing;
    }
    else
    {
        for( int j=0; j<item->fileListItem->childCount(); j++ )
        {
            ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->fileListItem->child(j);
            urlList.append( child->url );
//             child->state = ReplayGainFileListItem::Processing;
        }
    }
    item->backendID = qobject_cast<ReplayGainPlugin*>(item->backendPlugin)->apply( urlList, item->mode );

    if( item->backendID < 100 )
    {
        switch( item->backendID )
        {
            case BackendPlugin::BackendNeedsConfiguration:
            {
                logger->log( item->logID, "\t" + i18n("Replay Gain failed. The used backend needs to be configured properly.") );
                remove( item, ReplayGainFileListItem::BackendNeedsConfiguration );
                break;
            }
            case BackendPlugin::FeatureNotSupported:
            {
                logger->log( item->logID, "\t" + i18n("Replay Gain failed. The preferred plugin lacks support for a necessary feature.") );
                item->take++;
                replaygain( item );
                break;
            }
            case BackendPlugin::UnknownError:
            {
                logger->log( item->logID, "\t" + i18n("Replay Gain failed. Unknown Error.") );
                item->take++;
                replaygain( item );
                break;
            }
        }
    }
    else
    {
        if( !updateTimer.isActive() )
            updateTimer.start( ConfigUpdateDelay );
    }
}

void ReplayGainProcessor::pluginProcessFinished( int id, int exitCode )
{
    if( QObject::sender() == 0 )
    {
        logger->log( 1000, QString("Error: pluginProcessFinished was called from a null sender. Exit code: %1").arg(exitCode) );
        return;
    }

    foreach( ReplayGainProcessorItem *item, items )
    {
        if( item->backendPlugin && item->backendPlugin == QObject::sender() && item->backendID == id )
        {
            item->backendID = -1;

            if( item->killed )
            {
                remove( item, ReplayGainFileListItem::StoppedByUser );
                return;
            }

            if( exitCode == 0 )
            {
                remove( item, ReplayGainFileListItem::Succeeded );
            }
            else
            {
                logger->log( item->logID, "\t" + i18n("Replay Gain failed. Exit code: %1",exitCode) );
                item->take++;
                replaygain( item );
            }
        }
    }
}

void ReplayGainProcessor::pluginLog( int id, const QString& message )
{
    // log all cached logs that can be logged now
    for( int i=0; i<pluginLogQueue.size(); i++ )
    {
        for( int j=0; j<items.size(); j++ )
        {
            if( items.at(j)->backendPlugin && items.at(j)->backendPlugin == pluginLogQueue.at(i).plugin && items.at(j)->backendID == pluginLogQueue.at(i).id )
            {
                for( int k=0; k<pluginLogQueue.at(i).messages.size(); k++ )
                {
                    logger->log( items.at(j)->logID, pluginLogQueue.at(i).messages.at(k) );
                }
                pluginLogQueue.removeAt(i);
                i--;
                break;
            }
        }
    }

    if( message.trimmed().isEmpty() )
        return;

    // search the matching process
    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->backendPlugin && items.at(i)->backendPlugin == QObject::sender() && items.at(i)->backendID == id )
        {
            logger->log( items.at(i)->logID, message );
            return;
        }
    }

    // if the process can't be found; add to the log queue
    for( int i=0; i<pluginLogQueue.size(); i++ )
    {
        if( pluginLogQueue.at(i).plugin == QObject::sender() && pluginLogQueue.at(i).id == id )
        {
            pluginLogQueue[i].messages += message;
            return;
        }
    }

    // no existing item in the log queue; create new log queue item
    LogQueue newLog;
    newLog.plugin = qobject_cast<BackendPlugin*>(QObject::sender());
    newLog.id = id;
    newLog.messages = QStringList(message);
    pluginLogQueue += newLog;

//     logger->log( 1000, qobject_cast<BackendPlugin*>(QObject::sender())->name() + ": " + message.trimmed().replace("\n","\n\t") );
}

void ReplayGainProcessor::add( ReplayGainFileListItem* fileListItem, ReplayGainPlugin::ApplyMode mode )
{
    QString identifier;
    if( fileListItem->type == ReplayGainFileListItem::Track )
    {
        identifier = fileListItem->url.toLocalFile();
    }
    else
    {
        identifier = config->data.general.replayGainGrouping == Config::Data::General::Directory ? fileListItem->url.pathOrUrl().right(fileListItem->url.pathOrUrl().length()-fileListItem->url.pathOrUrl().lastIndexOf("/")-1) : fileListItem->albumName;
    }
    logger->log( 1000, i18n("Adding new item to Replay Gain processing list: '%1'",identifier) );

    ReplayGainProcessorItem *newItem = new ReplayGainProcessorItem( fileListItem );
    items.append( newItem );

    newItem->mode = mode;

    // register at the logger
    newItem->logID = logger->registerProcess( i18n("Replay Gain for %1",identifier) );
    logger->log( 1000, "\t" + i18n("Got log ID: %1",newItem->logID) );

    if( fileListItem->type == ReplayGainFileListItem::Track )
    {
        newItem->time = fileListItem->length;
    }
    else
    {
        for( int j=0; j<newItem->fileListItem->childCount(); j++ )
        {
            ReplayGainFileListItem *child = (ReplayGainFileListItem*)newItem->fileListItem->child(j);
            newItem->time += child->length;
        }
    }

    newItem->replaygainPipes = config->pluginLoader()->getReplayGainPipes( fileListItem->codecName );

    logger->log( newItem->logID, "\t" + i18n("Possible Replay Gain backends:") );
    for( int i=0; i<newItem->replaygainPipes.size(); i++ )
    {
        logger->log( newItem->logID, "\t\t" + newItem->replaygainPipes.at(i).plugin->name() );
    }

    // (visual) feedback
//     fileListItem->state = ReplayGainFileListItem::Processing;

    newItem->progressedTime.start();

    // and start
    replaygain( newItem );
}

void ReplayGainProcessor::remove( ReplayGainProcessorItem *item, ReplayGainFileListItem::ReturnCode returnCode )
{
    QString exitMessage;

    if( returnCode == ReplayGainFileListItem::Succeeded && item->take > 0 )
        returnCode = ReplayGainFileListItem::SucceededWithProblems;

    switch( returnCode )
    {
        case ReplayGainFileListItem::Succeeded:
            exitMessage = i18nc("Conversion exit status","Normal exit");
            break;
        case ReplayGainFileListItem::SucceededWithProblems:
            exitMessage = i18nc("Conversion exit status","Succeeded but problems occured");
            break;
        case ReplayGainFileListItem::StoppedByUser:
            exitMessage = i18nc("Conversion exit status","Aborted by the user");
            break;
        case ReplayGainFileListItem::BackendNeedsConfiguration:
            exitMessage = i18nc("Conversion exit status","Backend needs configuration");
            break;
        case ReplayGainFileListItem::Skipped:
            exitMessage = i18nc("Conversion exit status","Skipped");
            break;
        case ReplayGainFileListItem::Failed:
            exitMessage = i18nc("Conversion exit status","An error occurred");
            break;
    }

    logger->log( item->logID, "<br>" +  i18n("Removing file from conversion list. Exit code %1 (%2)",returnCode,exitMessage) );

    emit timeFinished( (float)item->time );

    emit finished( item->fileListItem, returnCode ); // send signal to FileList
    emit finishedProcess( item->logID, returnCode == ReplayGainFileListItem::Succeeded ); // send signal to Logger

    items.removeAll( item );
    delete item;

    if( items.size() == 0 )
        updateTimer.stop();
}

void ReplayGainProcessor::kill( ReplayGainFileListItem *fileListItem )
{
    foreach( ReplayGainProcessorItem *item, items )
    {
        if( item->fileListItem == fileListItem )
        {
            item->killed = true;

            if( item->backendID != -1 && item->backendPlugin )
            {
                item->backendPlugin->kill( item->backendID );
            }
        }
    }
}

void ReplayGainProcessor::updateProgress()
{
    float time = 0.0f;

    // trigger flushing of the logger cache
    pluginLog( 0, "" );

    foreach( ReplayGainProcessorItem *item, items )
    {
        float fileProgress = 0.0f;

        if( item->backendID != -1 && item->backendPlugin )
        {
            fileProgress = item->backendPlugin->progress( item->backendID );
        }

        time += (float)item->time * fileProgress / 100.0f;
        logger->log( item->logID, "<pre>\t<span style=\"color:#585858\">" + i18n("Progress: %1",fileProgress) + "</span></pre>" );
    }

    emit updateTime( time );
}

