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
        WaitingForReplayGain,
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

    QStringList directories();
    KUrl::List urls();
    void setState( State newState );

};

class ReplayGainFileListItemDelegate : public QItemDelegate
{
public:
    ReplayGainFileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // REPLAYGAINFILELISTITEM_H
