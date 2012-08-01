
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
    : QTreeWidget( parent ),
    config( _config ),
    logger( _logger )
{
    queue = false;

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
//     processAddAction = new KAction( KIcon("list-add"), i18n("Calculate ReplayGain"), this );
//     connect( processAddAction, SIGNAL(triggered()), this, SLOT(processAddSelectedItems()) );
//     processRemoveAction = new KAction( KIcon("list-remove"), i18n("Remove ReplayGain"), this );
//     connect( processRemoveAction, SIGNAL(triggered()), this, SLOT(processRemoveSelectedItems()) );
//     killAction = new KAction( KIcon("process-stop"), i18n("Stop calculation"), this );
//     connect( killAction, SIGNAL(triggered()), this, SLOT(killSelectedItems()) );
    moveAction = new KAction( KIcon ("folder-new"), i18n("Move to new Album"), this );
    connect( moveAction, SIGNAL(triggered()), this, SLOT(moveSelectedItems()) );
    removeAction = new KAction( KIcon("edit-delete"), i18n("Remove"), this );
    removeAction->setShortcut( QKeySequence::Delete );
    connect( removeAction, SIGNAL(triggered()), this, SLOT(removeSelectedItems()) );
    addAction( removeAction );

    contextMenu = new QMenu( this );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)) );

    const QList<ReplayGainPlugin*> replayGainPlugins = config->pluginLoader()->getAllReplayGainPlugins();
    for( int i=0; i<replayGainPlugins.size(); i++ )
    {
        connect( replayGainPlugins.at(i), SIGNAL(jobFinished(int,int)), this, SLOT(pluginProcessFinished(int,int)) );
        connect( replayGainPlugins.at(i), SIGNAL(log(int,const QString&)), this, SLOT(pluginLog(int,const QString&)) );
    }
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
    if( itemAt(event->pos()) && static_cast<ReplayGainFileListItem*>(itemAt(event->pos()))->type != ReplayGainFileListItem::Track )
        QTreeWidget::dragMoveEvent(event);
}

