
#ifndef _SOUNDKONVERTERVIEW_H_
#define _SOUNDKONVERTERVIEW_H_

#include "ui_soundkonverterview.h"

#include <QWidget>
#include <QUrl>

class QMenu;
class QAction;
class KActionMenu;

class ComboButton;
class Config;
class Logger;

class SoundKonverterView : public QWidget
{
    Q_OBJECT

public:
    SoundKonverterView(Logger *_logger, Config *_config, QWidget *parent);
    ~SoundKonverterView();

    void addConvertFiles(const QList<QUrl>& urls, QString _profile, QString _format, const QString& directory, const QString& notifyCommand="");
    void loadAutosaveFileList();

    QAction *getStartAction()
    {
        return startAction;
    }

    KActionMenu *getStopActionMenu()
    {
        return stopActionMenu;
    }

    void startConversion();
    void killConversion();

signals:
    /** Use this signal to change the content of the statusbar */
//     void signalChangeStatusbar(const QString& text);

    /** Use this signal to change the content of the caption */
//     void signalChangeCaption( const QString& text );

public slots:
    bool showCdDialog(const QString& device="", QString _profile="", QString _format="", const QString& directory="", const QString& notifyCommand="");
    void loadFileList(bool user=true);
    void saveFileList(bool user=true);
    void updateFileList();

private slots:
    void addClicked( int index );
    void showFileDialog();
    void showDirDialog();
    void showUrlDialog();
    void showPlaylistDialog();

    // connected to fileList
    /** The count of items in the file list has changed to @p count */
    void fileCountChanged(int count);
    /** The conversion has started */
    void conversionStarted();
    /** The conversion has stopped */
    void conversionStopped(bool failed);
    /** Conversion will continue/stop after current files have been converted */
    void queueModeChanged(bool enabled);

private:
    Ui::SoundKonverterView ui;

    Config *config;
    Logger *logger;

    QAction *startAction;

    KActionMenu *stopActionMenu;
    QAction *killAction;
    QAction *stopAction;
    QAction *continueAction;

    void cleanupParameters(QString *profile, QString *format);

signals:
    void progressChanged(const QString& progress);
    void signalConversionStarted();
    void signalConversionStopped(bool failed);
    void showLog(const int logId);
};

#endif // _soundKonverterVIEW_H_
