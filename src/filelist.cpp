
#include "filelist.h"
// // #include "filelistitem.h"
#include "config.h"
#include "logger.h"
#include "optionseditor.h"
#include "optionslayer.h"
#include "core/conversionoptions.h"
#include "outputdirectory.h"
#include "codecproblems.h"

#include <KApplication>
#include <KIcon>
#include <KAction>
// #include <kactioncollection.h>
#include <KMessageBox>
#include <KStandardDirs>
// #include <KDiskFreeSpaceInfo>
#include <kmountpoint.h>
// #include <KIO/Job>

#include <QLayout>
#include <QGridLayout>
#include <QStringList>
#include <QMenu>
#include <QFile>
#include <QResizeEvent>
#include <QDir>
#include <QProgressBar>


FileList::FileList( Logger *_logger, Config *_config, QWidget *parent )
    : QTreeWidget( parent ),
    logger( _logger ),
    config( _config )
{
    queue = false;
    optionsEditor = 0;
    tagEngine = config->tagEngine();

    setAcceptDrops( true );
    setDragEnabled( false );

    setItemDelegate( new FileListItemDelegate(this) );

    setColumnCount( 4 );
    QStringList labels;
    labels.append( i18n("State") );
    labels.append( i18n("Input") );
    labels.append( i18n("Output") );
    labels.append( i18n("Quality") );
    setHeaderLabels( labels );
//     header()->setClickEnabled( false );

    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSortingEnabled( false );

    setRootIsDecorated( false );
    setDragDropMode( QAbstractItemView::InternalMove );

    setMinimumHeight( 200 );

    QGridLayout *grid = new QGridLayout( this );
    grid->setRowStretch( 0, 1 );
    grid->setRowStretch( 2, 1 );
    grid->setColumnStretch( 0, 1 );
    grid->setColumnStretch( 2, 1 );
    pScanStatus = new QProgressBar( this );
    pScanStatus->setMinimumHeight( pScanStatus->height() );
    pScanStatus->setFormat( "%v / %m" );
    pScanStatus->hide();
    grid->addWidget( pScanStatus, 1, 1 );
    grid->setColumnStretch( 1, 2 );

    // we haven't got access to the action collection of soundKonverter, so let's create a new one
//     actionCollection = new KActionCollection( this );

    editAction = new KAction( KIcon("view-list-text"), i18n("Edit options..."), this );
    connect( editAction, SIGNAL(triggered()), this, SLOT(showOptionsEditorDialog()) );
    startAction = new KAction( KIcon("system-run"), i18n("Start conversion"), this );
    connect( startAction, SIGNAL(triggered()), this, SLOT(convertSelectedItems()) );
    stopAction = new KAction( KIcon("process-stop"), i18n("Stop conversion"), this );
    connect( stopAction, SIGNAL(triggered()), this, SLOT(killSelectedItems()) );
    removeAction = new KAction( KIcon("edit-delete"), i18n("Remove"), this );
    removeAction->setShortcut( QKeySequence::Delete );
    connect( removeAction, SIGNAL(triggered()), this, SLOT(removeSelectedItems()) );
    addAction( removeAction );
//     KAction *removeActionGlobal = new KAction( KIcon("edit-delete"), i18n("Remove"), this );
//     removeActionGlobal->setShortcut( QKeySequence::Delete );
//     connect( removeActionGlobal, SIGNAL(triggered()), this, SLOT(removeSelectedItems()) );
//     addAction( removeActionGlobal );
//     paste = new KAction( i18n("Paste"), "editpaste", 0, this, 0, actionCollection, "paste" );  // TODO paste

    contextMenu = new QMenu( this );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)) );

    connect( this, SIGNAL(itemSelectionChanged()), this, SLOT(itemsSelected()) );
}

FileList::~FileList()
{
    // NOTE no cleanup needed since it all gets cleaned up in other classes

    if( !KApplication::kApplication()->sessionSaving() )
    {
        QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/filelist_autosave.xml") );
        listFile.remove();
    }
}

void FileList::dragEnterEvent( QDragEnterEvent *event )
{
    if( event->mimeData()->hasFormat("text/uri-list") )
        event->acceptProposedAction();
}

void FileList::dropEvent( QDropEvent *event )
{
    QList<QUrl> q_urls = event->mimeData()->urls();
    KUrl::List k_urls;
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    const bool canDecodeAac = config->pluginLoader()->canDecode( "m4a/aac" );
    const bool canDecodeAlac = config->pluginLoader()->canDecode( "m4a/alac" );
    const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;

    for( int i=0; i<q_urls.size(); i++ )
    {
        QString mimeType;
        QString codecName = config->pluginLoader()->getCodecFromFile( q_urls.at(i), &mimeType, checkM4a );

        if( mimeType == "inode/directory" || config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            k_urls += q_urls.at(i);
        }
        else
        {
            if( codecName.isEmpty() && !mimeType.startsWith("audio") && !mimeType.startsWith("video") && !mimeType.startsWith("application") )
                continue;

            if( mimeType == "application/x-ole-storage" || // Thumbs.db
                mimeType == "application/x-wine-extension-ini" ||
                mimeType == "application/x-cue" ||
                mimeType == "application/x-k3b" ||
                mimeType == "application/pdf" ||
                mimeType == "application/x-trash" ||
                mimeType.startsWith("application/vnd.oasis.opendocument") ||
                mimeType.startsWith("application/vnd.openxmlformats-officedocument") ||
                mimeType.startsWith("application/vnd.sun.xml")
            )
                continue;

            fileName = KUrl(q_urls.at(i)).pathOrUrl();

            if( codecName.isEmpty() )
                codecName = mimeType;
            if( codecName.isEmpty() )
                codecName = fileName.right(fileName.length()-fileName.lastIndexOf(".")-1);

            if( problems.value(codecName).count() < 2 )
            {
                problems[codecName] += QStringList();
                problems[codecName] += QStringList();
            }
            problems[codecName][0] += fileName;
            if( !errorList.isEmpty() )
            {
                problems[codecName][1] += errorList;
            }
            else
            {
                problems[codecName][1] += i18n("This file type is unknown to soundKonverter.\nMaybe you need to install an additional soundKonverter plugin.\nYou should have a look at your distribution's package manager for this.");
            }
        }
    }

    QList<CodecProblems::Problem> problemList;
    for( int i=0; i<problems.count(); i++ )
    {
        CodecProblems::Problem problem;
        problem.codecName = problems.keys().at(i);
        if( problem.codecName != "wav" )
        {
            #if QT_VERSION >= 0x040500
                problems[problem.codecName][1].removeDuplicates();
            #else
                QStringList found;
                for( int j=0; j<problems.value(problem.codecName).at(1).count(); j++ )
                {
                    if( found.contains(problems.value(problem.codecName).at(1).at(j)) )
                    {
                        problems[problem.codecName][1].removeAt(j);
                        j--;
                    }
                    else
                    {
                        found += problems.value(problem.codecName).at(1).at(j);
                    }
                }
            #endif
            problem.solutions = problems.value(problem.codecName).at(1);
            if( problems.value(problem.codecName).at(0).count() <= 3 )
            {
                problem.affectedFiles = problems.value(problem.codecName).at(0);
            }
            else
            {
                problem.affectedFiles += problems.value(problem.codecName).at(0).at(0);
                problem.affectedFiles += problems.value(problem.codecName).at(0).at(1);
                problem.affectedFiles += i18n("... and %1 more files",problems.value(problem.codecName).at(0).count()-3);
            }
            problemList += problem;
        }
    }

    if( problemList.count() > 0 )
    {
        CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Decode, problemList, this );
        problemsDialog->exec();
    }

    if( k_urls.count() > 0 )
    {
        ConversionOptions *conversionOptions = config->data.profiles.value("soundkonverter_last_used");
        if( conversionOptions )
            optionsLayer->setCurrentConversionOptions( conversionOptions );
        optionsLayer->addUrls( k_urls );
        optionsLayer->fadeIn();
    }

    event->acceptProposedAction();
}

void FileList::resizeEvent( QResizeEvent *event )
{
    if( event->size().width() < 500 )
        return;

    setColumnWidth( Column_State, 150 );
    setColumnWidth( Column_Input, (event->size().width()-270)/2 );
    setColumnWidth( Column_Output, (event->size().width()-270)/2 );
    setColumnWidth( Column_Quality, 120 );
}

int FileList::countDir( const QString& directory, bool recursive, int count )
{
    QDir dir( directory );
    dir.setFilter( QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable );
    dir.setSorting( QDir::Unsorted );

    count += dir.count();

    if( recursive )
    {
        dir.setFilter( QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable );

        const QStringList list = dir.entryList();

        foreach( const QString fileName, list )
        {
            count = countDir( directory + "/" + fileName, recursive, count );
        }
    }

    if( tScanStatus.elapsed() > ConfigUpdateDelay * 10 )
    {
        pScanStatus->setMaximum( count );
        kapp->processEvents();
        tScanStatus.start();
    }

    return count;
}

int FileList::listDir( const QString& directory, const QStringList& filter, bool recursive, int conversionOptionsId, int count )
{
    QString codecName;

    QDir dir( directory );
    dir.setFilter( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable );
    dir.setSorting( QDir::LocaleAware );

    const QStringList list = dir.entryList();

    const bool containsAac = filter.contains("m4a/aac");
    const bool containsAlac = filter.contains("m4a/alac");
    const bool checkM4a = ( containsAac || containsAlac ) && containsAac != containsAlac;

    foreach( const QString fileName, list )
    {
        QFileInfo fileInfo( directory + "/" + fileName );

        const bool isDir = fileInfo.isDir(); // NOTE checking for isFile might not work with all file names
        if( isDir && recursive )
        {
            count = listDir( directory + "/" + fileName, filter, recursive, conversionOptionsId, count );
        }
        else if( !isDir )
        {
            count++;

            codecName = config->pluginLoader()->getCodecFromFile( directory + "/" + fileName, 0, checkM4a );

            if( filter.contains(codecName) )
            {
                addFiles( KUrl(directory + "/" + fileName), 0, "", codecName, conversionOptionsId );
            }

            if( tScanStatus.elapsed() > ConfigUpdateDelay * 10 )
            {
                pScanStatus->setValue( count );
                tScanStatus.start();
            }
        }
    }

    return count;
}

void FileList::addFiles( const KUrl::List& fileList, ConversionOptions *conversionOptions, const QString& command, const QString& _codecName, int conversionOptionsId )
{
    QString codecName;
    QString filePathName;
    QString device;

    int lastConversionOptionsId = -1;

    bool optionsLayerHidden = false; // shouldn't be necessary

    if( !conversionOptions && conversionOptionsId == -1 )
    {
        logger->log( 1000, "@addFiles: No conversion options given" );
        return;
    }

    int batchNumber = 0;
    foreach( const KUrl fileName, fileList )
    {
        if( !_codecName.isEmpty() )
        {
            codecName = _codecName;
        }
        else
        {
            QFileInfo fileInfo( fileName.toLocalFile() );
            if( fileInfo.isDir() )
            {
                if( !optionsLayerHidden && QObject::sender() == optionsLayer )
                {
                    optionsLayerHidden = true;
                    optionsLayer->hide();
                    kapp->processEvents();
                }

                //             debug
                //             logger->log( 1000, "@addFiles: adding dir: " + fileName.toLocalFile() );

                addDir( fileName, true, config->pluginLoader()->formatList(PluginLoader::Decode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)), conversionOptions );
                continue;
            }
            else
            {
                const bool canDecodeAac = config->pluginLoader()->canDecode( "m4a/aac" );
                const bool canDecodeAlac = config->pluginLoader()->canDecode( "m4a/alac" );
                const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;
                codecName = config->pluginLoader()->getCodecFromFile( fileName, 0, checkM4a );

                if( !config->pluginLoader()->canDecode(codecName) )
                    continue;
            }
        }

//         logger->log( 1000, "adding file: " + fileName.toLocalFile() + ", codec: " + codecName );

        FileListItem * const newItem = new FileListItem( this );
        if( conversionOptionsId == -1 )
        {
            if( batchNumber == 0 )
            {
                newItem->conversionOptionsId = config->conversionOptionsManager()->addConversionOptions( conversionOptions );
            }
            else
            {
                newItem->conversionOptionsId = config->conversionOptionsManager()->increaseReferences( lastConversionOptionsId );
            }
        }
        else
        {
            newItem->conversionOptionsId = config->conversionOptionsManager()->increaseReferences( conversionOptionsId );
        }

        lastConversionOptionsId = newItem->conversionOptionsId;
        newItem->codecName = codecName;
        newItem->track = -1;
        newItem->url = fileName;
        newItem->local = ( newItem->url.isLocalFile() || newItem->url.protocol() == "file" );
        newItem->tags = tagEngine->readTags( newItem->url );
        if( !newItem->tags && newItem->codecName == "wav" && newItem->local )
        {
            QFile file( newItem->url.toLocalFile() );
            newItem->length = file.size() / 176400; // assuming it's a 44100 Hz, 16 bit wave file
        }
        else
        {
            newItem->length = ( newItem->tags && newItem->tags->length > 0 ) ? newItem->tags->length : 200.0f;
        }
        newItem->notifyCommand = command;

        addTopLevelItem( newItem );
        updateItem( newItem );
        emit timeChanged( newItem->length );

        batchNumber++;

        if( batchNumber % 50 == 0 )
            kapp->processEvents();
    }

    if( !pScanStatus->isVisible() )
    {
        emit fileCountChanged( topLevelItemCount() );

        if( queue )
            convertNextItem();
    }
}

void FileList::addDir( const KUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions )
{
    if( !conversionOptions )
    {
        logger->log( 1000, "@addDir: No conversion options given" );
        return;
    }

    const int conversionOptionsId = config->conversionOptionsManager()->addConversionOptions( conversionOptions );

    pScanStatus->setValue( 0 );
    pScanStatus->setMaximum( 0 );
    pScanStatus->show(); // show the status while scanning the directories
    tScanStatus.start();

    const int count = countDir( directory.toLocalFile(), recursive );

    pScanStatus->setMaximum( count );
    kapp->processEvents();

    listDir( directory.toLocalFile(), codecList, recursive, conversionOptionsId );

    pScanStatus->hide(); // hide the status bar, when the scan is done

    emit fileCountChanged( topLevelItemCount() );

    if( queue )
        convertNextItem();
}

void FileList::addTracks( const QString& device, QList<int> trackList, int tracks, QList<TagData*> tagList, ConversionOptions *conversionOptions, const QString& notifyCommand )
{
    FileListItem *lastListItem = 0;

    if( !conversionOptions )
    {
        logger->log( 1000, "@addTracks: No conversion options given" );
        return;
    }

    for( int i=0; i<trackList.count(); i++ )
    {
        FileListItem *newItem = new FileListItem( this );
        if( i == 0 )
        {
            newItem->conversionOptionsId = config->conversionOptionsManager()->addConversionOptions( conversionOptions );
        }
        else
        {
            newItem->conversionOptionsId = config->conversionOptionsManager()->increaseReferences( lastListItem->conversionOptionsId );
        }
        lastListItem = newItem;
        newItem->codecName = "audio cd";
        newItem->notifyCommand = notifyCommand;
        newItem->track = trackList.at(i);
        newItem->tracks = tracks;
        newItem->device = device;
        newItem->tags = tagList.at(i);
        newItem->length = newItem->tags ? newItem->tags->length : 200.0f;
        addTopLevelItem( newItem );
        updateItem( newItem );
        emit timeChanged( newItem->length );
    }

    emit fileCountChanged( topLevelItemCount() );

    if( queue )
        convertNextItem();
}