void ReplayGainFileList::dropEvent( QDropEvent *event )
{
    const QList<QUrl> q_urls = event->mimeData()->urls();
    KUrl::List k_urls;
    KUrl::List k_urls_dirs;
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    if( event->source() == this )
    {
        ReplayGainFileListItem *destination = static_cast<ReplayGainFileListItem*>(itemAt(event->pos()));

        if( !destination )
        {
            QTreeWidget::dropEvent(event);
        }
        else
        {
            bool canDrop = true;
            if( destination->type == ReplayGainFileListItem::Album )
            {
                QList<QTreeWidgetItem*> q_items = selectedItems();
                foreach( QTreeWidgetItem *q_item, q_items )
                {
                    ReplayGainFileListItem *item = (ReplayGainFileListItem*)q_item;

                    if( item->codecName != destination->codecName || item->samplingRate != destination->samplingRate )
                    {
                        canDrop = false;
                        break;
                    }
                }
            }
            if( canDrop )
            {
                QTreeWidget::dropEvent(event);
            }
            else
            {
                KMessageBox::error( this, i18n("Some tracks can't be added to the album because either the codec or the sampling rate is different.") );
            }
        }

        // remove album, if last child was dragged out of it
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
        foreach( QUrl url, q_urls )
        {
            QString mimeType;
            QString codecName = config->pluginLoader()->getCodecFromFile( url, &mimeType );

            if( mimeType == "inode/directory" )
            {
                k_urls_dirs += url;
            }
            else if( config->pluginLoader()->canReplayGain(codecName,0,&errorList) )
            {
                k_urls += url;
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

                fileName = KUrl(url).pathOrUrl();

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
        foreach( KUrl url, k_urls_dirs )
        {
            addDir( url, true, config->pluginLoader()->formatList(PluginLoader::ReplayGain,PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
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

        const bool isDir = fileInfo.isDir(); // NOTE checking for isFile might not work with all file names
        if( isDir && recursive )
        {
            count = listDir( directory + "/" + fileName, filter, recursive, fast, count );
        }
        else if( !isDir )
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
                    if( tScanStatus.elapsed() > ConfigUpdateDelay * 10 )
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
    ReplayGainFileListItem *newAlbumItem, *newTrackItem;
    QString codecName;
    QStringList unsupportedList;

    foreach( KUrl url, fileList )
    {
        if( !_codecName.isEmpty() )
        {
            codecName = _codecName;
        }
        else
        {
            codecName = config->pluginLoader()->getCodecFromFile( url );

            if( !config->pluginLoader()->canReplayGain(codecName,0) )
            {
                unsupportedList.append( url.pathOrUrl() );
                continue;
            }
        }

        TagData *tags = config->tagEngine()->readTags( url );

        if( tags && !tags->album.simplified().isEmpty() )
        {
            const int samplingRate = tags->samplingRate;

            newAlbumItem = 0;
            newTrackItem = 0;

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
                    newTrackItem = new ReplayGainFileListItem( topLevelItem(j) );
                    newTrackItem->type = ReplayGainFileListItem::Track;
                    newTrackItem->codecName = codecName;
                    newTrackItem->samplingRate = samplingRate;
                    newTrackItem->url = url;
                    newTrackItem->tags = tags;
                    newTrackItem->time = tags->length;
                    break;
                }
            }

            // no existing album found
            if( !newTrackItem )
            {
                // create album element
                newAlbumItem = new ReplayGainFileListItem( this, lastAlbumItem );
                newAlbumItem->type = ReplayGainFileListItem::Album;
                newAlbumItem->codecName = codecName;
                newAlbumItem->samplingRate = samplingRate;
                newAlbumItem->url = url.directory();
                newAlbumItem->albumName = config->data.general.replayGainGrouping == Config::Data::General::Directory ? url.directory() : tags->album;
                newAlbumItem->setExpanded( true );
                newAlbumItem->setFlags( newAlbumItem->flags() ^ Qt::ItemIsDragEnabled );
                lastAlbumItem = newAlbumItem;
                updateItem( newAlbumItem );
                // create track element
                newTrackItem = new ReplayGainFileListItem( newAlbumItem );
                newTrackItem->type = ReplayGainFileListItem::Track;
                newTrackItem->codecName = codecName;
                newTrackItem->samplingRate = samplingRate;
                newTrackItem->url = url;
                newTrackItem->tags = tags;
                newTrackItem->time = tags->length;
            }
        }
        else if( tags )
        {
            newTrackItem = new ReplayGainFileListItem( this );
            newTrackItem->type = ReplayGainFileListItem::Track;
            newTrackItem->codecName = codecName;
            newTrackItem->samplingRate = tags->samplingRate;
            newTrackItem->url = url;
            newTrackItem->tags = tags;
            newTrackItem->time = tags->length;
        }
        else
        {
            newTrackItem = new ReplayGainFileListItem( this );
            newTrackItem->type = ReplayGainFileListItem::Track;
            newTrackItem->codecName = codecName;
            newTrackItem->url = url;
            newTrackItem->time = 200;
        }

        updateItem( newTrackItem );

        emit timeChanged( newTrackItem->time );
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

void ReplayGainFileList::updateItem( ReplayGainFileListItem *item )
{
    if( !item )
        return;

    if( item->type == ReplayGainFileListItem::Album )
    {
        item->setText( Column_File, item->albumName + " (" + item->codecName + ", " + QString::number(item->samplingRate) + " Hz)" );
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
    update( indexFromItem( item, 0 ) );
    update( indexFromItem( item, 1 ) );
    update( indexFromItem( item, 2 ) );
}

void ReplayGainFileList::startProcessing( ReplayGainPlugin::ApplyMode _mode )
{
    // iterate through all items and set the state to "Waiting"
    ReplayGainFileListItem *item, *child;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        if( item->type == ReplayGainFileListItem::Track )
        {
            bool isStopped = false;
            if( item )
            {
                switch( item->state )
                {
                    case ReplayGainFileListItem::Waiting:
                        break;
                    case ReplayGainFileListItem::Processing:
                        break;
                    case ReplayGainFileListItem::Stopped:
                        isStopped = true;
                        break;
                }
            }
            if( isStopped )
            {
                item->state = ReplayGainFileListItem::Waiting;
                updateItem( item );
            }
        }
        else
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                child = (ReplayGainFileListItem*)item->child(j);
                bool isStopped = false;
                if( child )
                {
                    switch( child->state )
                    {
                        case ReplayGainFileListItem::Waiting:
                            break;
                        case ReplayGainFileListItem::Processing:
                            break;
                        case ReplayGainFileListItem::Stopped:
                            isStopped = true;
                            break;
                    }
                }
                if( isStopped )
                {
                    child->state = ReplayGainFileListItem::Waiting;
                    updateItem( child );
                }
            }
            bool isStopped = false;
            if( item )
            {
                switch( item->state )
                {
                    case ReplayGainFileListItem::Waiting:
                        break;
                    case ReplayGainFileListItem::Processing:
                        break;
                    case ReplayGainFileListItem::Stopped:
                        isStopped = true;
                        break;
                }
            }
            if( isStopped )
            {
                item->state = ReplayGainFileListItem::Waiting;
                updateItem( item );
            }
        }

    }

    mode = _mode;
    queue = true;

//     emit queueModeChanged( queue );
    emit processStarted();

    processNextItem();
}

void ReplayGainFileList::cancelProcess()
{
    queue = false;
//     emit queueModeChanged( queue );

    ReplayGainFileListItem *item, *child;
    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        if( item->type == ReplayGainFileListItem::Track )
        {
            bool canKill = false;
            if( item )
            {
                switch( item->state )
                {
                    case ReplayGainFileListItem::Waiting:
                        break;
                    case ReplayGainFileListItem::Processing:
                        canKill = true;
                        break;
                    case ReplayGainFileListItem::Stopped:
                        break;
                }
            }
            if( canKill )
                emit killItem( item );
        }
        else
        {
            bool canKill = false;
            if( item )
            {
                switch( item->state )
                {
                    case ReplayGainFileListItem::Waiting:
                        break;
                    case ReplayGainFileListItem::Processing:
                        canKill = true;
                        break;
                    case ReplayGainFileListItem::Stopped:
                        break;
                }
            }
            if( canKill )
            {
                emit killItem( item );
            }
            else
            {
                for( int j=0; j<item->childCount(); j++ )
                {
                    child = (ReplayGainFileListItem*)item->child(j);
                    bool canKill = false;
                    if( child )
                    {
                        switch( child->state )
                        {
                            case ReplayGainFileListItem::Waiting:
                                break;
                            case ReplayGainFileListItem::Processing:
                                canKill = true;
                                break;
                            case ReplayGainFileListItem::Stopped:
                                break;
                        }
                    }
                    if( canKill )
                        emit killItem( child );
                }
            }
        }

        bool canKill = false;
        if( item )
        {
            switch( item->state )
            {
                case ReplayGainFileListItem::Waiting:
                    break;
                case ReplayGainFileListItem::Processing:
                    canKill = true;
                    break;
                case ReplayGainFileListItem::Stopped:
                    break;
            }
        }
        if( canKill )
            emit killItem( item );
    }
}

void ReplayGainFileList::processNextItem()
{
    if( !queue )
        return;

    int count = processingCount();

    // look for waiting files
    for( int i=0; i<topLevelItemCount() && count < config->data.general.numReplayGainFiles; i++ )
    {
        ReplayGainFileListItem *item = topLevelItem(i);

        bool calcGain = false;

        if( item->state != ReplayGainFileListItem::Waiting )
            continue;

        if( item->type == ReplayGainFileListItem::Track )
        {
            if( mode == ReplayGainPlugin::Force || mode == ReplayGainPlugin::Remove || !item->tags || item->tags->track_gain == 210588 || item->tags->album_gain == 210588 )
                calcGain = true;
        }
        else
        {
            bool albumGainDifferent = false;
            bool childProcessing = false;

            float albumGain = 210588;
            for( int j=0; j<item->childCount(); j++ )
            {
                ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
                if( child->state != ReplayGainFileListItem::Waiting )
                {
                    childProcessing = true;
                    break;
                }

                if( j == 0 && child->tags )
                    albumGain = child->tags->album_gain;

                if( !child->tags || child->tags->album_gain != albumGain || child->tags->album_gain == 210588 )
                {
                    albumGainDifferent = true;
                    break;
                }
            }

            if( mode == ReplayGainPlugin::Force || mode == ReplayGainPlugin::Remove || ( !childProcessing && albumGainDifferent ) )
                calcGain = true;
        }

        if( calcGain )
        {
            if( item->type == ReplayGainFileListItem::Album )
            {
                for( int j=0; j<item->childCount(); j++ )
                {
                    ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
                    child->state = ReplayGainFileListItem::Processing;
                    updateItem( child );
                }
            }
            item->state = ReplayGainFileListItem::Processing;
            updateItem( item );

            count++;
            emit processItem( item, mode );
        }
        else
        {
            if( item->type == ReplayGainFileListItem::Album )
            {
                for( int j=0; j<item->childCount(); j++ )
                {
                    ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
                    if( child->state == ReplayGainFileListItem::Waiting )
                    {
                        child->state = ReplayGainFileListItem::Stopped;
                        child->returnCode = ReplayGainFileListItem::Skipped;
                    }
                }
            }
            item->state = ReplayGainFileListItem::Stopped;
            item->returnCode = ReplayGainFileListItem::Skipped;
        }
    }

    if( count == 0 )
        itemFinished( 0, ReplayGainFileListItem::Succeeded );
}

int ReplayGainFileList::waitingCount()
{
    int count = 0;
    ReplayGainFileListItem *item;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem( i );
        if( item->state == ReplayGainFileListItem::Waiting )
            count++;
    }

    return count;
}

int ReplayGainFileList::processingCount() // TODO use ReplayGainProcessor
{
    ReplayGainFileListItem *item, *child;
    int count = 0;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track )
        {
            if( item->state == ReplayGainFileListItem::Processing )
                count++;
        }
        else if( item->type == ReplayGainFileListItem::Album )
        {
            if( item->state == ReplayGainFileListItem::Processing )
            {
                count++;
            }
            else
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
    }

    return count;
}

void ReplayGainFileList::itemFinished( ReplayGainFileListItem *item, ReplayGainFileListItem::ReturnCode returnCode )
{
    if( item )
    {
        item->state = ReplayGainFileListItem::Stopped;
        item->returnCode = returnCode;
        if( item->type == ReplayGainFileListItem::Track )
            item->tags = config->tagEngine()->readTags( item->url );
        updateItem( item );
        if( item->type == ReplayGainFileListItem::Album )
        {
            for( int j=0; j<item->childCount(); j++ )
            {
                ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
                child->state = ReplayGainFileListItem::Stopped;
                child->returnCode = returnCode;
                child->tags = config->tagEngine()->readTags( child->url );
                updateItem( child );
            }
        }
    }

    if( waitingCount() > 0 && queue )
    {
        processNextItem();
    }
    else if( processingCount() == 0 )
    {
        queue = false;
//         emit queueModeChanged( queue );
//         float time = 0;
//         for( int i=0; i<topLevelItemCount(); i++ )
//         {
//             FileListItem *temp_item = topLevelItem( i );
//             updateItem( temp_item ); // TODO why?
//             time += temp_item->length;
//         }
//         emit finished( time );
        emit finished( false );
        emit processStopped();
//         emit fileCountChanged( topLevelItemCount() );
    }
}

void ReplayGainFileList::showContextMenu( const QPoint& point )
{
    QList<QTreeWidgetItem*> q_items = selectedItems();
    bool canRemove = q_items.count() > 0;
    bool canStart = q_items.count() > 0;
    bool canMove = q_items.count() > 0;
    bool canKill = q_items.count() > 0;

    foreach( QTreeWidgetItem *q_item, q_items )
    {
        ReplayGainFileListItem *item = (ReplayGainFileListItem*)q_item;

        if( item->type == ReplayGainFileListItem::Track )
        {
            switch( item->state )
            {
                case ReplayGainFileListItem::Waiting:
                    canKill = false;
                    break;
                case ReplayGainFileListItem::Processing:
                    canRemove = false;
                    canStart = false;
                    canMove = false;
                    break;
                case ReplayGainFileListItem::Stopped:
                    canKill = false;
                    break;
            }
        }
        else
        {
            canMove = false;
            if( item->state == ReplayGainFileListItem::Processing )
            {
                canRemove = false;
                canStart = false;
            }
            else
            {
                for( int j=0; j<item->childCount(); j++ )
                {
                    ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);

                    switch( child->state )
                    {
                        case ReplayGainFileListItem::Waiting:
                            canKill = false;
                            break;
                        case ReplayGainFileListItem::Processing:
                            canRemove = false;
                            canStart = false;
                            break;
                        case ReplayGainFileListItem::Stopped:
                            canKill = false;
                            break;
                    }
                }
            }
        }
    }

    contextMenu->clear();

    contextMenu->addAction( collapseAction );
    contextMenu->addAction( expandAction );
//     if( canStart || canKill )
//     {
//         contextMenu->addSeparator();
//     }
//     if( canStart )
//     {
//         contextMenu->addAction( processAddAction );
//         contextMenu->addAction( processRemoveAction );
//     }
//     if( canKill )
//     {
//         contextMenu->addAction( killAction );
//     }
    if( canMove )
    {
        contextMenu->addSeparator();
        contextMenu->addAction( moveAction );
    }
    if( canRemove )
    {
        contextMenu->addSeparator();
        contextMenu->addAction( removeAction );
    }

    contextMenu->popup( viewport()->mapToGlobal(point) );
}

