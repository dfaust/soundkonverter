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
#ifndef URLOPENER_H
#define URLOPENER_H

#include <KDialog>

#include <KUrl>

class Config;
class Options;
class QLabel;
class ConversionOptions;
class KDialog;
class KFileWidget;
class KPushButton;
class KUrlRequester;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class UrlOpener : public KDialog
{
    Q_OBJECT
public:
    enum DialogPage {
        FileOpenPage,
        ConversionOptionsPage
    };

    UrlOpener( Config *_config, QWidget *parent=0, Qt::WFlags f=0 );
    ~UrlOpener();

    DialogPage currentPage() { return page; }

private:
    Config *config;

    KUrlRequester *urlRequester;
    Options *options;
    DialogPage page;
    QLabel *lSelector;
    QLabel *lOptions;
    KUrl::List urls;
    KPushButton *pProceed;
    KPushButton *pAdd;
    KPushButton *pCancel;

private slots:
    void proceedClickedSlot();
    void okClickedSlot();

signals:
    void open( const KUrl::List& files, ConversionOptions *conversionOptions );

};

#endif
