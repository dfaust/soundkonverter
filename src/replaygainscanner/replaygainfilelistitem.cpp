
#include "replaygainfilelistitem.h"
// #include <QResizeEvent> // NOTE needed by drag'n'drop events - but why?
#include <QPainter>


ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidget *parent )
    : QTreeWidgetItem( parent ),
    tags( 0 )
{
    state = Waiting;
    samplingRate = 0;
}

ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidget *parent, QTreeWidgetItem *preceding )
    : QTreeWidgetItem( parent, preceding ),
    tags( 0 )
{
    state = Waiting;
    samplingRate = 0;
}

ReplayGainFileListItem::ReplayGainFileListItem( QTreeWidgetItem *parent )
    : QTreeWidgetItem( parent ),
    tags( 0 )
{
    state = Waiting;
    samplingRate = 0;
}

ReplayGainFileListItem::~ReplayGainFileListItem()
{
    if( tags )
        delete tags;
}


ReplayGainFileListItemDelegate::ReplayGainFileListItemDelegate( QObject *parent )
    : QItemDelegate( parent )
{}

// TODO margin
void ReplayGainFileListItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    ReplayGainFileListItem *item =  static_cast<ReplayGainFileListItem*>( index.internalPointer() );

    QColor backgroundColor;

    painter->save();

    QStyleOptionViewItem _option = option;

    bool isProcessing = false;
    bool isSucceeded = false;
    bool isFailed = false;
    if( item )
    {
        switch( item->state )
        {
            case ReplayGainFileListItem::Waiting:
                break;
            case ReplayGainFileListItem::Processing:
                isProcessing = true;
                break;
            case ReplayGainFileListItem::Stopped:
                switch( item->returnCode )
                {
                    case ReplayGainFileListItem::Succeeded:
                        isSucceeded = true;
                        break;
                    case ReplayGainFileListItem::SucceededWithProblems:
                        isSucceeded = true;
                        break;
                    case ReplayGainFileListItem::Skipped:
                        break;
                    case ReplayGainFileListItem::StoppedByUser:
                        break;
                    case ReplayGainFileListItem::BackendNeedsConfiguration:
                        isFailed = true;
                        break;
                    case ReplayGainFileListItem::Failed:
                        isFailed = true;
                        break;
                }
                break;
        }
    }

    if( isProcessing )
    {
        if( option.state & QStyle::State_Selected )
        {
            backgroundColor = QColor(215,102,102); // hsv:   0, 134, 215
        }
        else
        {
            backgroundColor = QColor(255,234,234); // hsv:   0,  21, 255
        }
    }
    else if( isSucceeded )
    {
        if( option.state & QStyle::State_Selected )
        {
            backgroundColor = QColor(125,205,139); // hsv: 131, 100, 205
        }
        else
        {
            backgroundColor = QColor(234,255,238); // hsv: 131,  21, 255
        }
    }
    else if( isFailed )
    {
        if( option.state & QStyle::State_Selected )
        {
            backgroundColor = QColor(235,154, 49); // hsv:  34, 202, 235
        }
        else
        {
            backgroundColor = QColor(255,204,156); // hsv:  29,  99, 255
        }
    }
    else
    {
        if( option.state & QStyle::State_Selected )
        {
            backgroundColor = option.palette.highlight().color();
        }
        else
        {
            backgroundColor = option.palette.base().color();
        }
    }

    painter->fillRect( option.rect, backgroundColor );

    int m_left, m_top, m_right, m_bottom;
    item->treeWidget()->getContentsMargins( &m_left, &m_top, &m_right, &m_bottom );
    //QRect m_rect = QRect( option.rect.x()+m_left, option.rect.y()+m_top, option.rect.width()-m_left-m_right, option.rect.height()-m_top-m_bottom );
    QRect m_rect = QRect( option.rect.x()+m_left, option.rect.y(), option.rect.width()-m_left-m_right, option.rect.height() );

    QRect textRect = painter->boundingRect( QRect(), Qt::AlignLeft|Qt::TextSingleLine, item->text(index.column()) );

    if ( textRect.width() < m_rect.width() )
    {
        painter->drawText( m_rect, Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );
    }
    else
    {
        painter->drawText( m_rect, Qt::AlignRight|Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );
        QLinearGradient linearGrad( QPoint(m_rect.x(),0), QPoint(m_rect.x()+15,0) );
        linearGrad.setColorAt( 0, backgroundColor );
        backgroundColor.setAlpha( 0 );
        linearGrad.setColorAt( 1, backgroundColor );
        painter->fillRect( m_rect.x(), m_rect.y(), 15, m_rect.height(), linearGrad );
    }
//     painter->drawText( m_rect, Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );

//     QItemDelegate::paint( painter, _option, index );

    painter->restore();

//     int progress = (index.row() != 0 ? 100 / index.row() : 0);
//
//     // draw your cool progress bar here
//     QStyleOptionProgressBar opt;
//     opt.rect = option.rect;
//     opt.minimum = 0;
//     opt.maximum = 100;
//     opt.progress = progress;
//     opt.text = QString("%1%").arg(progress);
//     opt.textVisible = true;
//     QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, painter, 0);

    // maybe even let default implementation draw list item contents?
    // QItemDelegate::paint(painter, option, index);
}
