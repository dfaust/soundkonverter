
#ifndef DIRDIALOG_H
#define DIRDIALOG_H

#include <QDialog>

class Config;
class Options;
class ConversionOptions;

class QLabel;
class QCheckBox;
class QPushButton;
class KUrlRequester;
class QListWidget;


/**
 * @short The Dir Opener
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class DirOpener : public QDialog
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
    DirOpener( Config *config, Mode _mode, QWidget *parent=0, Qt::WindowFlags f=0 );

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
    QListWidget *fileTypes;
    QPushButton *pSelectAll;
    QPushButton *pSelectNone;
    QCheckBox *cRecursive;

signals:
    void open( const QUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions = 0 );
};


#endif
