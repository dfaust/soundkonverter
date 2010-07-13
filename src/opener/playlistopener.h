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
    void done( const KUrl::List& files, ConversionOptions *conversionOptions );

};

#endif
