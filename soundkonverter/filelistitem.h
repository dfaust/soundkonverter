//
// C++ Interface: filelistitem
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILELISTITEM_H
#define FILELISTITEM_H

#include "metadata/tagengine.h"

#include <QTreeWidgetItem>
#include <QItemDelegate>

#include <KUrl>

// class ConversionOptions;


/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class FileListItem : public QTreeWidgetItem
{
public:
    /** Constructor */
    FileListItem( QTreeWidget *parent );

    /** Constructor */
    FileListItem( QTreeWidget *parent, QTreeWidgetItem *after );

    /** Destructor */
    virtual ~FileListItem();

    int conversionOptionsId;
    TagData *tags;              // we need to instruct the tagengine to read the tags from the file!
                                // and the user can change them!
    KUrl url;                   // the original input file path name
//     QString fileName;           // just the name of the file (no path)
//     QString filePathName;       // the path and name of the file
    KUrl outputUrl;             // if the user wants to change the output directory/file name per file!
//     QString mimeType;           // the mime type of the file format
//     QString fileFormat;         // the extension of the file format (for easier use)
    QString codecName;          // the codec name of the input file
    bool converting;            // is this item being converted at the moment?
    bool local;                 // is this a local file?
    int track;                  // the number of the track, if it is on an audio cd
                                // if it is lower than 0, it isn't an audio cd track at all
    int tracks;                 // the total amount of tracks on the cd
    QString device;             // the device of the audio cd
    bool ripping;               // is this track currently being ripped?

    float time;                 // the length of the track, used for the calculation of the progress bar
    QString notify;             // execute this command, when the file is converted
};

class FileListItemDelegate : public QItemDelegate
{
public:
    FileListItemDelegate( QObject *parent = 0 );

    virtual void paint( QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif
