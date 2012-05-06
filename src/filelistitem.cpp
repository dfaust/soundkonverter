//
// C++ Implementation: filelistitem
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "filelistitem.h"

#include <QPainter>


FileListItem::FileListItem( QTreeWidget *parent, QTreeWidgetItem *after )
    : QTreeWidgetItem( parent, after )
{
    state = WaitingForConversion;
    length = 0;
    tags = 0;
}

FileListItem::FileListItem( QTreeWidget *parent )
    : QTreeWidgetItem( parent )
{
    state = WaitingForConversion;
    length = 0;
    tags = 0;
}

FileListItem::~FileListItem()
{
    if( tags )
        delete tags;
}

FileListItemDelegate::FileListItemDelegate( QObject *parent )
    : QItemDelegate( parent )
{}

// TODO margin
void FileListItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    FileListItem *item =  static_cast<FileListItem*>( index.internalPointer() );

    QColor backgroundColor;

    painter->save();

    QStyleOptionViewItem _option = option;

    bool isConverting = false;
    bool isFailed = false;
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
                isConverting = true;
                break;
            case FileListItem::Stopped:
                break;
            case FileListItem::BackendNeedsConfiguration:
                isFailed = true;
                break;
            case FileListItem::DiscFull:
                isFailed = true;
                break;
            case FileListItem::Failed:
                isFailed = true;
                break;
        }
    }

    if( isConverting )
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

    if( index.column() == 1 || index.column() == 2 )
    {
        QRect textRect = painter->boundingRect( QRect(), Qt::AlignLeft|Qt::TextSingleLine, item->text(index.column()) );

        if ( textRect.width() < m_rect.width() )
        {
            painter->drawText( m_rect, Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );
        }
        else
        {
            //textRect = painter->boundingRect( QRect(), Qt::AlignLeft, "... " );
            painter->drawText( m_rect, Qt::AlignRight|Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );
            QLinearGradient linearGrad( QPoint(m_rect.x(),0), QPoint(m_rect.x()+15,0) );
            linearGrad.setColorAt( 0, backgroundColor );
            backgroundColor.setAlpha( 0 );
            linearGrad.setColorAt( 1, backgroundColor );
            painter->fillRect( m_rect.x(), m_rect.y(), 15, m_rect.height(), linearGrad );
        }
    }
    else
    {
        painter->drawText( m_rect, Qt::TextSingleLine|Qt::TextExpandTabs, item->text(index.column()) );
    }

    //QItemDelegate::paint( painter, _option, index );

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
