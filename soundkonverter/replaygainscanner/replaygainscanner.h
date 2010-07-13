

#ifndef REPLAYGAINSCANNER_H
#define REPLAYGAINSCANNER_H

#include <kdialog.h>

// #include "replaygainfilelist.h"

class Config;
class Logger;
class ComboButton;
class ReplayGainFileList;

class QCheckBox;
class KPushButton;
class QProgressBar;
class QTreeWidget;
class KFileDialog;


/**
 * @short The Replay Gain Tool
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class ReplayGainScanner : public KDialog
{
    Q_OBJECT
public:
    /** Constructor */
    ReplayGainScanner( Config*, Logger*, QWidget *parent=0, Qt::WFlags f=0 );

    /** Destructor */
    virtual ~ReplayGainScanner();

    void addFiles( KUrl::List urls );

private slots:
    void addClicked( int );
    void showDirDialog();
    void showFileDialog();
    void fileDialogAccepted();
    void showHelp();
    void calcReplayGainClicked();
    void removeReplayGainClicked();
    void cancelClicked();
    void processStarted();
    void processStopped();
    void updateProgress( int progress, int totalSteps );

private:
    ComboButton* cAdd;
    QCheckBox* cForce;
    ReplayGainFileList* lList;
    QProgressBar* pProgressBar;
    KPushButton* pTagVisible;
    KPushButton* pRemoveTag;
    KPushButton* pCancel;
    KPushButton* pClose;
    KFileDialog *fileDialog;

//     TagEngine* tagEngine;
    Config* config;
    Logger* logger;

    QTime elapsedTime;

signals:
    void addFile( const QString& );
    void addDir( const QString&, const QStringList& filter = QStringList(), bool recursive = true );
    void calcAllReplayGain( bool force );
    void removeAllReplayGain();
    void cancelProcess();
};

#endif // REPLAYGAINSCANNER_H
