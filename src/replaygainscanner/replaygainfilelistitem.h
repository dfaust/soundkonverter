
#ifndef REPLAYGAINFILELISTITEM_H
#define REPLAYGAINFILELISTITEM_H

#include <QTreeWidget>
#include <QItemDelegate>
#include <QPointer>
#include <KUrl>

#include "metadata/tagengine.h"


class ReplayGainFileListItem : public QTreeWidgetItem
{
public:
    ReplayGainFileListItem( QTreeWidget *parent );
    ReplayGainFileListItem( QTreeWidget *parent, QTreeWidgetItem *preceding );

    ReplayGainFileListItem( QTreeWidgetItem *parent );

    ~ReplayGainFileListItem();

    enum Type {
        Track,
        Album
    } type;

    enum State {
        Waiting,
        Processing,
        Processed,
        Failed
    } state;

    KUrl url;
    QString albumName;
    QString codecName;
    int samplingRate;
    QPointer<TagData> tags;

    int time;

    int processId;
    int take;

// private:
//     void dragEnterEvent( QDragEnterEvent *event );
//     void dragMoveEvent( QDragMoveEvent *event );
//     void dropEvent( QDropEvent *event );

};

class ReplayGainFileListItemDelegate : public QItemDelegate
{
public:
    ReplayGainFileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // REPLAYGAINFILELISTITEM_H