void FileList::updateItem( FileListItem *item )
{
    if( !item )
        return;

    // KUrl outputUrl;
    // if( !item->outputUrl.toLocalFile().isEmpty() )
    // {
    //     outputUrl = item->outputUrl;
    // }
    // else
    // {
    //     outputUrl = OutputDirectory::calcPath( item, config );
    // }
    const KUrl outputUrl = OutputDirectory::calcPath( item, config );
    item->setText( Column_Output, outputUrl.toLocalFile() );

    removeItemWidget( item, Column_State );
    if( item->lInfo.data() )
    {
        disconnect( item->lInfo.data(), SIGNAL(linkActivated(const QString&)), this, 0 );
        delete item->lInfo.data();
    }
    item->setText( Column_State, "" );
    item->setToolTip( Column_State, "" );
    item->setToolTip( Column_Input, "" );
    item->setToolTip( Column_Output, "" );
    item->setToolTip( Column_Quality, "" );

    switch( item->state )
    {
        case FileListItem::WaitingForConversion:
        {
            if( QFile::exists(outputUrl.toLocalFile()) )
            {
                item->setText( Column_State, i18n("Will be skipped") );
            }
            else
            {
                item->setText( Column_State, i18n("Waiting") );
            }
            break;
        }
        case FileListItem::ApplyingReplayGain:
        {
            item->setText( Column_State, i18n("Replay Gain") );
            break;
        }
        case FileListItem::WaitingForAlbumGain:
        {
            item->setText( Column_State, i18n("Waiting for Replay Gain") );
            break;
        }
        case FileListItem::ApplyingAlbumGain:
        {
            item->setText( Column_State, i18n("Replay Gain") );
            break;
        }
        case FileListItem::Ripping:
        {
            item->setText( Column_State, i18n("Ripping") );
            break;
        }
        case FileListItem::Converting:
        {
            item->setText( Column_State, i18n("Converting") );
            break;
        }
        case FileListItem::Stopped:
        {
            switch( item->returnCode )
            {
                case FileListItem::Succeeded:
                case FileListItem::SucceededWithProblems:
                {
                    break;
                }
                case FileListItem::StoppedByUser:
                {
                    item->setText( Column_State, i18n("Stopped") );
                    break;
                }
                case FileListItem::BackendNeedsConfiguration:
                {
                    item->setText( Column_State, i18n("Backend not configured") );
                    break;
                }
                case FileListItem::DiscFull:
                {
                    item->setText( Column_State, i18n("Disc full") );
                    break;
                }
                case FileListItem::Skipped:
                {
                    item->setText( Column_State, i18n("Will be skipped") );
                    break;
                }
                case FileListItem::Failed:
                {
                    item->lInfo = new QLabel( "<a href=\"" + QString::number(item->logId) + "\">" + i18n("Failed") + "</a>" );
                    connect( item->lInfo.data(), SIGNAL(linkActivated(const QString&)), this, SLOT(showLogClicked(const QString&)) );
                    setItemWidget( item, Column_State, item->lInfo.data() );
                    const QString toolTip = i18n("The conversion has failed.\nSee the log for more information.");
                    item->setToolTip( Column_State, toolTip );
                    item->setToolTip( Column_Input, toolTip );
                    item->setToolTip( Column_Output, toolTip );
                    item->setToolTip( Column_Quality, toolTip );
                    break;
                }
            }
            break;
        }
    }

    ConversionOptions *options = config->conversionOptionsManager()->getConversionOptions(item->conversionOptionsId);
    if( options )
        item->setText( Column_Quality, options->profile );

    if( item->track >= 0 )
    {
        if( item->tags )
        {
            item->setText( Column_Input, QString().sprintf("%02i",item->tags->track) + " - " + item->tags->artist + " - " + item->tags->title );
        }
        else // shouldn't be possible
        {
            item->setText( Column_Input, i18n("CD track %1").arg(item->track) );
        }
    }
    else
    {
        item->setText( Column_Input, item->url.pathOrUrl() );
    }
}

void FileList::showLogClicked( const QString& logIdString )
{
    const int logId = logIdString.toInt();

    if( !logId )
        return;

    emit showLog( logId );
}

void FileList::updateItems( QList<FileListItem*> items )
{
    for( int i=0; i<items.size(); i++ )
    {
        updateItem( items.at(i) );
    }
}

void FileList::updateAllItems()
{
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        updateItem( topLevelItem(i) );
    }
}

void FileList::startConversion()
{
    // iterate through all items and set the state to "Waiting"
    FileListItem *item;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        bool isStopped = false;
        if( item )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    break;
                case FileListItem::Ripping:
                    break;
                case FileListItem::Converting:
                    break;
                case FileListItem::ApplyingReplayGain:
                    break;
                case FileListItem::WaitingForAlbumGain:
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    isStopped = true;
                    break;
            }
        }
        if( isStopped )
        {
            item->state = FileListItem::WaitingForConversion;
            updateItem( item );
        }
    }

    queue = true;
    emit queueModeChanged( queue );
    emit conversionStarted();
    convertNextItem();
}

void FileList::killConversion()
{
    queue = false;
    emit queueModeChanged( queue );

    FileListItem *item;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        bool canKill = false;
        if( item )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    break;
                case FileListItem::Ripping:
                    canKill = true;
                    break;
                case FileListItem::Converting:
                    canKill = true;
                    break;
                case FileListItem::ApplyingReplayGain:
                    canKill = true;
                    break;
                case FileListItem::WaitingForAlbumGain:
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    break;
            }
        }
        if( canKill )
            emit killItem( item );
    }
}

void FileList::stopConversion()
{
    queue = false;
    emit queueModeChanged( queue );
}

