
#include "convert.h"
#include "convertitem.h"
#include "config.h"
#include "core/codecplugin.h"
#include "core/conversionoptions.h"
#include "filelist.h"
#include "global.h"
#include "logger.h"
#include "outputdirectory.h"

#include <kio/jobclasses.h>
#include <kio/job.h>

#include <KLocale>
#include <KMessageBox>
#include <QFile>
#include <QFileInfo>


Convert::Convert( Config *_config, FileList *_fileList, Logger *_logger, QObject *parent )
    : QObject( parent ),
    config( _config ),
    fileList( _fileList ),
    logger( _logger )
{
    connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateProgress()) );

    QList<CodecPlugin*> codecPlugins = config->pluginLoader()->getAllCodecPlugins();
    for( int i=0; i<codecPlugins.size(); i++ )
    {
        connect( codecPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( codecPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
    QList<FilterPlugin*> filterPlugins = config->pluginLoader()->getAllFilterPlugins();
    for( int i=0; i<filterPlugins.size(); i++ )
    {
        connect( filterPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( filterPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
    QList<ReplayGainPlugin*> replaygainPlugins = config->pluginLoader()->getAllReplayGainPlugins();
    for( int i=0; i<replaygainPlugins.size(); i++ )
    {
        connect( replaygainPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( replaygainPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
    QList<RipperPlugin*> ripperPlugins = config->pluginLoader()->getAllRipperPlugins();
    for( int i=0; i<ripperPlugins.size(); i++ )
    {
        connect( ripperPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( ripperPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
}

Convert::~Convert()
{}

void Convert::cleanUp()
{
    // TODO clean up
}

void Convert::get( ConvertItem *item )
{
    if( item->take > 0 )
        remove( item, FileListItem::Failed );

    logger->log( item->logID, i18n("Getting file") );
    item->state = ConvertItem::get;

    item->tempInputUrl = item->generateTempUrl( "download", item->inputUrl.fileName().mid(item->inputUrl.fileName().lastIndexOf(".")+1) );

    if( !updateTimer.isActive() )
        updateTimer.start( ConfigUpdateDelay );

    logger->log( item->logID, i18n("Copying \"%1\" to \"%2\"",item->inputUrl.pathOrUrl(),item->tempInputUrl.toLocalFile()) );

    item->kioCopyJob = KIO::file_copy( item->inputUrl, item->tempInputUrl, 0700 , KIO::HideProgressInfo );
    connect( item->kioCopyJob.data(), SIGNAL(result(KJob*)), this, SLOT(kioJobFinished(KJob*)) );
    connect( item->kioCopyJob.data(), SIGNAL(percent(KJob*,unsigned long)), this, SLOT(kioJobProgress(KJob*,unsigned long)) );
}

void Convert::convert( ConvertItem *item )
{
    if( !item )
        return;

    ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions( item->fileListItem->conversionOptionsId );
    if( !conversionOptions )
        return;

    KUrl inputUrl;
    if( !item->tempInputUrl.toLocalFile().isEmpty() )
        inputUrl = item->tempInputUrl;
    else
        inputUrl = item->inputUrl;

    if( !item->fileListItem->tags )
        item->fileListItem->tags = config->tagEngine()->readTags( inputUrl );

    if( item->outputUrl.isEmpty() )
    {
        // item->outputUrl = !item->fileListItem->outputUrl.url().isEmpty() ? item->fileListItem->outputUrl : OutputDirectory::calcPath( item->fileListItem, config, usedOutputNames.values() );
        item->outputUrl = OutputDirectory::calcPath( item->fileListItem, config, usedOutputNames.values() );
        if( QFile::exists(item->outputUrl.toLocalFile()) )
        {
            logger->log( item->logID, "\tOutput file already exists" );
            item->outputUrl = KUrl();
            remove( item, FileListItem::Skipped );
            return;
        }
        OutputDirectory::makePath( item->outputUrl );
        fileList->updateItem( item->fileListItem );
    }
    usedOutputNames.insert( item->logID, item->outputUrl.toLocalFile() );

    if( item->take > item->conversionPipes.count() - 1 )
    {
        logger->log( item->logID, "\t" + i18n("No more backends left to try :(") );
        remove( item, FileListItem::Failed );
        return;
    }

    if( item->take > 0 )
        item->updateTimes();

    item->conversionPipesStep = -1;

    if( !updateTimer.isActive() )
        updateTimer.start( ConfigUpdateDelay );

    if( item->conversionPipes.at(item->take).trunks.count() == 1 ) // conversion can be done by one plugin alone
    {
        logger->log( item->logID, i18n("Converting") );
        item->state = ConvertItem::convert;
        item->conversionPipesStep = 0;
        item->backendPlugin = item->conversionPipes.at(item->take).trunks.at(0).plugin;
        if( item->backendPlugin->type() == "codec" || item->backendPlugin->type() == "filter" )
        {
            bool useInternalReplayGain = false;
            if( item->conversionPipes.at(item->take).trunks.at(0).data.hasInternalReplayGain && item->mode & ConvertItem::replaygain )
            {
                foreach( Config::CodecData codecData, config->data.backends.codecs )
                {
                    if( codecData.codecName == item->conversionPipes.at(item->take).trunks.at(0).codecTo )
                    {
                        if( codecData.replaygain.first() == i18n("Try internal") )
                            useInternalReplayGain = true;

                        break;
                    }
                }
            }
            if( useInternalReplayGain )
            {
                item->internalReplayGainUsed = true;
            }
            item->backendID = qobject_cast<CodecPlugin*>(item->backendPlugin)->convert( inputUrl, item->outputUrl, item->conversionPipes.at(item->take).trunks.at(0).codecFrom, item->conversionPipes.at(item->take).trunks.at(0).codecTo, conversionOptions, item->fileListItem->tags, useInternalReplayGain );
        }
        else if( item->backendPlugin->type() == "ripper" )
        {
            item->backendID = qobject_cast<RipperPlugin*>(item->backendPlugin)->rip( item->fileListItem->device, item->fileListItem->track, item->fileListItem->tracks, item->outputUrl );
        }

        if( item->backendID < 100 )
        {
            switch( item->backendID )
            {
                case BackendPlugin::BackendNeedsConfiguration:
                {
                    logger->log( item->logID, "\t" + i18n("Conversion failed. At least one of the used backends needs to be configured properly.") );
                    remove( item, FileListItem::BackendNeedsConfiguration );
                    break;
                }
                case BackendPlugin::FeatureNotSupported:
                {
                    logger->log( item->logID, "\t" + i18n("Conversion failed. The preferred plugin lacks support for a necessary feature.") );
                    executeSameStep( item );
                    break;
                }
                case BackendPlugin::UnknownError:
                {
                    logger->log( item->logID, "\t" + i18n("Conversion failed. Unknown Error.") );
                    executeSameStep( item );
                    break;
                }
            }
        }
        else if( item->backendPlugin->type() == "ripper" )
        {
            item->fileListItem->state = FileListItem::Ripping;
        }
    }
    else // conversion needs two plugins or more
    {
        bool usePipes = false;
        bool useInternalReplayGain = false;
        QStringList commandList;
        if( config->data.advanced.usePipes )
        {
            usePipes = true;

            const int stepCount = item->conversionPipes.at(item->take).trunks.count() - 1;
            int step = 0;
            foreach( const ConversionPipeTrunk trunk, item->conversionPipes.at(item->take).trunks )
            {
                BackendPlugin *plugin = trunk.plugin;
                QStringList command;
                const KUrl inUrl = ( step == 0 ) ? inputUrl : KUrl();
                const KUrl outUrl = ( step == stepCount ) ? item->outputUrl : KUrl();
                if( plugin->type() == "codec" || plugin->type() == "filter" )
                {
                    if( step == stepCount && trunk.data.hasInternalReplayGain && item->mode & ConvertItem::replaygain )
                    {
                        foreach( Config::CodecData codecData, config->data.backends.codecs )
                        {
                            if( codecData.codecName == trunk.codecTo )
                            {
                                if( codecData.replaygain.first() == i18n("Try internal") )
                                    useInternalReplayGain = true;

                                break;
                            }
                        }
                    }
                    command = qobject_cast<CodecPlugin*>(plugin)->convertCommand( inUrl, outUrl, item->conversionPipes.at(item->take).trunks.at(step).codecFrom, item->conversionPipes.at(item->take).trunks.at(step).codecTo, conversionOptions, item->fileListItem->tags, useInternalReplayGain );
                }
                else if( plugin->type() == "ripper" )
                {
                    command = qobject_cast<RipperPlugin*>(plugin)->ripCommand( item->fileListItem->device, item->fileListItem->track, item->fileListItem->tracks, outUrl );
                }
                if( command.isEmpty() )
                {
                    usePipes = false;
                    break;
                }
                commandList.append( command.join(" ") );
                step++;
            }
        }
        if( usePipes )
        {
            // both plugins support pipes
            const QString command = commandList.join(" | ");

            if( useInternalReplayGain )
                item->internalReplayGainUsed = true;

            logger->log( item->logID, i18n("Converting") );
            item->state = ConvertItem::convert;
            // merge all conversion times into one since we are doing everything in one step
            float time = 0.0f;
            foreach( float t, item->convertTimes )
            {
                time += t;
            }
            item->convertTimes.clear();
            item->convertTimes.append( time );
            item->conversionPipesStep = 0;
            logger->log( item->logID, "<pre>\t<span style=\"color:#DC6300\">" + command + "</span></pre>" );
            item->process = new KProcess();
            item->process.data()->setOutputChannelMode( KProcess::MergedChannels );
            connect( item->process.data(), SIGNAL(readyRead()), this, SLOT(processOutput()) );
            connect( item->process.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );
            item->process.data()->clearProgram();
            item->process.data()->setShellCommand( command );
            item->process.data()->start();
        }
        else
        {
            // at least on plugins doesn't support pipes

            // estimated size of the wav file
            int estimatedFileSize = 0;
            if( item->conversionPipes.at(item->take).trunks.first().plugin->type() == "ripper" && item->fileListItem->tags )
            {
                estimatedFileSize = item->fileListItem->tags->length * 44100*16*2/8/1024/1024;
            }
            else
            {
                QFileInfo fileInfo( inputUrl.toLocalFile() );
                estimatedFileSize = fileInfo.size();
                if( config->pluginLoader()->isCodecLossless(item->fileListItem->codecName) )
                {
                    estimatedFileSize *= 1.4;
                }
                else if( item->fileListItem->codecName == "midi" || item->fileListItem->codecName == "mod" )
                {
                    estimatedFileSize *= 1000;
                }
                else
                {
                    estimatedFileSize *= 10;
                }
                estimatedFileSize /= 1024*1024;
            }
            const bool useSharedMemory = config->data.advanced.useSharedMemoryForTempFiles && estimatedFileSize > 0 && estimatedFileSize < config->data.advanced.maxSizeForSharedMemoryTempFiles;

            for( int i=0; i<item->conversionPipes.at(item->take).trunks.count()-1; i++ )
            {
                item->tempConvertUrls += item->generateTempUrl( "convert", config->pluginLoader()->codecExtensions(item->conversionPipes.at(item->take).trunks.at(i).codecTo).first(), useSharedMemory );
            }

            convertNextBackend( item );
        }
    }
}

void Convert::convertNextBackend( ConvertItem *item )
{
    if( !item )
        return;

    ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions( item->fileListItem->conversionOptionsId );
    if( !conversionOptions )
        return;

    const int stepCount = item->conversionPipes.at(item->take).trunks.count() - 1;
    const int step = ++item->conversionPipesStep;

    if( step > stepCount )
    {
        executeNextStep( item );
    }

    BackendPlugin *plugin = item->conversionPipes.at(item->take).trunks.at(step).plugin;
    if( !plugin )
        return;

    KUrl inputUrl;
    if( !item->tempInputUrl.toLocalFile().isEmpty() )
        inputUrl = item->tempInputUrl;
    else
        inputUrl = item->inputUrl;

    const KUrl inUrl = ( step == 0 ) ? inputUrl : item->tempConvertUrls.at(step - 1);
    const KUrl outUrl = ( step == stepCount ) ? item->outputUrl : item->tempConvertUrls.at(step);

    if( step == 0 )
    {
        if( plugin->type() == "ripper" )
        {
            item->state = ConvertItem::rip;
            logger->log( item->logID, i18n("Ripping") );
        }
        else if( plugin->type() == "codec" || ( plugin->type() == "filter" && item->conversionPipes.at(item->take).trunks.at(step).codecFrom != "wav" ) )
        {
            item->state = ConvertItem::decode;
            logger->log( item->logID, i18n("Decoding") );
        }
        else if( plugin->type() == "filter" )
        {
            item->state = ConvertItem::filter;
            logger->log( item->logID, i18n("Applying filter") );
        }
    }
    else if( step == stepCount )
    {
        if( plugin->type() == "codec" || ( plugin->type() == "filter" && item->conversionPipes.at(item->take).trunks.at(step).codecTo != "wav" ) )
        {
            item->state = ConvertItem::encode;
            logger->log( item->logID, i18n("Encoding") );
        }
        else if( plugin->type() == "filter" )
        {
            item->state = ConvertItem::filter;
            logger->log( item->logID, i18n("Applying filter") );
        }
    }
    else
    {
        item->state = ConvertItem::filter;
        logger->log( item->logID, i18n("Applying filter") );
    }

    item->backendPlugin = plugin;
    if( plugin->type() == "codec" || plugin->type() == "filter" )
    {
        bool useInternalReplayGain = false;
        if( step == stepCount && item->conversionPipes.at(item->take).trunks.at(step).data.hasInternalReplayGain && item->mode & ConvertItem::replaygain )
        {
            foreach( Config::CodecData codecData, config->data.backends.codecs )
            {
                if( codecData.codecName == item->conversionPipes.at(item->take).trunks.at(step).codecTo )
                {
                    if( codecData.replaygain.first() == i18n("Try internal") )
                        useInternalReplayGain = true;

                    break;
                }
            }
        }
        if( useInternalReplayGain )
        {
            item->internalReplayGainUsed = true;
        }
        item->backendID = qobject_cast<CodecPlugin*>(plugin)->convert( inUrl, outUrl, item->conversionPipes.at(item->take).trunks.at(step).codecFrom, item->conversionPipes.at(item->take).trunks.at(step).codecTo, conversionOptions, item->fileListItem->tags, useInternalReplayGain );
    }
    else if( plugin->type() == "ripper" )
    {
        item->backendID = qobject_cast<RipperPlugin*>(plugin)->rip( item->fileListItem->device, item->fileListItem->track, item->fileListItem->tracks, outUrl );
    }

    if( item->backendID < 100 )
    {
        switch( item->backendID )
        {
            case BackendPlugin::BackendNeedsConfiguration:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. At least one of the used backends needs to be configured properly.") );
                remove( item, FileListItem::BackendNeedsConfiguration );
                break;
            }
            case BackendPlugin::FeatureNotSupported:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. The preferred plugin lacks support for a necessary feature.") );
                executeSameStep( item );
                break;
            }
            case BackendPlugin::UnknownError:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. Unknown Error.") );
                executeSameStep( item );
                break;
            }
        }
    }
    else if( item->backendPlugin->type() == "ripper" )
    {
        item->fileListItem->state = FileListItem::Ripping;
    }
}

void Convert::replaygain( ConvertItem *item )
{
    if( !item )
        return;

    const QString albumName = item->fileListItem->tags ? item->fileListItem->tags->album : "";

    if( fileList->waitForAlbumGain(item->fileListItem) )
    {
        logger->log( item->logID, i18n("Skipping Replay Gain, Album Gain will be calculated later") );

        albumGainItems[albumName].append( item );

        item->state = ConvertItem::replaygain;
        executeNextStep( item );
        return;
    }

    QList<ConvertItem*> albumItems;
    if( !albumName.isEmpty() )
        albumItems = albumGainItems[albumName];

    albumItems.append( item );

    logger->log( item->logID, i18n("Applying Replay Gain") );

    if( item->take > item->replaygainPipes.count() - 1 )
    {
        logger->log( item->logID, "\t" + i18n("No more backends left to try :(") );
        remove( item, FileListItem::Failed );
        return;
    }

    item->state = ConvertItem::replaygain;
    item->backendPlugin = item->replaygainPipes.at(item->take).plugin;
    KUrl::List urlList;
    foreach( ConvertItem *albumItem, albumItems )
    {
        urlList.append( albumItem->outputUrl );
        if( albumItem != item )
        {
            albumItem->fileListItem->state = FileListItem::ApplyingAlbumGain;
            fileList->updateItem( albumItem->fileListItem );
        }
    }
    item->backendID = qobject_cast<ReplayGainPlugin*>(item->backendPlugin)->apply( urlList );

    if( item->backendID < 100 )
    {
        switch( item->backendID )
        {
            case BackendPlugin::BackendNeedsConfiguration:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. At least one of the used backends needs to be configured properly.") );
                remove( item, FileListItem::BackendNeedsConfiguration );
                break;
            }
            case BackendPlugin::FeatureNotSupported:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. The preferred plugin lacks support for a necessary feature.") );
                executeSameStep( item );
                break;
            }
            case BackendPlugin::UnknownError:
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. Unknown Error.") );
                executeSameStep( item );
                break;
            }
        }
    }

    if( !updateTimer.isActive() )
        updateTimer.start( ConfigUpdateDelay );
}

void Convert::writeTags( ConvertItem *item )
{
    if( !item || !item->fileListItem || !item->fileListItem->tags )
        return;

    logger->log( item->logID, i18n("Writing tags") );
//     item->state = ConvertItem::write_tags;
//     item->fileListItem->setText( 0, i18n("Writing tags")+"... 00 %" );

    config->tagEngine()->writeTags( item->outputUrl, item->fileListItem->tags );

    KUrl inputUrl;
    if( !item->tempInputUrl.toLocalFile().isEmpty() )
        inputUrl = item->tempInputUrl;
    else
        inputUrl = item->inputUrl;

    if( !(item->fileListItem->tags->tagsRead & TagData::Covers) )
    {
        item->fileListItem->tags->covers = config->tagEngine()->readCovers( inputUrl );
        item->fileListItem->tags->tagsRead = TagData::TagsRead(item->fileListItem->tags->tagsRead | TagData::Covers);
    }

    const bool success = config->tagEngine()->writeCovers( item->outputUrl, item->fileListItem->tags->covers );
    if( config->data.coverArt.writeCovers == 0 || ( config->data.coverArt.writeCovers == 1 && !success ) )
    {
        config->tagEngine()->writeCoversToDirectory( item->outputUrl.directory(), item->fileListItem->tags );
    }
}

void Convert::executeUserScript( ConvertItem *item )
{
    logger->log( item->logID, i18n("Running user script") );
    item->state = ConvertItem::execute_userscript;

//     KUrl source( item->fileListItem->options.filePathName );
//     KUrl destination( item->outputFilePathName );
//
//     item->fileListItem->setText( 0, i18n("Running user script")+"... 00 %" );
//
//     item->convertProcess->clearProgram();
//
//     QString userscript = locate( "data", "soundkonverter/userscript.sh" );
//     if( userscript == "" ) executeNextStep( item );
//
//     *(item->convertProcess) << userscript;
//     *(item->convertProcess) << source.path();
//     *(item->convertProcess) << destination.path();
//
//     logger->log( item->logID, userscript + " \"" + source.path() + "\" \"" + destination.path() + "\"" );

// // /*    item->convertProcess->setPriority( config->data.general.priority );
// //     item->convertProcess->start( KProcess::NotifyOnExit, KProcess::AllOutput );*/
}

void Convert::executeNextStep( ConvertItem *item )
{
    logger->log( item->logID, "<br>" +  i18n("Executing next step") );

    item->lastTake = item->take;
    item->take = 0;
    item->progress = 0.0f;

    switch( item->state )
    {
        case ConvertItem::get:
        {
            if( item->mode & ConvertItem::convert )
                convert( item );
            else
                remove( item, FileListItem::Succeeded );
            break;
        }
        case ConvertItem::convert:
        case ConvertItem::encode:
        {
            if( item->mode & ConvertItem::replaygain )
                replaygain( item );
            else
                remove( item, FileListItem::Succeeded );
            break;
        }
        case ConvertItem::rip:
        case ConvertItem::decode:
        case ConvertItem::filter:
        {
            item->take = item->lastTake;
            convertNextBackend( item );
            break;
        }
        case ConvertItem::replaygain:
        {
            remove( item, FileListItem::Succeeded );
            break;
        }
        default:
        {
            if( item->mode & ConvertItem::get )
                get( item );
            else if( item->mode & ConvertItem::convert )
                convert( item );
            else if( item->mode & ConvertItem::replaygain )
                replaygain( item );
            else
                remove( item, FileListItem::Succeeded );
            break;
        }
    }
}

void Convert::executeSameStep( ConvertItem *item )
{
    item->take++;
    item->updateTimes();
    item->progress = 0.0f;

    if( item->internalReplayGainUsed )
    {
        item->mode = ConvertItem::Mode( item->mode | ConvertItem::replaygain );
        item->internalReplayGainUsed = false;
    }

    switch( item->state )
    {
        case ConvertItem::get:
        {
            get( item );
            return;
        }
        case ConvertItem::rip:
        case ConvertItem::convert:
        case ConvertItem::decode:
        case ConvertItem::filter:
        case ConvertItem::encode:
        {
            // remove temp/failed files
            foreach( const KUrl url, item->tempConvertUrls )
            {
                if( QFile::exists(url.toLocalFile()) )
                {
                    QFile::remove( url.toLocalFile() );
                }
            }
            if( QFile::exists(item->outputUrl.toLocalFile()) )
            {
                QFile::remove( item->outputUrl.toLocalFile() );
            }
            convert( item );
            return;
        }
        case ConvertItem::replaygain:
        {
            replaygain( item );
            return;
        }
        default:
            break;
    }

    remove( item, FileListItem::Failed ); // shouldn't be possible
}

void Convert::kioJobProgress( KJob *job, unsigned long percent )
{
    // search the item list for our item
    foreach( ConvertItem *item, items )
    {
        if( item->kioCopyJob.data() == job )
        {
            item->progress = (float)percent;
        }
    }
}

void Convert::kioJobFinished( KJob *job )
{
    // search the item list for our item
    foreach( ConvertItem *item, items )
    {
        if( item->kioCopyJob.data() == job )
        {
            item->kioCopyJob.data()->deleteLater();

            if( job->error() == 0 ) // copy was successful
            {
                float fileTime;
                switch( item->state )
                {
                    case ConvertItem::get:
                        fileTime = item->getTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                if( item->state == ConvertItem::get )
                {
                    if( !item->fileListItem->tags )
                    {
                        item->fileListItem->tags = config->tagEngine()->readTags( item->tempInputUrl );
                        if( item->fileListItem->tags )
                        {
                            logger->log( item->logID, i18n("Read tags successfully") );
                        }
                        else
                        {
                            logger->log( item->logID, i18n("Unable to read tags") );
                        }
                    }
                }
                item->finishedTime += fileTime;
                executeNextStep( item );
            }
            else
            {
                // remove temp/failed files
                if( QFile::exists(item->tempInputUrl.toLocalFile()) )
                {
                    QFile::remove(item->tempInputUrl.toLocalFile());
                    logger->log( item->logID, i18nc("removing file","Removing: %1",item->tempInputUrl.toLocalFile()) );
                }
                if( QFile::exists(item->tempInputUrl.toLocalFile()+".part") )
                {
                    QFile::remove(item->tempInputUrl.toLocalFile()+".part");
                    logger->log( item->logID, i18nc("removing file","Removing: %1",item->tempInputUrl.toLocalFile()+".part") );
                }

                if( job->error() == 1 )
                {
                    remove( item, FileListItem::StoppedByUser );
                }
                else
                {
                    logger->log( item->logID, i18n("An error occurred. Error code: %1 (%2)",job->error(),job->errorString()) );
                    remove( item, FileListItem::Failed );
                }
            }
        }
    }
}

void Convert::processOutput()
{
    foreach( ConvertItem *item, items )
    {
        if( item->process.data() == QObject::sender() )
        {
            const QString output = item->process.data()->readAllStandardOutput().data();

            bool logOutput = true;
            foreach( const ConversionPipeTrunk trunk, item->conversionPipes.at(item->take).trunks )
            {
                const float progress = trunk.plugin->parseOutput( output );

                if( progress > item->progress )
                {
                    item->progress = progress;
                    logOutput = false;
                }
            }

            if( logOutput && !output.simplified().isEmpty() )
                logger->log( item->logID, "<pre>\t<span style=\"color:#C00000\">" + output.trimmed().replace("\n","<br>\t") + "</span></pre>" );

            break;
        }
    }
}

void Convert::processExit( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitStatus)

    if( QObject::sender() == 0 )
    {
        logger->log( 1000, QString("Error: processExit was called from a null sender. Exit code: %1").arg(exitCode) );
        KMessageBox::error( 0, QString("Error: processExit was called from a null sender. Exit code: %1").arg(exitCode) );
        return;
    }

    // search the item list for our item
    foreach( ConvertItem *item, items )
    {
        if( item->process.data() == QObject::sender() )
        {
            item->process.data()->deleteLater(); // NOTE crash discovered here - probably fixed by using deleteLater

            if( item->killed )
            {
                remove( item, FileListItem::StoppedByUser );
                return;
            }

            if( exitCode == 0 )
            {
                float fileTime;
                switch( item->state )
                {
                    case ConvertItem::convert:
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                    case ConvertItem::encode:
                        fileTime = item->convertTimes.at(item->conversionPipesStep);
                        break;
                    case ConvertItem::replaygain:
                        fileTime = item->replaygainTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                item->finishedTime += fileTime;

                if( item->state == ConvertItem::rip )
                {
                    item->fileListItem->state = FileListItem::Converting;
                    emit rippingFinished( item->fileListItem->device );
                }

                if( item->internalReplayGainUsed )
                {
                    item->mode = ConvertItem::Mode( item->mode ^ ConvertItem::replaygain );
                }

                switch( item->state )
                {
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                        convertNextBackend( item );
                        break;
                    default:
                        executeNextStep( item );
                }
            }
            else
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. Exit code: %1",exitCode) );
                executeSameStep( item );
            }
        }
    }
}

void Convert::pluginProcessFinished( int id, int exitCode )
{
    if( QObject::sender() == 0 )
    {
        logger->log( 1000, QString("Error: pluginProcessFinished was called from a null sender. Exit code: %1").arg(exitCode) );
        KMessageBox::error( 0, QString("Error: pluginProcessFinished was called from a null sender. Exit code: %1").arg(exitCode) );
        return;
    }

    foreach( ConvertItem *item, items )
    {
        if( item->backendPlugin && item->backendPlugin == QObject::sender() && item->backendID == id )
        {
            item->backendID = -1;

            if( item->killed )
            {
                remove( item, FileListItem::StoppedByUser );
                return;
            }

            if( exitCode == 0 )
            {
                float fileTime;
                switch( item->state )
                {
                    case ConvertItem::convert:
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                    case ConvertItem::encode:
                        fileTime = item->convertTimes.at(item->conversionPipesStep);
                        break;
                    case ConvertItem::replaygain:
                        fileTime = item->replaygainTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                item->finishedTime += fileTime;

                if( item->state == ConvertItem::rip )
                {
                    item->fileListItem->state = FileListItem::Converting;
                    emit rippingFinished( item->fileListItem->device );
                }

                if( item->internalReplayGainUsed )
                {
                    item->mode = ConvertItem::Mode( item->mode ^ ConvertItem::replaygain );
                }

                executeNextStep( item );
            }
            else
            {
                logger->log( item->logID, "\t" + i18n("Conversion failed. Exit code: %1",exitCode) );
                executeSameStep( item );
            }
        }
    }
}

void Convert::pluginLog( int id, const QString& message )
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

void Convert::add( FileListItem *fileListItem )
{
    KUrl fileName;
    if( fileListItem->track >= 0 )
    {
        if( fileListItem->tags )
        {
            fileName = KUrl( i18nc("identificator for the logger","CD track %1: %2 - %3",QString().sprintf("%02i",fileListItem->tags->track),fileListItem->tags->artist,fileListItem->tags->title) );
        }
        else // shouldn't be possible
        {
            fileName = KUrl( i18nc("identificator for the logger","CD track %1",fileListItem->track) );
        }
    }
    else
    {
        fileName = fileListItem->url;
    }
    logger->log( 1000, i18n("Adding new item to conversion list: '%1'",fileName.pathOrUrl()) );

    ConvertItem *newItem = new ConvertItem( fileListItem );
    items.append( newItem );

    // register at the logger
    newItem->logID = logger->registerProcess( fileName.pathOrUrl() );
    logger->log( 1000, "\t" + i18n("Got log ID: %1",QString::number(newItem->logID)) );

    // TODO remove redundancy, logID is needed in the fileListItem
    newItem->fileListItem->logId = newItem->logID;

//     logger->log( newItem->logID, "Mime Type: " + newItem->fileListItem->mimeType );
//     if( newItem->fileListItem->tags ) logger->log( newItem->logID, i18n("Tags successfully read") );
//     else logger->log( newItem->logID, i18n("Reading tags failed") );

    // set some variables to default values
    newItem->mode = (ConvertItem::Mode)0x0000;
    newItem->state = (ConvertItem::Mode)0x0000;

    newItem->inputUrl = fileListItem->url;

    ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions(fileListItem->conversionOptionsId);
    if( !conversionOptions )
    {
        logger->log( 1000, "Convert::add(...) no ConversionOptions found" );
        remove( newItem, FileListItem::Failed );
        return;
    }

    if( fileListItem->track >= 0 )
    {
        logger->log( newItem->logID, "\t" + i18n("Track number: %1, device: %2",QString::number(fileListItem->track),fileListItem->device) );
    }

    newItem->conversionPipes = config->pluginLoader()->getConversionPipes( fileListItem->codecName, conversionOptions->codecName, conversionOptions->filterOptions, conversionOptions->pluginName );

    logger->log( newItem->logID, "\t" + i18n("Possible conversion strategies:") );
    for( int i=0; i<newItem->conversionPipes.size(); i++ )
    {
        QStringList pipe_str;

        for( int j=0; j<newItem->conversionPipes.at(i).trunks.size(); j++ )
        {
            pipe_str += QString("%1 %2 %3 (%4)").arg(newItem->conversionPipes.at(i).trunks.at(j).codecFrom).arg(QChar(10141)).arg(newItem->conversionPipes.at(i).trunks.at(j).codecTo).arg(newItem->conversionPipes.at(i).trunks.at(j).plugin->name());
        }

        logger->log( newItem->logID, "\t\t" + pipe_str.join(", ") );
    }

    if( conversionOptions->outputDirectoryMode != OutputDirectory::Source )
        logger->log( newItem->logID, i18n("File system type: %1",conversionOptions->outputFilesystem) );

    newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::convert );

    if( conversionOptions->replaygain )
    {
        newItem->replaygainPipes = config->pluginLoader()->getReplayGainPipes( conversionOptions->codecName );
        newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::replaygain );
    }

    if( !newItem->inputUrl.isLocalFile() && fileListItem->track == -1 )
        newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::get );
//     if( (!newItem->inputUrl.isLocalFile() && item->track == -1) || newItem->inputUrl.url().toAscii() != newItem->inputUrl.url() )
//         newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::get );

    newItem->updateTimes();

    // (visual) feedback
    fileListItem->state = FileListItem::Converting;

    newItem->progressedTime.start();

    // and start
    executeNextStep( newItem );
}

void Convert::remove( ConvertItem *item, FileListItem::ReturnCode returnCode )
{
    // TODO "remove" (re-add) the times to the progress indicator
    //emit uncountTime( item->getTime + item->getCorrectionTime + item->ripTime +
    //                  item->decodeTime + item->encodeTime + item->replaygainTime );

    QString exitMessage;

    bool waitForAlbumGain = false;
    const QString albumName = item->fileListItem->tags ? item->fileListItem->tags->album : "";
    if( !albumName.isEmpty() )
        waitForAlbumGain = albumGainItems[albumName].contains( item );

    QFileInfo outputFileInfo( item->outputUrl.toLocalFile() );
    float fileRatio = outputFileInfo.size();
    if( item->fileListItem->track > 0 )
    {
        fileRatio /= item->fileListItem->length*4*44100;
    }
    else if( !item->fileListItem->local )
    {
        QFileInfo inputFileInfo( item->tempInputUrl.toLocalFile() );
        fileRatio /= inputFileInfo.size();
    }
    else
    {
        QFileInfo inputFileInfo( item->inputUrl.toLocalFile() );
        fileRatio /= inputFileInfo.size();
    }
    ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions( item->fileListItem->conversionOptionsId );
    if( fileRatio < 0.01 && outputFileInfo.size() < 100000 && returnCode != FileListItem::StoppedByUser && ( !conversionOptions || !config->pluginLoader()->isCodecInferiorQuality(conversionOptions->codecName) ) )
    {
        exitMessage = i18n("An error occurred, the output file size is less than one percent of the input file size");

        if( returnCode == FileListItem::Succeeded || returnCode == FileListItem::SucceededWithProblems )
        {
            logger->log( item->logID, i18n("Conversion failed, trying again. Exit code: -2 (%1)",exitMessage) );
            item->take = item->lastTake;
            logger->log( item->logID, i18n("Removing partially converted output file") );
            QFile::remove( item->outputUrl.toLocalFile() );
            executeSameStep( item );
            return;
        }
    }

    if( returnCode == FileListItem::Succeeded && item->lastTake > 0 )
        returnCode = FileListItem::SucceededWithProblems;

    switch( returnCode )
    {
        case FileListItem::Succeeded:
            exitMessage = i18nc("Conversion exit status","Normal exit");
            break;
        case FileListItem::SucceededWithProblems:
            exitMessage = i18nc("Conversion exit status","Succeeded but problems occurred");
            break;
        case FileListItem::StoppedByUser:
            exitMessage = i18nc("Conversion exit status","Aborted by the user");
            break;
        case FileListItem::BackendNeedsConfiguration:
            exitMessage = i18nc("Conversion exit status","Backend needs configuration");
            break;
        case FileListItem::DiscFull:
            exitMessage = i18nc("Conversion exit status","Not enough space on the output device");
            break;
        case FileListItem::Skipped:
            exitMessage = i18nc("Conversion exit status","File already exists");
            break;
        case FileListItem::Failed:
            exitMessage = i18nc("Conversion exit status","An error occurred");
            break;
    }

    if( returnCode == FileListItem::Succeeded || returnCode == FileListItem::SucceededWithProblems )
        writeTags( item );

    if( !waitForAlbumGain && !item->fileListItem->notifyCommand.isEmpty() && ( !config->data.general.waitForAlbumGain || !conversionOptions->replaygain ) )
    {
        QList<ConvertItem*> albumItems;
        if( !albumName.isEmpty() )
            albumItems = albumGainItems[albumName];

        albumItems.append( item );

        foreach( ConvertItem *albumItem, albumItems )
        {
            QString command = albumItem->fileListItem->notifyCommand;
            // command.replace( "%u", albumItem->fileListItem->url );
            command.replace( "%i", albumItem->inputUrl.toLocalFile() );
            command.replace( "%o", albumItem->outputUrl.toLocalFile() );
            logger->log( item->logID, i18n("Executing command: \"%1\"",command) );

            QProcess::startDetached( command );
        }
    }

    // remove temp/failed files
    if( QFile::exists(item->tempInputUrl.toLocalFile()) )
    {
        QFile::remove(item->tempInputUrl.toLocalFile());
    }
    foreach( const KUrl url, item->tempConvertUrls )
    {
        if( QFile::exists(url.toLocalFile()) )
        {
            QFile::remove( url.toLocalFile() );
        }
    }
    if( returnCode != FileListItem::Succeeded && returnCode != FileListItem::SucceededWithProblems && returnCode != FileListItem::Skipped && QFile::exists(item->outputUrl.toLocalFile()) )
    {
        logger->log( item->logID, i18n("Removing partially converted output file") );
        QFile::remove(item->outputUrl.toLocalFile());
    }

    usedOutputNames.remove( item->logID );

    logger->log( item->logID, "<br>" +  i18n("Removing file from conversion list. Exit code %1 (%2)",returnCode,exitMessage) );

    logger->log( item->logID, "\t" + i18n("Conversion time") + ": " + Global::prettyNumber(item->progressedTime.elapsed(),"ms") );
    logger->log( item->logID, "\t" + i18n("Output file size") + ": " + Global::prettyNumber(outputFileInfo.size(),"B") );
    logger->log( item->logID, "\t" + i18n("File size ratio") + ": " + Global::prettyNumber(fileRatio*100,"%") );

    emit timeFinished( item->fileListItem->length );

    if( item->process.data() )
        item->process.data()->deleteLater();
    if( item->kioCopyJob.data() )
        item->kioCopyJob.data()->deleteLater();

    if( !waitForAlbumGain && !albumName.isEmpty() )
    {
        QList<ConvertItem*> albumItems = albumGainItems[albumName];

        foreach( ConvertItem *albumItem, albumItems )
        {
            emit finished( albumItem->fileListItem, FileListItem::Succeeded ); // send signal to FileList
        }
        qDeleteAll( albumItems );

        albumGainItems.remove( albumName );
    }
    emit finished( item->fileListItem, returnCode, waitForAlbumGain ); // send signal to FileList
    emit finishedProcess( item->logID, returnCode == FileListItem::Succeeded, waitForAlbumGain ); // send signal to Logger

    items.removeAll( item );

    if( !waitForAlbumGain )
        delete item;

    if( items.size() == 0 && albumGainItems.size() == 0 )
        updateTimer.stop();
}

void Convert::kill( FileListItem *fileListItem )
{
    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->fileListItem == fileListItem )
        {
            items.at(i)->killed = true;

            if( items.at(i)->backendID != -1 && items.at(i)->backendPlugin )
            {
                items.at(i)->backendPlugin->kill( items.at(i)->backendID );
            }
            else if( items.at(i)->process.data() != 0 )
            {
                items.at(i)->process.data()->kill();
            }
            else if( items.at(i)->kioCopyJob.data() != 0 )
            {
                items.at(i)->kioCopyJob.data()->kill( KJob::EmitResult );
            }
        }
    }
}

void Convert::itemRemoved( FileListItem *fileListItem )
{
    if( !fileListItem )
        return;

    const QString albumName = fileListItem->tags ? fileListItem->tags->album : "";

    if( !albumName.isEmpty() )
    {
        QList<ConvertItem*> albumItems = albumGainItems[albumName];

        foreach( ConvertItem *albumItem, albumItems )
        {
            if( albumItem->fileListItem == fileListItem )
            {
                albumGainItems[albumName].removeAll( albumItem );
                break;
            }
        }
    }
}

void Convert::updateProgress()
{
    float time = 0.0f;
    float fileTime;
    QString fileProgressString;

    // trigger flushing of the logger cache
    pluginLog( 0, "" );

    foreach( ConvertItem *item, items )
    {
        float fileProgress = 0.0f;

        if( item->backendID != -1 && item->backendPlugin )
        {
            fileProgress = item->backendPlugin->progress( item->backendID );
        }
        else
        {
            fileProgress = item->progress;
        }

        if( fileProgress >= 0 )
        {
            fileProgressString = Global::prettyNumber(fileProgress,"%");
        }
        else
        {
            fileProgressString = i18nc("The conversion progress can't be determined","Unknown");
            fileProgress = 0; // make it a valid value so the calculations below work
        }

        fileTime = 0.0f;
        switch( item->state )
        {
            case ConvertItem::get:
            {
                fileTime = item->getTime;
                item->fileListItem->setText( 0, i18n("Getting file")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::convert:
            {
                fileTime = item->convertTimes.at(item->conversionPipesStep);
                item->fileListItem->setText( 0, i18n("Converting")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::rip:
            {
                fileTime = item->convertTimes.at(item->conversionPipesStep);
                item->fileListItem->setText( 0, i18n("Ripping")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::decode:
            {
                fileTime = item->convertTimes.at(item->conversionPipesStep);
                item->fileListItem->setText( 0, i18n("Decoding")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::filter:
            {
                fileTime = item->convertTimes.at(item->conversionPipesStep);
                item->fileListItem->setText( 0, i18n("Filter")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::encode:
            {
                fileTime = item->convertTimes.at(item->conversionPipesStep);
                item->fileListItem->setText( 0, i18n("Encoding")+"... "+fileProgressString );
                break;
            }
            case ConvertItem::replaygain:
            {
                const QString albumName = item->fileListItem->tags ? item->fileListItem->tags->album : "";
                QList<ConvertItem*> albumItems;
                if( !albumName.isEmpty() )
                    albumItems = albumGainItems[albumName];
                if( !albumItems.contains(item) )
                    albumItems.append( item );
                foreach( ConvertItem *albumItem, albumItems )
                {
                    fileTime += albumItem->replaygainTime;
                }
                item->fileListItem->setText( 0, i18n("Replay Gain")+"... "+fileProgressString );
                break;
            }
        }
        time += item->finishedTime + fileProgress * fileTime / 100.0f;
        logger->log( item->logID, "<pre>\t<span style=\"color:#585858\">" + i18n("Progress: %1",fileProgress) + "</span></pre>" );
    }

    emit updateTime( time );
}

