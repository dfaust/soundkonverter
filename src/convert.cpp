
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


Convert::Convert( Config *_config, FileList *_fileList, Logger *_logger )
    : config( _config ),
    fileList( _fileList ),
    logger( _logger )
{
    connect( fileList, SIGNAL(convertItem(FileListItem*)), this, SLOT(add(FileListItem*)) );
    connect( fileList, SIGNAL(killItem(FileListItem*)), this, SLOT(kill(FileListItem*)) );
    connect( fileList, SIGNAL(itemRemoved(FileListItem*)), this, SLOT(itemRemoved(FileListItem*)) );
    connect( this, SIGNAL(finished(FileListItem*,int)), fileList, SLOT(itemFinished(FileListItem*,int)) );
    connect( this, SIGNAL(rippingFinished(const QString&)), fileList, SLOT(rippingFinished(const QString&)) );
    connect( this, SIGNAL(finishedProcess(int,int)), logger, SLOT(processCompleted(int,int)) );

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
        remove( item, -1 );

    logger->log( item->logID, i18n("Getting file") );
    item->state = ConvertItem::get;

    item->tempInputUrl = item->generateTempUrl( "download", item->inputUrl.fileName().mid(item->inputUrl.fileName().lastIndexOf(".")+1) );

    if( !updateTimer.isActive() )
        updateTimer.start( config->data.general.updateDelay );

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

    if( item->outputUrl.isEmpty() )
    {
        item->outputUrl = ( !item->fileListItem->outputUrl.url().isEmpty() ) ? OutputDirectory::makePath(OutputDirectory::uniqueFileName(item->fileListItem->outputUrl)) : OutputDirectory::makePath(OutputDirectory::uniqueFileName(OutputDirectory::calcPath(item->fileListItem,config,"",false),usedOutputNames.values()) );
        item->fileListItem->outputUrl = item->outputUrl;
        fileList->updateItem( item->fileListItem );
    }
    usedOutputNames.insert( item->logID, item->outputUrl.toLocalFile() );

    if( item->take > item->conversionPipes.count() - 1 )
    {
        logger->log( item->logID, "\t" + i18n("No more backends left to try :(") );
        remove( item, -1 );
        return;
    }

    if( item->take > 0 )
        item->updateTimes();

    item->conversionPipesStep = -1;

    if( !updateTimer.isActive() )
        updateTimer.start( config->data.general.updateDelay );

    if( item->conversionPipes.at(item->take).trunks.count() == 1 ) // conversion can be done by one plugin alone
    {
        logger->log( item->logID, i18n("Converting") );
        item->state = ConvertItem::convert;
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
            item->fileListItem->state = FileListItem::Ripping;
            item->backendID = qobject_cast<RipperPlugin*>(item->backendPlugin)->rip( item->fileListItem->device, item->fileListItem->track, item->fileListItem->tracks, item->outputUrl );
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

    if( item->backendID == -1 )
    {
        executeSameStep( item );
    }
    else if( item->backendID == -100 )
    {
        logger->log( item->logID, "\t" + i18n("Conversion failed. At least one of the used backends needs to be configured properly.") );
        remove( item, 100 );
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

        remove( item, -1 );
        return;
    }

    item->state = ConvertItem::replaygain;
    item->backendPlugin = item->replaygainPipes.at(item->take).plugin;
    KUrl::List urlList;
    for( int i=0; i<albumItems.count(); i++ )
    {
        urlList.append( albumItems.at(i)->outputUrl );
        if( albumItems.at(i) != item )
        {
            albumItems.at(i)->fileListItem->state = FileListItem::ApplyingAlbumGain;
            fileList->updateItem( albumItems.at(i)->fileListItem );
        }
    }
    item->backendID = qobject_cast<ReplayGainPlugin*>(item->backendPlugin)->apply( urlList );

    if( !updateTimer.isActive() )
        updateTimer.start( config->data.general.updateDelay );
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

    if( !item->fileListItem->tags->coversRead )
    {
        item->fileListItem->tags->covers = config->tagEngine()->readCovers( inputUrl );
        item->fileListItem->tags->coversRead = true;
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
                remove( item, 0 );
            break;
        }
        case ConvertItem::convert:
        case ConvertItem::encode:
        {
            if( item->mode & ConvertItem::replaygain )
                replaygain( item );
            else
                remove( item, 0 );
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
            remove( item, 0 );
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
                remove( item, 0 );
            break;
        }
    }
}

