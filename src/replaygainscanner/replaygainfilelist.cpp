
#include "replaygainfilelist.h"

#include "logger.h"
#include "config.h"
#include "codecproblems.h"

#include <QResizeEvent>
#include <QGridLayout>
#include <QProgressBar>
#include <KMessageBox>
#include <QDir>
#include <QFileInfo>
#include <KAction>
#include <QMenu>


ReplayGainFileList::ReplayGainFileList( Config *_config, Logger *_logger, QWidget *parent )
    : QTreeWidget( parent )
{
    config = _config;
    logger = _logger;

    queue = false;
    killed = false;

    lastAlbumItem = 0;

    setAcceptDrops( true );
    setDragEnabled( true );

    setItemDelegate( new ReplayGainFileListItemDelegate(this) );

    setColumnCount( 3 );
    QStringList labels;
    labels.append( i18n("File") );
    labels.append( i18n("Track") );
    labels.append( i18n("Album") );
    setHeaderLabels( labels );
//     header()->setClickEnabled( false );

    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSortingEnabled( false );

    setDragDropMode( QAbstractItemView::InternalMove );

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

    collapseAction = new KAction( KIcon("view-process-all"), i18n("Collapse all"), this );
    collapseAction->setShortcut( Qt::CTRL | Qt::Key_Minus );
    connect( collapseAction, SIGNAL(triggered()), this, SLOT(collapseAll()) );
    addAction( collapseAction );
    expandAction = new KAction( KIcon("view-process-all-tree"), i18n("Expand all"), this );
    expandAction->setShortcut( Qt::CTRL | Qt::Key_Plus );
    connect( expandAction, SIGNAL(triggered()), this, SLOT(expandAll()) );
    addAction( expandAction );
//     startAction = new KAction( KIcon("system-run"), i18n("Calculate Replay Gain"), this );
//     connect( startAction, SIGNAL(triggered()), this, SLOT(convertSelectedItems()) );
//     stopAction = new KAction( KIcon("process-stop"), i18n("Stop Calculation"), this );
//     connect( stopAction, SIGNAL(triggered()), this, SLOT(killSelectedItems()) );
    removeAction = new KAction( KIcon("edit-delete"), i18n("Remove"), this );
    removeAction->setShortcut( QKeySequence::Delete );
    connect( removeAction, SIGNAL(triggered()), this, SLOT(removeSelectedItems()) );
    addAction( removeAction );
//     paste = new KAction( i18n("Paste"), "editpaste", 0, this, 0, actionCollection, "paste" );  // TODO paste
//     newAction = new KAction( KIcon("file-new"), i18n("New album"), this );
//     connect( newAction, SIGNAL(triggered()), this, SLOT(newAlbum()) );

    contextMenu = new QMenu( this );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)) );

    QList<ReplayGainPlugin*> replayGainPlugins = config->pluginLoader()->getAllReplayGainPlugins();
    for( int i=0; i<replayGainPlugins.size(); i++ )
    {
        connect( replayGainPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( replayGainPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }

    connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateProgress()) );
}

ReplayGainFileList::~ReplayGainFileList()
{}

void ReplayGainFileList::dragEnterEvent( QDragEnterEvent *event )
{
    if( event->mimeData()->hasFormat("text/uri-list") || event->source() == this )
        event->acceptProposedAction();
}

void ReplayGainFileList::dragMoveEvent( QDragMoveEvent *event )
{
    if( itemAt(event->pos()) && itemAt(event->pos()) && static_cast<ReplayGainFileListItem*>(itemAt(event->pos()))->type!=ReplayGainFileListItem::Track )
        QTreeWidget::dragMoveEvent(event);
}

