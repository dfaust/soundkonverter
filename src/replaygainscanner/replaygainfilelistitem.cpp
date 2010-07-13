
#include "replaygainfilelistitem.h"
#include <QResizeEvent> // NOTE needed by drag'n'drop events - but why?


ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidget *parent )
    : QTreeWidgetItem( parent )
{
    state = Waiting;
    tags = 0;
    samplingRate = 0;
    take = 0;
    processId = -1;
}
    ;
ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidget *parent, QTreeWidgetItem *preceding )
    : QTreeWidgetItem( parent, preceding )
{
    state = Waiting;
    tags = 0;
    samplingRate = 0;
    take = 0;
    processId = -1;
}

ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidgetItem *parent )
    : QTreeWidgetItem( parent )
{
    state = Waiting;
    tags = 0;
    samplingRate = 0;
    take = 0;
    processId = -1;
}

ReplayGainFileListItem::~ReplayGainFileListItem()
{
//     if( tags ) delete tags;
}

// void ReplayGainFileListItem::dragEnterEvent( QDragEnterEvent *event )
// {
//     if( event->mimeData()->hasFormat("text/uri-list") ) event->acceptProposedAction();
// }
// 
// void ReplayGainFileListItem::dragMoveEvent( QDragMoveEvent *event )
// {
//     if( 1 ) event->acceptProposedAction();
// }
// 
// void ReplayGainFileListItem::dropEvent( QDropEvent *event )
// {
//     event->acceptProposedAction();
// }