void FileList::continueConversion()
{
    queue = true;
    emit queueModeChanged( queue );

    convertNextItem();
}

void FileList::convertNextItem()
{
    if( !queue )
        return;

    int count = convertingCount();
    bool callItemsSelected = false;
    QStringList devices;

    // look for tracks that are being ripped
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        FileListItem *item = topLevelItem( i );
        if( item->state == FileListItem::Ripping )
        {
            devices += item->device;
        }
    }

    // look for waiting files
    for( int i=0; i<topLevelItemCount() && count < config->data.general.numFiles; i++ )
    {
        FileListItem *item = topLevelItem( i );
        if( item->state == FileListItem::WaitingForConversion )
        {
            if( item->track >= 0 && !devices.contains(item->device) )
            {
                count++;
                devices += item->device;
                emit convertItem( item );
                if( selectedFiles.contains(item) )
                    callItemsSelected = true;
            }
            else if( item->track < 0 )
            {
                count++;
                emit convertItem( item );
                if( selectedFiles.contains(item) )
                    callItemsSelected = true;
            }
        }
    }

    if( callItemsSelected )
        itemsSelected();

    if( count == 0 )
        itemFinished( 0, FileListItem::Succeeded );
}

int FileList::waitingCount()
{
    int count = 0;
    FileListItem *item;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        if( item->state == FileListItem::WaitingForConversion )
            count++;
    }

    return count;
}

int FileList::convertingCount() // TODO use Convert
{
    int count = 0;
    FileListItem *item;
    QString albumName;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        bool isConverting = false;
        switch( item->state )
        {
            case FileListItem::WaitingForConversion:
                break;
            case FileListItem::Ripping:
                isConverting = true;
                break;
            case FileListItem::Converting:
                isConverting = true;
                break;
            case FileListItem::ApplyingReplayGain:
                isConverting = true;
                break;
            case FileListItem::WaitingForAlbumGain:
                break;
            case FileListItem::ApplyingAlbumGain:
                break;
            case FileListItem::Stopped:
                break;
        }
        if( isConverting )
            count++;
    }

    return count;
}

// qulonglong FileList::spaceLeftForDirectory( const QString& dir )
// {
//     if( dir.isEmpty() )
//         return 0;
//
//     KMountPoint::Ptr mp = KMountPoint::currentMountPoints().findByPath( dir );
//     if( !mp )
//         return 0;
//
//     KDiskFreeSpaceInfo job = KDiskFreeSpaceInfo::freeSpaceInfo( mp->mountPoint() );
//     if( !job.isValid() )
//         return 0;
//
//     KIO::filesize_t mBSize = job.size() / 1024 / 1024;
//     KIO::filesize_t mBUsed = job.used() / 1024 / 1024;
//
//     return mBSize - mBUsed;
// }

void FileList::itemFinished( FileListItem *item, FileListItem::ReturnCode returnCode, bool waitingForAlbumGain )
{
    if( item )
    {
        if( waitingForAlbumGain )
        {
            item->state = FileListItem::WaitingForAlbumGain;

            item->returnCode = returnCode;
            updateItem( item );
        }
        else if( returnCode == FileListItem::Succeeded || returnCode == FileListItem::SucceededWithProblems )
        {
            config->conversionOptionsManager()->removeConversionOptions( item->conversionOptionsId );
            if( selectedFiles.contains(item) )
                itemsSelected();
            delete item;
            item = 0;
        }
        else
        {
            item->state = FileListItem::Stopped;

            item->returnCode = returnCode;
            updateItem( item );
        }
    }

    // FIXME disabled until saving gets faster
//     save( false );

    if( queue && waitingCount() > 0 )
    {
        convertNextItem();
    }
    else if( convertingCount() == 0 )
    {
        queue = false;
        save( false );
        emit queueModeChanged( queue );
//         float time = 0;
//         for( int i=0; i<topLevelItemCount(); i++ )
//         {
//             FileListItem *temp_item = topLevelItem( i );
//             updateItem( temp_item ); // TODO why?
//             time += temp_item->length;
//         }
//         emit finished( time );
        emit finished( true );
        emit conversionStopped( returnCode != FileListItem::Succeeded );
        emit fileCountChanged( topLevelItemCount() );
    }
}

