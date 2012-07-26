
#ifndef REPLAYGAINFILELISTITEM_H
#define REPLAYGAINFILELISTITEM_H

#include "core/replaygainplugin.h"
#include "metadata/tagengine.h"

#include <QTreeWidget>
#include <QItemDelegate>
#include <QPointer>
#include <KUrl>


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
        Stopped
    } state;

    enum ReturnCode {
        Succeeded,
        SucceededWithProblems,
        StoppedByUser,
        Skipped,
        BackendNeedsConfiguration,
        Failed
    } returnCode;

    KUrl url;
    QString albumName;
    QString codecName;
    int samplingRate;
    QPointer<TagData> tags;

    int time;
};

class ReplayGainFileListItemDelegate : public QItemDelegate
{
public:
    ReplayGainFileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // REPLAYGAINFILELISTITEM_H
