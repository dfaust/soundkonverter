/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


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

QStringList ReplayGainFileListItem::directories()
{
    QStringList directories;

    if( type == ReplayGainFileListItem::Track )
    {
        directories.append( url.directory() );
    }
    else
    {
        for( int j=0; j<childCount(); j++ )
        {
            directories.append( static_cast<ReplayGainFileListItem*>(child(j))->url.directory() );
        }
    }

    return directories;
}

KUrl::List ReplayGainFileListItem::urls()
{
    KUrl::List urls;

    if( type == ReplayGainFileListItem::Track )
    {
        urls.append( url );
    }
    else
    {
        for( int j=0; j<childCount(); j++ )
        {
            urls.append( static_cast<ReplayGainFileListItem*>(child(j))->url );
        }
    }

    return urls;
}

void ReplayGainFileListItem::setState( ReplayGainFileListItem::State newState )
{
    if( type == ReplayGainFileListItem::Track )
    {
        state = newState;
    }
    else
    {
        for( int j=0; j<childCount(); j++ )
        {
            static_cast<ReplayGainFileListItem*>(child(j))->state = newState;
        }
    }
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
            case ReplayGainFileListItem::WaitingForReplayGain:
            {
                break;
            }
            case ReplayGainFileListItem::Processing:
            {
                isProcessing = true;
                break;
            }
            case ReplayGainFileListItem::Stopped:
            {
                switch( item->returnCode )
                {
                    case ReplayGainFileListItem::Succeeded:
                    case ReplayGainFileListItem::SucceededWithProblems:
                    {
                        isSucceeded = true;
                        break;
                    }
                    case ReplayGainFileListItem::Skipped:
                    case ReplayGainFileListItem::StoppedByUser:
                    {
                        break;
                    }
                    case ReplayGainFileListItem::BackendNeedsConfiguration:
                    case ReplayGainFileListItem::Failed:
                    {
                        isFailed = true;
                        break;
                    }
                }
                break;
            }
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