bool FileList::waitForAlbumGain( FileListItem *item ) // TODO was ist wenn die datei unten drunter schon fertig ist, was ist wenn eine datei oben drpber noch nicht fertig ist
{
    if( !config->data.general.waitForAlbumGain )
        return false;

    if( !item || !item->tags )
        return false;

    if( item->tags->album.isEmpty() )
        return false;

    FileListItem *nextItem;

    nextItem = item;
    while( ( nextItem = (FileListItem*)itemAbove(nextItem) ) )
    {
        if( nextItem && nextItem->tags && nextItem->tags->album == item->tags->album )
        {
            if( nextItem->state != FileListItem::WaitingForAlbumGain )
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    nextItem = item;
    while( ( nextItem = (FileListItem*)itemBelow(nextItem) ) )
    {
        if( nextItem && nextItem->tags && nextItem->tags->album == item->tags->album )
        {
            if( nextItem->state != FileListItem::WaitingForAlbumGain )
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    return false;
}

void FileList::rippingFinished( const QString& device )
{
    if( waitingCount() > 0 && queue )
    {
        // look for waiting files
        for( int i=0; i<topLevelItemCount(); i++ )
        {
            FileListItem *item = topLevelItem( i );
            if( item->state == FileListItem::WaitingForConversion )
            {
                if( item->track >= 0 && item->device == device )
                {
                    emit convertItem( item );
                    if( selectedFiles.contains(item) )
                        itemsSelected();
                    return;
                }
            }
        }
    }
}

void FileList::showContextMenu( const QPoint& point )
{
    const FileListItem * const item = (FileListItem*)itemAt( point );

    // if item is null, we can abort here
    if( !item )
        return;

    // add a tilte to our context manu
    //contextMenu->insertTitle( static_cast<FileListItem*>(item)->fileName ); // TODO sqeeze or something else

    // TODO implement pasting, etc.

    contextMenu->clear();

    // is this file (of our item) beeing converted at the moment?
    bool isConverting = false;
    if( item )
    {
        switch( item->state )
        {
            case FileListItem::WaitingForConversion:
                break;
            case FileListItem::Ripping:
                isConverting = true;
                break;
            case FileListItem::Converting:
                isConverting = true;
                break;
            case FileListItem::ApplyingReplayGain:
                isConverting = true;
                break;
            case FileListItem::WaitingForAlbumGain:
                break;
            case FileListItem::ApplyingAlbumGain:
                break;
            case FileListItem::Stopped:
                break;
        }
    }
    if( item->state == FileListItem::ApplyingAlbumGain )
    {
    }
    if( item->state == FileListItem::WaitingForAlbumGain )
    {
        contextMenu->addAction( removeAction );
    }
    else if( isConverting )
    {
        //contextMenu->addAction( paste );
        //contextMenu->addSeparator();
        contextMenu->addAction( stopAction );
    }
    else
    {
        contextMenu->addAction( editAction );
        contextMenu->addSeparator();
        contextMenu->addAction( removeAction );
        //contextMenu->addAction( paste );
        contextMenu->addSeparator();
        contextMenu->addAction( startAction );
    }

    // show the popup menu
    contextMenu->popup( viewport()->mapToGlobal(point) );
}

void FileList::showOptionsEditorDialog()
{
    if( !optionsEditor )
    {
        optionsEditor = new OptionsEditor( config, this );
        if( !optionsEditor )
        {
             // TODO error message
            return;
        }
        connect( this, SIGNAL(editItems(QList<FileListItem*>)), optionsEditor, SLOT(itemsSelected(QList<FileListItem*>)) );
        connect( this, SIGNAL(setPreviousItemEnabled(bool)), optionsEditor, SLOT(setPreviousEnabled(bool)) );
        connect( this, SIGNAL(setNextItemEnabled(bool)), optionsEditor, SLOT(setNextEnabled(bool)) );
        connect( this, SIGNAL(itemRemoved(FileListItem*)), optionsEditor, SLOT(itemRemoved(FileListItem*)) );
        connect( optionsEditor, SIGNAL(user2Clicked()), this, SLOT(selectPreviousItem()) );
        connect( optionsEditor, SIGNAL(user1Clicked()), this, SLOT(selectNextItem()) );
        connect( optionsEditor, SIGNAL(updateFileListItems(QList<FileListItem*>)), this, SLOT(updateItems(QList<FileListItem*>)) );
    }
    itemsSelected();
    optionsEditor->show();
}

void FileList::selectPreviousItem()
{
    QTreeWidgetItem * const item = itemAbove( selectedItems().first() );

    if( !item )
        return;

    disconnect( this, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedFiles.count(); i++ )
    {
        selectedFiles.at(i)->setSelected( false );
    }

    item->setSelected( true );
    scrollToItem( item );

    connect( this, SIGNAL(itemSelectionChanged()), this, SLOT(itemsSelected()) );

    itemsSelected();
}

void FileList::selectNextItem()
{
    QTreeWidgetItem * const item = itemBelow( selectedItems().last() );

    if( !item )
        return;

    disconnect( this, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedFiles.count(); i++ )
    {
        selectedFiles.at(i)->setSelected( false );
    }

    item->setSelected( true );
    scrollToItem( item );

    connect( this, SIGNAL(itemSelectionChanged()), this, SLOT(itemsSelected()) );

    itemsSelected();
}

void FileList::removeSelectedItems()
{
    FileListItem *item;
    QList<QTreeWidgetItem*> items = selectedItems();
    for( int i=0; i<items.size(); i++ )
    {
        item = (FileListItem*)items.at(i);
        bool canRemove = false;
        if( item && item->isSelected() )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    canRemove = true;
                    break;
                case FileListItem::Ripping:
                    break;
                case FileListItem::Converting:
                    break;
                case FileListItem::ApplyingReplayGain:
                    break;
                case FileListItem::WaitingForAlbumGain:
                    canRemove = true;
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    canRemove = true;
                    break;
            }
            if( canRemove )
            {
                emit timeChanged( -item->length );
                config->conversionOptionsManager()->removeConversionOptions( item->conversionOptionsId );
                delete item;
            }
        }
    }
    emit fileCountChanged( topLevelItemCount() );

    itemsSelected();
}

void FileList::convertSelectedItems()
{
    bool started = false;
    bool callItemsSelected = false;
    FileListItem *item;
    QList<QTreeWidgetItem*> items = selectedItems();
    for( int i=0; i<items.size(); i++ )
    {
        item = (FileListItem*)items.at(i);
        bool canConvert = false;
        if( item )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    canConvert = true;
                    break;
                case FileListItem::Ripping:
                    break;
                case FileListItem::Converting:
                    break;
                case FileListItem::ApplyingReplayGain:
                    break;
                case FileListItem::WaitingForAlbumGain:
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    canConvert = true;
                    break;
            }
        }
        if( canConvert )
        {
            // emit conversionStarted first because in case the plugin reports an error the conversion stop immediately
            // and the itemFinished slot gets called. If conversionStarted was emitted at the end of this function
            // it might broadcast the message that the conversion has started after the conversion already stopped
            if( !started )
                emit conversionStarted();

            emit convertItem( item );

            if( selectedFiles.contains(item) )
                callItemsSelected = true;

            started = true;
        }
    }

    if( callItemsSelected )
        itemsSelected();
}

