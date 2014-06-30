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


#ifndef DIRDIALOG_H
#define DIRDIALOG_H

#include <kdialog.h>

class Config;
class Options;
class ConversionOptions;

class QLabel;
class QCheckBox;
class KPushButton;
class KUrlRequester;
class KListWidget;


/**
 * @short The Dir Opener
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class DirOpener : public KDialog
{
    Q_OBJECT
public:
    enum DialogPage {
        DirOpenPage,
        ConversionOptionsPage
    };

    enum Mode {
        Convert    = 0x0001,
        ReplayGain = 0x0002
    };

    /** Constructor */
    DirOpener( Config *config, Mode _mode, QWidget *parent=0, Qt::WFlags f=0 );

    /** Destructor */
    ~DirOpener();

    /** true if the file dialog was aborted (don't execute the dialog) */
    bool dialogAborted;

private slots:
    void proceedClicked();
    void addClicked();
    void selectAllClicked();
    void selectNoneClicked();
    void showHelp();

private:
    /** config pointer */
    Config *config;

    /** the widget for selecting the directory */
    QWidget *dirOpenerWidget;
    /** the conversion options editor widget */
    Options *options;
    /** the current page */
    DialogPage page;
    /** the dialog mode */
    Mode mode;

    QLabel *lSelector;
    QLabel *lOptions;

    KUrlRequester *uDirectory;
    KListWidget *fileTypes;
    KPushButton *pSelectAll;
    KPushButton *pSelectNone;
    QCheckBox *cRecursive;

signals:
    void open( const KUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions = 0 );
};


#endif
