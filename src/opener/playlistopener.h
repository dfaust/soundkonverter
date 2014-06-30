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

//
// C++ Interface: opener
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLAYLISTOPENER_H
#define PLAYLISTOPENER_H

#include <KDialog>

#include <KUrl>

class Config;
class Options;
class QLabel;
class ConversionOptions;
class KDialog;
class KPushButton;
class KFileDialog;

/** @author Daniel Faust <hessijames@gmail.com> */
class PlaylistOpener : public KDialog
{
    Q_OBJECT
public:
    PlaylistOpener( Config *_config, QWidget *parent=0, Qt::WFlags f=0 );
    ~PlaylistOpener();

    /** true if the file dialog was aborted (don't execute the dialog) */
    bool dialogAborted;

private:
    Config *config;

    KFileDialog *fileDialog;
    Options *options;
    KUrl::List urls;
    KPushButton *pAdd;
    KPushButton *pCancel;
    QLabel *formatHelp;

private slots:
    void fileDialogAccepted();
    void okClickedSlot();

signals:
    void open( const KUrl::List& files, ConversionOptions *conversionOptions );

};

#endif
