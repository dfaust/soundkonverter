
#ifndef REPLAYGAINSCANNER_H
#define REPLAYGAINSCANNER_H

#include <KDialog>

class Config;
class Logger;
class ComboButton;
class ReplayGainFileList;
class ProgressIndicator;

class QCheckBox;
class KPushButton;
class QTreeWidget;
class KFileDialog;


/**
 * @short The Replay Gain Tool
 * @author Daniel Faust <hessijames@gmail.com>
 */
class ReplayGainScanner : public KDialog
{
    Q_OBJECT
public:
    ReplayGainScanner( Config*, Logger*, bool showMainWindowButton = false, QWidget *parent=0, Qt::WFlags f=0 );
    ~ReplayGainScanner();

    void addFiles( KUrl::List urls );

private slots:
    void addClicked( int );
    void showDirDialog();
    void showFileDialog();
    void showMainWindowClicked();
    void fileDialogAccepted();
    void showHelp();
    void calcReplayGainClicked();
    void removeReplayGainClicked();
    void cancelClicked();
    void closeClicked();
    void processStarted();
    void processStopped();
    void progressChanged( const QString& progress );

private:
    ComboButton *cAdd;
    KPushButton *pShowMainWindow;
    QCheckBox *cForce;
    ReplayGainFileList *fileList;
    ProgressIndicator *progressIndicator;
    KPushButton *pTagVisible;
    KPushButton *pRemoveTag;
    KPushButton *pCancel;
    KPushButton *pClose;
    KFileDialog *fileDialog;

    Config *config;
    Logger *logger;

signals:
    void addFile( const QString& );
    void addDir( const QString&, const QStringList& filter = QStringList(), bool recursive = true );
    void calcAllReplayGain( bool force );
    void removeAllReplayGain();
    void cancelProcess();
    void showMainWindow();
};

#endif // REPLAYGAINSCANNER_H
