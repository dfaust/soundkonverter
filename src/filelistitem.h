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


#ifndef FILELISTITEM_H
#define FILELISTITEM_H

#include "metadata/tagengine.h"

#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QLabel>
#include <QWeakPointer>

#include <KUrl>


/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class FileListItem : public QTreeWidgetItem
{
public:
    enum State {
        WaitingForConversion,
        Ripping,
        Converting,
        ApplyingReplayGain,
        WaitingForAlbumGain,
        ApplyingAlbumGain,
        Stopped
    };

    // code values will be displayed in the log files
    enum ReturnCode {
        Succeeded                       = 0,
        SucceededWithProblems           = 1,
        StoppedByUser                   = 2,
        Skipped                         = 3,
        BackendNeedsConfiguration       = 4,
        DiscFull                        = 5,
        Failed                          = 6
    };

    FileListItem( QTreeWidget *parent );
    FileListItem( QTreeWidget *parent, QTreeWidgetItem *after );
    ~FileListItem();

    int conversionOptionsId;
    TagData *tags;              // we need to instruct the tagengine to read the tags from the file!
                                // and the user can change them!
    KUrl url;                   // the original input file path name
    // KUrl outputUrl;             // if the user wants to change the output directory/file name per file
    QString codecName;          // the codec name of the input file
    State state;                // is this item being converted or ripper or etc.
    ReturnCode returnCode;      // what's the return code of the conversion
    bool local;                 // is this a local file?
    int track;                  // the number of the track, if it is on an audio cd
                                // if it is lower than 0, it isn't an audio cd track at all
    int tracks;                 // the total amount of tracks on the cd
    QString device;             // the device of the audio cd

    float length;               // the length of the track, used for the calculation of the progress bar
    QString notifyCommand;      // execute this command, when the file is converted (%i=input file, %o=output file)

    int logId;                  // the id the item is registered at the logger with, 0 if the conversion hasn't started yet

    QWeakPointer<QLabel> lInfo; // a pointer to button to show additional information (e.g. error log). if no butotn shall be shown the pointer must be 0
};

class FileListItemDelegate : public QItemDelegate
{
public:
    FileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif
