
#ifndef REPLAYGAINSCANNER_H
#define REPLAYGAINSCANNER_H

#include <QDialog>

class Config;
class Logger;
class ComboButton;
class ReplayGainFileList;
class ProgressIndicator;

class QCheckBox;
class QPushButton;
class QTreeWidget;
class QFileDialog;


/**
 * @short The Replay Gain Tool
 * @author Daniel Faust <hessijames@gmail.com>
 */
class ReplayGainScanner : public QDialog
{
    Q_OBJECT
public:
    ReplayGainScanner( Config*, Logger*, bool showMainWindowButton = false, QWidget *parent=0, Qt::WindowFlags f=0 );
    ~ReplayGainScanner();

    void addFiles( QList<QUrl> urls );

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
    QPushButton *pShowMainWindow;
    QCheckBox *cForce;
    ReplayGainFileList *fileList;
    ProgressIndicator *progressIndicator;
    QPushButton *pTagVisible;
    QPushButton *pRemoveTag;
    QPushButton *pCancel;
    QPushButton *pClose;
    QFileDialog *fileDialog;

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