// void ReplayGainFileList::processAddSelectedItems()
// {
//     foreach( QTreeWidgetItem *q_item, selectedItems() )
//     {
//         ReplayGainFileListItem *item = (ReplayGainFileListItem*)q_item;
//
//         if( item->type == ReplayGainFileListItem::Track )
//         {
//             if( item->state != ReplayGainFileListItem::Processing )
//             {
//             }
//         }
//         else
//         {
//             if( item->state != ReplayGainFileListItem::Processing )
//             {
//             }
//             else
//             {
//                 for( int j=0; j<item->childCount(); j++ )
//                 {
//                     ReplayGainFileListItem *child = (ReplayGainFileListItem*)item->child(j);
//
//                 }
//             }
//         }
//         emit processItem( item, ReplayGainPlugin::Force );
//     }
// }

// void ReplayGainFileList::processRemoveSelectedItems()
// {
//     foreach( QTreeWidgetItem *q_item, selectedItems() )
//     {
//         ReplayGainFileListItem *item = (ReplayGainFileListItem*)q_item;
//
//         emit processItem( item, ReplayGainPlugin::Remove );
//     }
// }

// void ReplayGainFileList::killSelectedItems()
// {
//
// }

void ReplayGainFileList::moveSelectedItems()
{
    QList<QTreeWidgetItem*> q_items = selectedItems();

    ReplayGainFileListItem *newAlbumItem = 0;

    if( q_items.count() > 0 )
    {
        newAlbumItem = new ReplayGainFileListItem( this );
        newAlbumItem->type = ReplayGainFileListItem::Album;
        newAlbumItem->codecName = QString();
        newAlbumItem->samplingRate = 0;
        newAlbumItem->url = KUrl();
        newAlbumItem->albumName = i18n("New album");
        newAlbumItem->setExpanded( true );
        newAlbumItem->setFlags( newAlbumItem->flags() ^ Qt::ItemIsDragEnabled );
    }

    foreach( QTreeWidgetItem *q_item, q_items )
    {
        ReplayGainFileListItem *item = (ReplayGainFileListItem*)q_item;

        if( newAlbumItem->codecName.isEmpty() )
        {
            newAlbumItem->codecName = item->codecName;
            newAlbumItem->samplingRate = item->samplingRate;
            newAlbumItem->url = KUrl(item->url.directory());
        }

        if( item->codecName == newAlbumItem->codecName && item->samplingRate == newAlbumItem->samplingRate )
        {
            if( item->parent() )
                item->parent()->takeChild( item->parent()->indexOfChild(q_item) );
            else
                takeTopLevelItem( indexOfTopLevelItem(q_item) );

            newAlbumItem->addChild( q_item );
        }
    }

    if( newAlbumItem )
        updateItem( newAlbumItem );
}

void ReplayGainFileList::removeSelectedItems()
{
    ReplayGainFileListItem *item, *child;

    for( int i=0; i<topLevelItemCount(); i++ )
    {
        item = topLevelItem(i);
        if( item->type == ReplayGainFileListItem::Track && item->isSelected() && item->state != ReplayGainFileListItem::Processing )
        {
            emit timeChanged( -item->time );
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
                    emit timeChanged( -child->time );
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