void ReplayGainFileList::dropEvent( QDropEvent *event )
{
    QList<QUrl> q_urls = event->mimeData()->urls();
    KUrl::List k_urls;
    KUrl::List k_urls_dirs;
    QStringList errorList;
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    if( event->source() == this )
    {
        QTreeWidgetItem *destination = itemAt(event->pos());

        if( !destination || static_cast<ReplayGainFileListItem*>(destination)->type!=ReplayGainFileListItem::Track )
        {
//             QList<QTreeWidgetItem*> parents;
//             QList<QTreeWidgetItem*> children = selectedItems();
//             for( int i=0; i<children.count(); i++ )
//             {
//                 if( children.at(i)->parent() && static_cast<ReplayGainFileListItem*>(children.at(i))->parent()->type()==ReplayGainFileListItem::Album )
//                 {
//                     parents += children.at(i)->parent();
//                 }
//             }
            QTreeWidget::dropEvent(event);
//             for( int i=0; i<parents.count(); i++ )
//             {
//                 if( parents.at(i)->childCount() == 0 )
//                 {
//                     delete parents.at(i);
//                 }
//             }
        }
//         else if( destination && static_cast<ReplayGainFileListItem*>(destination)->type==ReplayGainFileListItem::Track )
//         {
//             QList<QTreeWidgetItem*> sources = selectedItems();
//             for( int i=0; i<sources.count(); i++ )
//             {
//                 if( sources.at(i)->parent() && static_cast<ReplayGainFileListItem*>(sources.at(i))->parent()->type()==ReplayGainFileListItem::Album )
//                 {
//                     for( int j=0; j<sources.at(i)->parent()->childCount(); j++ )
//                     {
//                         if( sources.at(i)->parent()->child(j) == sources.at(i) ) destination->parent()->addChild( sources.at(i)->parent()->takeChild(j) );
//                     }
//                 }
//                 else
//                 {
//                     for( int j=0; j<topLevelItemCount(); j++ )
//                     {
//                         if( topLevelItem(j) == sources.at(i) ) destination->parent()->addChild( takeTopLevelItem(j) );
//                     }
//                 }
//             }
//         }

        for( int i=0; i<topLevelItemCount(); i++ )
        {
            ReplayGainFileListItem *item = topLevelItem(i);
            if( item->type == ReplayGainFileListItem::Album )
            {
                if( item->childCount() == 0 )
                {
                    delete item;
                    i--;
                }
            }
        }
    }
    else
    {
        for( int i=0; i<q_urls.size(); i++ )
        {
            QString mimeType;
            QString codecName = config->pluginLoader()->getCodecFromFile( q_urls.at(i), &mimeType );

            if( mimeType == "inode/directory" )
            {
                k_urls_dirs += q_urls.at(i);
            }
            else if( config->pluginLoader()->canReplayGain(codecName,0,&errorList) )
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
            CodecProblems *problemsDialog = new CodecProblems( CodecProblems::ReplayGain, problemList, this );
            problemsDialog->exec();
        }

        if( k_urls.count() > 0 )
        {
            addFiles( k_urls );
        }
        for( int i=0; i<k_urls_dirs.count(); i++ )
        {
            addDir( k_urls_dirs.at(i), true, config->pluginLoader()->formatList(PluginLoader::ReplayGain,PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
        }

        event->acceptProposedAction();
    }
}

void ReplayGainFileList::resizeEvent( QResizeEvent *event )
{
    if( event->size().width() < 300 )
        return;

    setColumnWidth( Column_File, event->size().width()-160 );
    setColumnWidth( Column_Track, 80 );
    setColumnWidth( Column_Album, 80 );
}

int ReplayGainFileList::listDir( const QString& directory, const QStringList& filter, bool recursive, bool fast, int count )
{
    QString codecName;

    QDir dir( directory );
    dir.setFilter( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable );
    dir.setSorting( QDir::LocaleAware );

    const QStringList list = dir.entryList();

    foreach( const QString fileName, list )
    {
        QFileInfo fileInfo( directory + "/" + fileName );

        if( fileInfo.isDir() && recursive )
        {
            count = listDir( directory + "/" + fileName, filter, recursive, fast, count );
        }
        else if( !fileInfo.isDir() ) // NOTE checking for isFile may not work with all file names
        {
            count++;

            if( fast )
            {
                pScanStatus->setMaximum( count );
            }
            else
            {
                codecName = config->pluginLoader()->getCodecFromFile( directory + "/" + fileName );

                if( filter.count() == 0 || filter.contains(codecName) )
                {
                    addFiles( KUrl(directory + "/" + fileName), codecName );
                    if( tScanStatus.elapsed() > config->data.general.updateDelay * 10 )
                    {
                        pScanStatus->setValue( count );
                        tScanStatus.start();
                    }
                }
            }
        }
    }

    return count;
}

void ReplayGainFileList::addFiles( const KUrl::List& fileList, const QString& _codecName )
{
    ReplayGainFileListItem *newAlbumItem, *newItem;
    QString codecName;
    QString filePathName;
    QString device;
    QStringList unsupportedList;
    int samplingRate;
    KUrl url;

    for( int i=0; i<fileList.count(); i++ )
    {
        if( !_codecName.isEmpty() )
        {
            codecName = _codecName;
        }
        else
        {
            codecName = config->pluginLoader()->getCodecFromFile( fileList.at(i) );

            if( !config->pluginLoader()->canReplayGain(codecName,0) )
            {
                unsupportedList.append( fileList.at(i).pathOrUrl() );
                continue;
            }
        }

        TagData *tags = config->tagEngine()->readTags( fileList.at(i) );

        if( tags && tags->album.simplified() != "" )
        {
            newAlbumItem = 0;
            newItem = 0;
            samplingRate = tags->samplingRate;
            url = fileList.at(i);

            // search for an existing album
            for( int j=0; j<topLevelItemCount(); j++ )
            {
                if( topLevelItem(j)->type == ReplayGainFileListItem::Album &&
                    topLevelItem(j)->codecName == codecName &&
                    topLevelItem(j)->samplingRate == samplingRate &&
                    (
                      (
                          config->data.general.replayGainGrouping == Config::Data::General::AlbumDirectory &&
                          topLevelItem(j)->albumName == tags->album &&
                          topLevelItem(j)->url.toLocalFile() == url.directory()
                      ) || (
                          config->data.general.replayGainGrouping == Config::Data::General::Album &&
                          topLevelItem(j)->albumName == tags->album
                      ) || (
                          config->data.general.replayGainGrouping == Config::Data::General::Directory &&
                          topLevelItem(j)->url.toLocalFile() == url.directory()
                      )
                    )
                  )
                {
                    newItem = new ReplayGainFileListItem( topLevelItem(j) );
                    newItem->type = ReplayGainFileListItem::Track;
                    newItem->codecName = codecName;
                    newItem->url = url;
                    newItem->tags = tags;
                    newItem->time = tags->length;
                    break;
                }
            }

            // no existing album found
            if( !newItem )
            {
                // create album element
                newAlbumItem = new ReplayGainFileListItem( this, lastAlbumItem );
                newAlbumItem->type = ReplayGainFileListItem::Album;
                newAlbumItem->codecName = codecName;
                newAlbumItem->samplingRate = samplingRate;
                newAlbumItem->url = url.directory();
                newAlbumItem->albumName = tags->album;
                newAlbumItem->setExpanded( true );
                newAlbumItem->setFlags( newAlbumItem->flags() ^ Qt::ItemIsDragEnabled );
                lastAlbumItem = newAlbumItem;
                updateItem( newAlbumItem );
                // create track element
                newItem = new ReplayGainFileListItem( newAlbumItem );
                newItem->type = ReplayGainFileListItem::Track;
                newItem->codecName = codecName;
                newItem->samplingRate = samplingRate;
                newItem->url = url;
                newItem->tags = tags;
                newItem->time = tags->length;
            }
        }
        else if( tags )
        {
            newItem = new ReplayGainFileListItem( this );
            newItem->type = ReplayGainFileListItem::Track;
            newItem->codecName = codecName;
            newItem->samplingRate = tags->samplingRate;
            newItem->url = fileList.at(i);
            newItem->tags = tags;
            newItem->time = tags->length;
        }
        else
        {
            newItem = new ReplayGainFileListItem( this );
            newItem->type = ReplayGainFileListItem::Track;
            newItem->codecName = codecName;
            newItem->url = fileList.at(i);
            newItem->time = 200;
        }

        totalTime += newItem->time;

        updateItem( newItem, true );

//         emit timeChanged( newItem->time );
    }

//     emit fileCountChanged( topLevelItemCount() );

    if( unsupportedList.size() > 0 )
        KMessageBox::errorList( this, i18n("The following files could not be added:"), unsupportedList );
}

void ReplayGainFileList::addDir( const KUrl& directory, bool recursive, const QStringList& codecList )
{
    pScanStatus->setValue( 0 );
    pScanStatus->setMaximum( 0 );
    pScanStatus->show(); // show the status while scanning the directories
    tScanStatus.start();

    listDir( directory.path(), codecList, recursive, true );
    listDir( directory.path(), codecList, recursive );

    pScanStatus->hide(); // hide the status bar, when the scan is done
}

void ReplayGainFileList::removeSelectedItems()
{
    ReplayGainFileListItem *item, *child;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track && item->isSelected() && item->state != ReplayGainFileListItem::Processing )
        {
            totalTime -= item->time;
            delete item;
            i--;
        }
        else if( item->type == ReplayGainFileListItem::Album )
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                if( child->type == ReplayGainFileListItem::Track && ( child->isSelected() || item->isSelected() ) && child->state != ReplayGainFileListItem::Processing )
                {
                    totalTime -= child->time;
                    delete child;
                    j--;
                }
            }
            if( item->childCount() == 0 )
            {
                delete item;
                i--;
            }
        }
    }
}

void ReplayGainFileList::updateItem( ReplayGainFileListItem *item, bool initialUpdate )
{
    if( !item )
        return;

    if( item->type == ReplayGainFileListItem::Album )
    {
        const QString identificator = config->data.general.replayGainGrouping == Config::Data::General::Directory ? item->url.pathOrUrl().right(item->url.pathOrUrl().length()-item->url.pathOrUrl().lastIndexOf("/")-1) : item->albumName;
        item->setText( Column_File, identificator + " (" + item->codecName + ", " + QString::number(item->samplingRate) + " Hz)" );
    }
    else
    {
        item->setText( Column_File, item->url.pathOrUrl() );
        if( item->tags && item->tags->track_gain != 210588 )
        {
            item->setText( Column_Track, QString().sprintf("%+.2f dB",item->tags->track_gain) );
        }
        else
        {
            item->setText( Column_Track, "?" );
        }
        if( item->tags && item->tags->album_gain != 210588 )
        {
            item->setText( Column_Album, QString().sprintf("%+.2f dB",item->tags->album_gain) );
        }
        else
        {
            item->setText( Column_Album, "?" );
        }
    }

    if( !initialUpdate )
    {
        update( indexFromItem( item, 0 ) );
        update( indexFromItem( item, 1 ) );
        update( indexFromItem( item, 2 ) );
    }
}

void ReplayGainFileList::processItems( const QList<ReplayGainFileListItem*>& itemList )
{
    ReplayGainFileListItem *parent = 0;

    if( itemList.count() <= 0 )
        return;

    QList<ReplayGainPipe> pipes = config->pluginLoader()->getReplayGainPipes( itemList.at(0)->codecName );

    if( itemList.at(0)->take >= pipes.count() )
    {
        for( int i=0; i<itemList.count(); i++ )
        {
            itemList.at(i)->state = ReplayGainFileListItem::Failed;
            updateItem( itemList.at(i) );
        }
        processNextFile();
        return;
    }

    currentPlugin = pipes.at(itemList.at(0)->take).plugin;

    if( !currentPlugin )
        return;

    KUrl::List urls;
    for( int i=0; i<itemList.count(); i++ )
    {
        urls += itemList.at(i)->url;
    }

    currentId = currentPlugin->apply( urls, mode );
    if( currentId < 0 )
    {
        for( int i=0; i<itemList.count(); i++ )
        {
            itemList.at(i)->state = ReplayGainFileListItem::Failed;
            updateItem( itemList.at(i) );
            parent = (ReplayGainFileListItem*)itemList.at(i)->parent();
            if( parent )
            {
                parent->state = ReplayGainFileListItem::Failed;
                updateItem( parent );
            }
        }
        processNextFile();
        return;
    }

    currentTime = 0;
    for( int i=0; i<itemList.count(); i++ )
    {
        itemList.at(i)->processId = currentId;
        itemList.at(i)->take++;
        itemList.at(i)->state = ReplayGainFileListItem::Processing;
        updateItem( itemList.at(i) );
        currentTime += itemList.at(i)->time;
        parent = (ReplayGainFileListItem*)itemList.at(i)->parent();
        if( parent )
        {
            parent->state = ReplayGainFileListItem::Processing;
            updateItem( parent );
        }
    }
}