void FileList::killSelectedItems()
{
    FileListItem *item;
    QList<QTreeWidgetItem*> items = selectedItems();
    for( int i=0; i<items.size(); i++ )
    {
        item = (FileListItem*)items.at(i);
        bool canKill = false;
        if( item )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    break;
                case FileListItem::Ripping:
                    canKill = true;
                    break;
                case FileListItem::Converting:
                    canKill = true;
                    break;
                case FileListItem::ApplyingReplayGain:
                    canKill = true;
                    break;
                case FileListItem::WaitingForAlbumGain:
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    break;
            }
        }
        if( canKill )
            emit killItem( item );
    }
}

void FileList::itemsSelected()
{
    selectedFiles.clear();

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach( QTreeWidgetItem* item, items )
    {
        selectedFiles.append( (FileListItem*)item );
    }

    if( selectedFiles.count() > 0 )
    {
        emit setPreviousItemEnabled( itemAbove(selectedFiles.first()) != 0 );
        emit setNextItemEnabled( itemBelow(selectedFiles.last()) != 0 );
    }
    else
    {
        emit setPreviousItemEnabled( false );
        emit setNextItemEnabled( false );
    }

    emit editItems( selectedFiles );
}

void FileList::load( bool user )
{
    if( topLevelItemCount() > 0 )
    {
        const int ret = KMessageBox::questionYesNo( this, i18n("Do you want to overwrite the current file list?\n\nIf not, the saved file list will be appended.") );
        if( ret == KMessageBox::Yes )
        {
            FileListItem *item;
            for( int i=0; i<topLevelItemCount(); i++ )
            {
                item = topLevelItem(i);
                bool canRemove = false;
                if( item )
                {
                    switch( item->state )
                    {
                        case FileListItem::WaitingForConversion:
                            canRemove = true;
                            break;
                        case FileListItem::Ripping:
                            break;
                        case FileListItem::Converting:
                            break;
                        case FileListItem::ApplyingReplayGain:
                            break;
                        case FileListItem::WaitingForAlbumGain:
                            break;
                        case FileListItem::ApplyingAlbumGain:
                            break;
                        case FileListItem::Stopped:
                            canRemove = true;
                            break;
                    }
                }
                if( canRemove )
                {
                    config->conversionOptionsManager()->removeConversionOptions( item->conversionOptionsId );
                    delete item;
                    i--;
                }
            }

            itemsSelected();
        }
    }

    QMap<int,int> conversionOptionsIds;
    QMap<int,int> conversionOptionsReferences;
    QString fileName = user ? "filelist.xml" : "filelist_autosave.xml";
    QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/"+fileName) );
    if( listFile.open( QIODevice::ReadOnly ) )
    {
        QDomDocument list("soundkonverter_filelist");
        if( list.setContent( &listFile ) )
        {
            QDomElement root = list.documentElement();
            if( root.nodeName() == "soundkonverter" && root.attribute("type") == "filelist" )
            {
                QDomNodeList conversionOptionsElements = root.elementsByTagName("conversionOptions");
                for( int i=0; i<conversionOptionsElements.count(); i++ )
                {
                    ConversionOptions *conversionOptions = 0;
                    QList<QDomElement> filterOptionsElements;
                    const QString pluginName = conversionOptionsElements.at(i).toElement().attribute("pluginName");
                    CodecPlugin *plugin = (CodecPlugin*)config->pluginLoader()->backendPluginByName( pluginName );
                    if( plugin )
                    {
                        conversionOptions = plugin->conversionOptionsFromXml( conversionOptionsElements.at(i).toElement(), &filterOptionsElements );
                    }
                    else
                    {
                        continue;
                    }
                    if( conversionOptions )
                    {
                        foreach( QDomElement filterOptionsElement, filterOptionsElements )
                        {
                            FilterOptions *filterOptions = 0;
                            const QString filterPluginName = filterOptionsElement.attribute("pluginName");
                            FilterPlugin *filterPlugin = (FilterPlugin*)config->pluginLoader()->backendPluginByName( filterPluginName );
                            if( filterPlugin )
                            {
                                filterOptions = filterPlugin->filterOptionsFromXml( filterOptionsElement );
                            }
                            else
                            {
                                continue;
                            }
                            conversionOptions->filterOptions.append( filterOptions );
                        }
                    }
                    const int id = conversionOptionsElements.at(i).toElement().attribute("id").toInt();
                    conversionOptionsIds[id] = config->conversionOptionsManager()->addConversionOptions( conversionOptions );
                    conversionOptionsReferences[conversionOptionsIds[id]] = 0;
                }
                QDomNodeList files = root.elementsByTagName("file");
                pScanStatus->setValue( 0 );
                pScanStatus->setMaximum( files.count() );
                pScanStatus->show();
                tScanStatus.start();
                for( int i=0; i<files.count(); i++ )
                {
                    QDomElement file = files.at(i).toElement();
                    FileListItem *item = new FileListItem( this );
                    item->url = KUrl(file.attribute("url"));
                    // item->outputUrl = KUrl(file.attribute("outputUrl"));
                    item->codecName = file.attribute("codecName");
                    item->conversionOptionsId = conversionOptionsIds[file.attribute("conversionOptionsId").toInt()]; // TODO check if id exists
                    item->local = file.attribute("local").toInt();
                    item->track = file.attribute("track").toInt();
                    item->tracks = file.attribute("tracks").toInt();
                    item->device = file.attribute("device");
                    item->length = file.attribute("time").toInt();
                    item->notifyCommand = file.attribute("notifyCommand");
                    if( conversionOptionsReferences[item->conversionOptionsId] != 0 )
                        config->conversionOptionsManager()->increaseReferences( item->conversionOptionsId );
                    conversionOptionsReferences[item->conversionOptionsId]++;
                    if( file.elementsByTagName("tags").count() > 0 )
                    {
                        QDomElement tags = file.elementsByTagName("tags").at(0).toElement();
                        item->tags = new TagData();
                        item->tags->artist = tags.attribute("artist");
                        item->tags->composer = tags.attribute("composer");
                        item->tags->album = tags.attribute("album");
                        item->tags->title = tags.attribute("title");
                        item->tags->genre = tags.attribute("genre");
                        item->tags->comment = tags.attribute("comment");
                        item->tags->track = tags.attribute("track").toInt();
                        item->tags->trackTotal = tags.attribute("trackTotal").toInt();
                        item->tags->disc = tags.attribute("disc").toInt();
                        item->tags->discTotal = tags.attribute("discTotal").toInt();
                        item->tags->year = tags.attribute("year").toInt();
                        item->tags->trackGain = tags.attribute("track_gain").toFloat();
                        item->tags->albumGain = tags.attribute("album_gain").toFloat();
                        item->tags->length = tags.attribute("length").toInt();
                        item->tags->samplingRate = tags.attribute("samplingRate").toInt();
                    }
                    addTopLevelItem( item );
                    updateItem( item );
                    emit timeChanged( item->length );
                    if( tScanStatus.elapsed() > ConfigUpdateDelay * 10 )
                    {
                        pScanStatus->setValue( i );
                        tScanStatus.start();
                    }
                }
                pScanStatus->hide();
            }
        }
        listFile.close();
        emit fileCountChanged( topLevelItemCount() );
    }
}

void FileList::save( bool user )
{
    // assume it's a misclick if there's nothing to save
    if( user && topLevelItemCount() == 0 )
        return;

    QTime time;
    time.start();

    QDomDocument list("soundkonverter_filelist");
    QDomElement root = list.createElement("soundkonverter");
    root.setAttribute("type","filelist");
    list.appendChild(root);

    QList<int> ids = config->conversionOptionsManager()->getAllIds();
    for( int i=0; i<ids.size(); i++ )
    {
        if( !config->conversionOptionsManager()->getConversionOptions(ids.at(i)) )
        {
            // FIXME error message, null pointer for conversion options
            continue;
        }
        QDomElement conversionOptions = config->conversionOptionsManager()->getConversionOptions(ids.at(i))->toXml(list);
        conversionOptions.setAttribute("id",ids.at(i));
        root.appendChild(conversionOptions);
    }

    FileListItem *item;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);

        QDomElement file = list.createElement("file");
        file.setAttribute("url",item->url.pathOrUrl());
        // file.setAttribute("outputUrl",item->outputUrl.pathOrUrl());
        file.setAttribute("codecName",item->codecName);
        file.setAttribute("conversionOptionsId",item->conversionOptionsId);
        file.setAttribute("local",item->local);
        file.setAttribute("track",item->track);
        file.setAttribute("tracks",item->tracks);
        file.setAttribute("device",item->device);
        file.setAttribute("time",item->length);
        file.setAttribute("notifyCommand",item->notifyCommand);
        root.appendChild(file);
        if( item->tags )
        {
            QDomElement tags = list.createElement("tags");
            tags.setAttribute("artist",item->tags->artist);
            tags.setAttribute("composer",item->tags->composer);
            tags.setAttribute("album",item->tags->album);
            tags.setAttribute("title",item->tags->title);
            tags.setAttribute("genre",item->tags->genre);
            tags.setAttribute("comment",item->tags->comment);
            tags.setAttribute("track",item->tags->track);
            tags.setAttribute("trackTotal",item->tags->trackTotal);
            tags.setAttribute("disc",item->tags->disc);
            tags.setAttribute("discTotal",item->tags->discTotal);
            tags.setAttribute("year",item->tags->year);
            tags.setAttribute("track_gain",item->tags->trackGain);
            tags.setAttribute("album_gain",item->tags->albumGain);
            tags.setAttribute("length",item->tags->length);
            tags.setAttribute("samplingRate",item->tags->samplingRate);
            file.appendChild(tags);
        }
    }

    const QString fileName = user ? "filelist.xml" : "filelist_autosave.xml";
    QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/"+fileName) );
    if( listFile.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream(&listFile);
        stream << list.toString();
        listFile.close();
    }

    logger->log( 1000, QString("Saving the file list took %1 ms").arg(time.elapsed()) );
}


