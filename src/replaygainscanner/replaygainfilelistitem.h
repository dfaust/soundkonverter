
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

    // code values will be displayed in the log files
    enum ReturnCode {
        Succeeded                       = 0,
        SucceededWithProblems           = 1,
        StoppedByUser                   = 2,
        Skipped                         = 3,
        BackendNeedsConfiguration       = 4,
        Failed                          = 5
    } returnCode;

    KUrl url;
    QString albumName;
    QString codecName;
    int samplingRate;
    TagData *tags;

    int length;
};

class ReplayGainFileListItemDelegate : public QItemDelegate
{
public:
    ReplayGainFileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // REPLAYGAINFILELISTITEM_H
