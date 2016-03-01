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
#ifndef FILEOPENER_H
#define FILEOPENER_H

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
class FileOpener : public KDialog
{
    Q_OBJECT
public:
    FileOpener( Config *_config, QWidget *parent=0, Qt::WFlags f=0 );
    ~FileOpener();

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
    void showHelp();

signals:
    void open( const KUrl::List& files, ConversionOptions *conversionOptions );

};

#endif