void ReplayGainFileList::calcAllReplayGain( bool force )
{
    ReplayGainFileListItem *item, *child;

    emit processStarted();

    queue = true;
    killed = false;
    totalTime = 0;
    mode = force ? ReplayGainPlugin::Force : ReplayGainPlugin::Add;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track )
        {
            item->state = ReplayGainFileListItem::Waiting;
            item->take = 0;
            updateItem( item );
            totalTime += item->time;
        }
        else
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                child->state = ReplayGainFileListItem::Waiting;
                child->take = 0;
                updateItem( child );
                totalTime += child->time;
            }
            item->state = ReplayGainFileListItem::Waiting;
            updateItem( item );
        }
    }
    processedTime = 0;
    updateTimer.start( config->data.general.updateDelay );
    processNextFile();
}

void ReplayGainFileList::removeAllReplayGain()
{
    ReplayGainFileListItem *item, *child;

    emit processStarted();

    queue = true;
    killed = false;
    totalTime = 0;
    mode = ReplayGainPlugin::Remove;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track )
        {
            item->state = ReplayGainFileListItem::Waiting;
            item->take = 0;
            updateItem( item );
            totalTime += item->time;
        }
        else
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                child->state = ReplayGainFileListItem::Waiting;
                child->take = 0;
                updateItem( child );
                totalTime += child->time;
            }
            item->state = ReplayGainFileListItem::Waiting;
            updateItem( item );
        }
    }
    processedTime = 0;
    updateTimer.start( config->data.general.updateDelay );
    processNextFile();
}

void ReplayGainFileList::cancelProcess()
{
    ReplayGainFileListItem *item, *child;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track )
        {
            if( item->state == ReplayGainFileListItem::Processing )
            {
                currentPlugin->kill( item->processId );
            }
        }
        else
        {
            child = (ReplayGainFileListItem*)item->child(0);
            if( child->state == ReplayGainFileListItem::Processing )
            {
                currentPlugin->kill( child->processId );
            }
        }
    }
    killed = true;
    queue = false;
}

void ReplayGainFileList::processNextFile()
{
    int count = processingCount();

    if( !queue )
    {
        if( count <= 0 )
        {
            updateTimer.stop();
            emit processStopped();
        }
        return;
    }

    QList<ReplayGainFileListItem*> itemList;
    bool calcGain;

    for( int i=0; i<topLevelItemCount() && count<config->data.general.numFiles; i++ )
    {
        ReplayGainFileListItem *item = topLevelItem(i);

        calcGain = false;
        itemList.clear();

        if( item->state != ReplayGainFileListItem::Waiting )
            continue;

        if( item->type == ReplayGainFileListItem::Track )
        {
            itemList += item;
            if( mode == ReplayGainPlugin::Force || mode == ReplayGainPlugin::Remove || !item->tags || item->tags->track_gain == 210588 || item->tags->album_gain == 210588 )
                calcGain = true;
        }
        else
        {
            float albumGain = 210588;
            for( int j=0; j<item->childCount(); j++ )
            {
                ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
                if( child->state != ReplayGainFileListItem::Waiting )
                    continue;

                if( j == 0 && child->tags )
                    albumGain = child->tags->album_gain;

                itemList += child;
                if( mode == ReplayGainPlugin::Force || mode == ReplayGainPlugin::Remove || !child->tags || child->tags->album_gain != albumGain || child->tags->album_gain == 210588 )
                    calcGain = true;
//                 if( child->state == ReplayGainFileListItem::Processing ) { itemList.clear(); break; } // NOTE this would be possible if per file calaculation would be possible
            }
        }

//         // why?
//         if( !calcGain )
//         {
//             for( int j=0; j<item->childCount(); j++ )
//             {
//                 ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
//                 child->state = ReplayGainFileListItem::Processed;
//                 updateItem( child );
//                 processedTime += child->time;
//             }
//             item->state = ReplayGainFileListItem::Processed;
//             updateItem( item );
//             if( item->type == ReplayGainFileListItem::Track )
//                 processedTime += item->time;
//         }
//         else
//         {
//             count++;
//             processItems( itemList );
//             return;
//         }

        if( calcGain )
        {
            count++;
            processItems( itemList );
            return;
        }
    }

    if( count <= 0 )
    {
        queue = false;
        updateTimer.stop();
        emit processStopped();
    }
}

int ReplayGainFileList::processingCount()
{
    ReplayGainFileListItem *item, *child;
    int count = 0;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track && item->state == ReplayGainFileListItem::Processing )
        {
            count++;
        }
        else if( item->type == ReplayGainFileListItem::Album )
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                if( child->state == ReplayGainFileListItem::Processing )
                {
                    count++;
                    break;
                }
            }
        }
    }

    return count;
}

void ReplayGainFileList::pluginProcessFinished( int id, int exitCode )
{
    ReplayGainFileListItem *item, *child;
//     int processedCount;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track && item->processId == id )
        {
            item->processId = -1;
            if( killed )
            {
                item->state = ReplayGainFileListItem::Waiting;
            }
            else if( exitCode == 0 )
            {
                item->state = ReplayGainFileListItem::Processed;
            }
            else
            {
                item->state = ReplayGainFileListItem::Waiting;
            }
            item->tags = config->tagEngine()->readTags( item->url );
            updateItem( item );
            processedTime += item->time;
            break;
        }
        else if( item->type == ReplayGainFileListItem::Album )
        {
//             processedCount = 0;
            bool updateParent = false;
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                if( child->processId == id )
                {
                    child->processId = -1;
                    if( killed )
                    {
                        child->state = ReplayGainFileListItem::Waiting;
                    }
                    else if( exitCode == 0 )
                    {
                        child->state = ReplayGainFileListItem::Processed;
//                         item->state = ReplayGainFileListItem::Processed;
                    }
                    else
                    {
                        child->state = ReplayGainFileListItem::Waiting;
//                         item->state = ReplayGainFileListItem::Waiting;
                    }
                    child->tags = config->tagEngine()->readTags( child->url );
                    updateItem( child );
                    processedTime += child->time;
//                     processedCount++;
                    updateParent = true;
                }
            }
//             if( processedCount == item->childCount() )
            if( updateParent )
            {
                if( killed )
                {
                    item->state = ReplayGainFileListItem::Waiting;
                }
                else if( exitCode == 0 )
                {
                    item->state = ReplayGainFileListItem::Processed;
                }
                else
                {
                    item->state = ReplayGainFileListItem::Waiting;
                }
                updateItem( item );
                break;
            }
        }
    }
    processNextFile();
}

void ReplayGainFileList::pluginLog( int id, const QString& message )
{
    Q_UNUSED(id)

    logger->log( 1000, "\t" + message.trimmed().replace("\n","\n\t") );
}

void ReplayGainFileList::showContextMenu( const QPoint& point )
{
    ReplayGainFileListItem *item = (ReplayGainFileListItem*)itemAt( point );

    // if item is null, we can abort here
    //if( !item ) return;

    // add a tilte to our context manu
    //contextMenu->insertTitle( static_cast<FileListItem*>(item)->fileName ); // TODO sqeeze or something else

    // TODO implement pasting, etc.

    contextMenu->clear();

    // is this file (of our item) beeing converted at the moment?
    if( item && item->state != ReplayGainFileListItem::Processing )
    {
        contextMenu->addAction( collapseAction );
        contextMenu->addAction( expandAction );
        contextMenu->addSeparator();
        contextMenu->addAction( removeAction );
        //contextMenu->addAction( paste );
//         contextMenu->addAction( newAction );
        //contextMenu->addSeparator();
        //contextMenu->addAction( startAction );
    }
    else
    {
        contextMenu->addAction( collapseAction );
        contextMenu->addAction( expandAction );
//         contextMenu->addSeparator();
        //contextMenu->addAction( paste );
//         contextMenu->addAction( newAction );
        //contextMenu->addSeparator();
        //contextMenu->addAction( stopAction );
    }

    // show the popup menu
    contextMenu->popup( viewport()->mapToGlobal(point) );
}

void ReplayGainFileList::updateProgress()
{
    float progress = currentPlugin->progress( currentId );
    progress /= 100;
    emit updateProgress( processedTime + progress*currentTime, totalTime );
}