void Convert::executeSameStep( ConvertItem *item )
{
    item->take++;
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
        case ConvertItem::convert:
        {
            convert( item );
            return;
        }
        case ConvertItem::rip:
        case ConvertItem::decode:
        case ConvertItem::filter:
        case ConvertItem::encode: // TODO try next encoder instead of decoding again (not so easy at the moment)
        {
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

    remove( item, -1 ); // shouldn't be possible
}

void Convert::kioJobProgress( KJob *job, unsigned long percent )
{
    // search the item list for our item
    for( int i=0; i<items.count(); i++ )
    {
        if( items.at(i)->kioCopyJob.data() == job )
        {
            items.at(i)->progress = (float)percent;
        }
    }
}

void Convert::kioJobFinished( KJob *job )
{
    // search the item list for our item
    for( int i=0; i<items.count(); i++ )
    {
        if( items.at(i)->kioCopyJob.data() == job )
        {
            items.at(i)->kioCopyJob.data()->deleteLater();

            // copy was successful
            if( job->error() == 0 )
            {
                float fileTime;
                switch( items.at(i)->state )
                {
                    case ConvertItem::get:
                        fileTime = items.at(i)->getTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                if( items.at(i)->state == ConvertItem::get )
                {
                    if( !items.at(i)->fileListItem->tags )
                    {
                        items.at(i)->fileListItem->tags = config->tagEngine()->readTags( items.at(i)->tempInputUrl );
                        if( items.at(i)->fileListItem->tags )
                        {
                            logger->log( items.at(i)->logID, i18n("Read tags sucessfully") );
                        }
                        else
                        {
                            logger->log( items.at(i)->logID, i18n("Unable to read tags") );
                        }
                    }
                }
                items.at(i)->finishedTime += fileTime;
                executeNextStep( items.at(i) );
            }
            else
            {
                if( QFile::exists(items.at(i)->tempInputUrl.toLocalFile()) )
                {
                    QFile::remove(items.at(i)->tempInputUrl.toLocalFile());
                    logger->log( items.at(i)->logID, i18nc("removing file","Removing: %1",items.at(i)->tempInputUrl.toLocalFile()) );
                }
                if( QFile::exists(items.at(i)->tempInputUrl.toLocalFile()+".part") )
                {
                    QFile::remove(items.at(i)->tempInputUrl.toLocalFile()+".part");
                    logger->log( items.at(i)->logID, i18nc("removing file","Removing: %1",items.at(i)->tempInputUrl.toLocalFile()+".part") );
                }

                if( job->error() == 1 )
                {
                    remove( items.at(i), 1 );
                }
                else
                {
                    logger->log( items.at(i)->logID, i18n("An error accured. Error code: %1 (%2)",job->error(),job->errorString()) );
                    remove( items.at(i), -1 );
                }
            }
        }
    }
}

void Convert::processOutput()
{
    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->process.data() == QObject::sender() )
        {
            const QString output = items.at(i)->process.data()->readAllStandardOutput().data();

            bool logOutput = false;
            foreach( const ConversionPipeTrunk trunk, items.at(i)->conversionPipes.at(items.at(i)->take).trunks )
            {
                const float progress = trunk.plugin->parseOutput( output );

                if( progress > items.at(i)->progress )
                    items[i]->progress = progress;

                if( progress == -1 )
                    logOutput = true;
            }

            if( logOutput && !output.simplified().isEmpty() )
                logger->log( items.at(i)->logID, "<pre>\t<span style=\"color:#C00000\">" + output.trimmed().replace("\n","<br>\t") + "</span></pre>" );

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
    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->process.data() == QObject::sender() )
        {
            // FIXME crash discovered here - but no solution yet - maybe fixed by using deleteLater
            items.at(i)->process.data()->deleteLater();

            if( items.at(i)->killed )
            {
                // TODO clean up temp files, pipes, etc.
                remove( items.at(i), 1 );
                return;
            }

            if( exitCode == 0 )
            {
                float fileTime;
                switch( items.at(i)->state )
                {
                    case ConvertItem::convert:
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                    case ConvertItem::encode:
                        fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                        break;
                    case ConvertItem::replaygain:
                        fileTime = items.at(i)->replaygainTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                items.at(i)->finishedTime += fileTime;

                if( items.at(i)->state == ConvertItem::rip )
                {
                    items.at(i)->fileListItem->state = FileListItem::Converting;
                    emit rippingFinished( items.at(i)->fileListItem->device );
                }

                if( items.at(i)->internalReplayGainUsed )
                {
                    items.at(i)->mode = ConvertItem::Mode( items.at(i)->mode ^ ConvertItem::replaygain );
                }

                switch( items.at(i)->state )
                {
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                        convertNextBackend( items.at(i) );
                        break;
                    default:
                        executeNextStep( items.at(i) );
                }
            }
            else
            {
                foreach( const KUrl url, items.at(i)->tempConvertUrls )
                {
                    if( QFile::exists(url.toLocalFile()) )
                    {
                        QFile::remove( url.toLocalFile() );
                    }
                }
                if( QFile::exists(items.at(i)->outputUrl.toLocalFile()) )
                {
                    QFile::remove( items.at(i)->outputUrl.toLocalFile() );
                }

                logger->log( items.at(i)->logID, "\t" + i18n("Conversion failed. Exit code: %1",exitCode) );
                executeSameStep( items.at(i) );
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

    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->backendPlugin && items.at(i)->backendPlugin == QObject::sender() && items.at(i)->backendID == id )
        {
            items.at(i)->backendID = -1;

            if( items.at(i)->killed )
            {
                remove( items.at(i), 1 );
                return;
            }

            if( exitCode == 0 )
            {
                float fileTime;
                switch( items.at(i)->state )
                {
                    case ConvertItem::convert:
                    case ConvertItem::rip:
                    case ConvertItem::decode:
                    case ConvertItem::filter:
                    case ConvertItem::encode:
                        fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                        break;
                    case ConvertItem::replaygain:
                        fileTime = items.at(i)->replaygainTime;
                        break;
                    default:
                        fileTime = 0.0f;
                }
                items.at(i)->finishedTime += fileTime;

                if( items.at(i)->state == ConvertItem::rip )
                {
                    items.at(i)->fileListItem->state = FileListItem::Converting;
                    emit rippingFinished( items.at(i)->fileListItem->device );
                }

                if( items.at(i)->internalReplayGainUsed )
                {
                    items.at(i)->mode = ConvertItem::Mode( items.at(i)->mode ^ ConvertItem::replaygain );
                }

                executeNextStep( items.at(i) );
            }
            else
            {
                foreach( const KUrl url, items.at(i)->tempConvertUrls )
                {
                    if( QFile::exists(url.toLocalFile()) )
                    {
                        QFile::remove( url.toLocalFile() );
                    }
                }
                if( QFile::exists(items.at(i)->outputUrl.toLocalFile()) )
                {
                    QFile::remove( items.at(i)->outputUrl.toLocalFile() );
                }

                logger->log( items.at(i)->logID, "\t" + i18n("Conversion failed. Exit code: %1",exitCode) );
                executeSameStep( items.at(i) );
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

void Convert::add( FileListItem* item )
{
    KUrl fileName;
    if( item->track >= 0 )
    {
        if( item->tags )
        {
            fileName = KUrl( i18nc("identificator for the logger","CD track %1: %2 - %3",QString().sprintf("%02i",item->tags->track),item->tags->artist,item->tags->title) );
        }
        else // shouldn't be possible
        {
            fileName = KUrl( i18nc("identificator for the logger","CD track %1",item->track) );
        }
    }
    else
    {
        fileName = item->url;
    }
    logger->log( 1000, i18n("Adding new item to conversion list: '%1'",fileName.pathOrUrl()) );

    ConvertItem *newItem = new ConvertItem( item );
    items.append( newItem );

    // register at the logger
    newItem->logID = logger->registerProcess( fileName );
    logger->log( 1000, "\t" + i18n("Got log ID: %1",newItem->logID) );

    // TODO remove redundancy, logID is needed in the fileListItem
    newItem->fileListItem->logId = newItem->logID;

//     logger->log( newItem->logID, "Mime Type: " + newItem->fileListItem->mimeType );
//     if( newItem->fileListItem->tags ) logger->log( newItem->logID, i18n("Tags successfully read") );
//     else logger->log( newItem->logID, i18n("Reading tags failed") );

    // set some variables to default values
    newItem->mode = (ConvertItem::Mode)0x0000;
    newItem->state = (ConvertItem::Mode)0x0000;

    newItem->inputUrl = item->url;

    ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions(item->conversionOptionsId);
    if( !conversionOptions )
    {
        logger->log( 1000, "Convert::add(...) no ConversionOptions found" );
        remove( newItem, -1 );
        return;
    }

    if( item->track >= 0 )
    {
        logger->log( newItem->logID, "\t" + i18n("Track number: %1, device: %2",QString::number(item->track),item->device) );
    }

    newItem->conversionPipes = config->pluginLoader()->getConversionPipes( item->codecName, conversionOptions->codecName, conversionOptions->filterOptions, conversionOptions->pluginName );

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

    logger->log( newItem->logID, i18n("File system type: %1",conversionOptions->outputFilesystem) );

    newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::convert );

    if( conversionOptions->replaygain )
    {
        newItem->replaygainPipes = config->pluginLoader()->getReplayGainPipes( conversionOptions->codecName );
        newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::replaygain );
    }

    if( !newItem->inputUrl.isLocalFile() && item->track == -1 )
        newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::get );
//     if( (!newItem->inputUrl.isLocalFile() && item->track == -1) || newItem->inputUrl.url().toAscii() != newItem->inputUrl.url() )
//         newItem->mode = ConvertItem::Mode( newItem->mode | ConvertItem::get );

    newItem->updateTimes();

    // (visual) feedback
    item->state = FileListItem::Converting;

    newItem->progressedTime.start();

    // and start
    executeNextStep( newItem );
}

void Convert::remove( ConvertItem *item, int state )
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
    if( fileRatio < 0.01 && outputFileInfo.size() < 100000 && state != 1 && ( !conversionOptions || conversionOptions->codecName != "speex" ) )
    {
        exitMessage = i18n("An error occured, the output file size is less than one percent of the input file size");

        if( state == 0 )
        {
            logger->log( item->logID, i18n("Conversion failed, trying again. Exit code: -2 (%1)",exitMessage) );
            item->take = item->lastTake;
            QFile::remove( item->outputUrl.toLocalFile() );
            executeSameStep( item );
            return;
        }
    }

    if( state == 0 )
        exitMessage = i18nc("Conversion exit status","Normal exit");
    else if( state == 1 )
        exitMessage = i18nc("Conversion exit status","Aborted by the user");
    else if( state == 100 )
        exitMessage = i18nc("Conversion exit status","Backend needs configuration");
    else
        exitMessage = i18nc("Conversion exit status","An error occured");

    if( state == 0 )
    {
        writeTags( item );
    }

    if( !waitForAlbumGain && !item->fileListItem->notifyCommand.isEmpty() && ( !config->data.general.waitForAlbumGain || !conversionOptions->replaygain ) )
    {
        QList<ConvertItem*> albumItems;
        if( !albumName.isEmpty() )
            albumItems = albumGainItems[albumName];

        albumItems.append( item );

        for( int i=0; i<albumItems.count(); i++ )
        {
            QString command = albumItems.at(i)->fileListItem->notifyCommand;
            // command.replace( "%u", albumItems.at(i)->fileListItem->url );
            command.replace( "%i", albumItems.at(i)->inputUrl.toLocalFile() );
            command.replace( "%o", albumItems.at(i)->outputUrl.toLocalFile() );
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
    if( state != 0 && config->data.general.removeFailedFiles && QFile::exists(item->outputUrl.toLocalFile()) ) // FIXME gets removed when failed anyway
    {
        QFile::remove(item->outputUrl.toLocalFile());
        logger->log( item->logID, i18n("Removing partially converted output file") );
    }

    usedOutputNames.remove( item->logID );

    logger->log( item->logID, "<br>" +  i18n("Removing file from conversion list. Exit code %1 (%2)",state,exitMessage) );

    logger->log( item->logID, "\t" + i18n("Conversion time") + ": " + Global::prettyNumber(item->progressedTime.elapsed(),"ms") );
    logger->log( item->logID, "\t" + i18n("Output file size") + ": " + Global::prettyNumber(outputFileInfo.size(),"B") );
    logger->log( item->logID, "\t" + i18n("File size ratio") + ": " + Global::prettyNumber(fileRatio*100,"%") );

    emit timeFinished( item->finishedTime );

    if( item->process.data() )
        item->process.data()->deleteLater();
    if( item->kioCopyJob.data() )
        item->kioCopyJob.data()->deleteLater();

    if( !waitForAlbumGain && !albumName.isEmpty() )
    {
        QList<ConvertItem*> albumItems = albumGainItems[albumName];

        for( int i=0; i<albumItems.count(); i++ )
        {
            emit finished( albumItems.at(i)->fileListItem, 0 ); // send signal to FileList
        }
        qDeleteAll(albumItems);

        albumGainItems.remove( albumName );
    }
    emit finished( item->fileListItem, ( state == 0 && waitForAlbumGain ) ? 102 : state ); // send signal to FileList
    emit finishedProcess( item->logID, ( state == 0 && waitForAlbumGain ) ? 102 : state ); // send signal to Logger

    items.removeAll( item );

    if( !waitForAlbumGain )
        delete item;

    if( items.size() == 0 && albumGainItems.size() == 0 )
        updateTimer.stop();
}

void Convert::kill( FileListItem *item )
{
    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->fileListItem == item )
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

void Convert::itemRemoved( FileListItem *item )
{
    if( !item )
        return;

    const QString albumName = item->tags ? item->tags->album : "";

    if( !albumName.isEmpty() )
    {
        QList<ConvertItem*> albumItems = albumGainItems[albumName];

        for( int i=0; i<albumItems.count(); i++ )
        {
            if( albumItems.at(i)->fileListItem == item )
            {
                albumGainItems[albumName].removeAll( albumItems.at(i) );
                break;
            }
        }
    }
}

void Convert::updateProgress()
{
    float time = 0;
    float fileTime;
    float fileProgress;
    QString fileProgressString;

    // trigger flushing of the logger cache
    pluginLog( 0, "" );

    for( int i=0; i<items.size(); i++ )
    {
        if( items.at(i)->backendID != -1 && items.at(i)->backendPlugin )
        {
            fileProgress = items.at(i)->backendPlugin->progress( items.at(i)->backendID );
        }
        else
        {
            fileProgress = items.at(i)->progress;
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

        switch( items.at(i)->state )
        {
            case ConvertItem::get:
                fileTime = items.at(i)->getTime;
                items.at(i)->fileListItem->setText( 0, i18n("Getting file")+"... "+fileProgressString );
                break;
            case ConvertItem::convert:
                fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                items.at(i)->fileListItem->setText( 0, i18n("Converting")+"... "+fileProgressString );
                break;
            case ConvertItem::rip:
                fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                items.at(i)->fileListItem->setText( 0, i18n("Ripping")+"... "+fileProgressString );
                break;
            case ConvertItem::decode:
                fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                items.at(i)->fileListItem->setText( 0, i18n("Decoding")+"... "+fileProgressString );
                break;
            case ConvertItem::filter:
                fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                items.at(i)->fileListItem->setText( 0, i18n("Filter")+"... "+fileProgressString );
                break;
            case ConvertItem::encode:
                fileTime = items.at(i)->convertTimes.at(items.at(i)->conversionPipesStep);
                items.at(i)->fileListItem->setText( 0, i18n("Encoding")+"... "+fileProgressString );
                break;
            case ConvertItem::replaygain:
                fileTime = items.at(i)->replaygainTime;
                items.at(i)->fileListItem->setText( 0, i18n("Replay Gain")+"... "+fileProgressString );
                break;
            default:
                fileTime = 0.0f;
        }
        time += items.at(i)->finishedTime + fileProgress * fileTime / 100.0f;
        logger->log( items.at(i)->logID, "<pre>\t<span style=\"color:#585858\">" + i18n("Progress: %1",fileProgress) + "</span></pre>" );
    }

    emit updateTime( time );
}

