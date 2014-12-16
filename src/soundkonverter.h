
#ifndef SOUNDKONVERTER_H
#define SOUNDKONVERTER_H

#include <KXmlGuiWindow>
#include <QUrl>
#include <QPointer>

class SoundKonverterView;
class Config;
class Logger;
class LogViewer;
class ReplayGainScanner;

class KStatusNotifierItem;

/** This class serves as the main window for soundKonverter. It handles the menus, toolbars, and status bars. */
class SoundKonverter : public KXmlGuiWindow
{
    Q_OBJECT

public:
    SoundKonverter();
    ~SoundKonverter();

    virtual void saveProperties(KConfigGroup& configGroup);

    void showSystemTray();
    void addConvertFiles(const QList<QUrl>& urls, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand);
    void addReplayGainFiles(const QList<QUrl>& urls);
    bool ripCd(const QString& device, const QString& profile, const QString& format, const QString& directory, const QString& notifyCommand);
    void startConversion();
    void loadAutosaveFileList();
    void startupChecks();

    void setAutoCloseEnabled(bool autoCloseEnabled)
    {
        this->autoCloseEnabled = autoCloseEnabled;
    }

private slots:
    void showConfigDialog();
    void showLogViewer(const int logId=0);
    void showReplayGainScanner();
    void replayGainScannerClosed();
    void showMainWindow();
    void showAboutPlugins();
    void progressChanged(const QString& progress);

    /** The conversion has started */
    void conversionStarted();
    /** The conversion has stopped */
    void conversionStopped(bool failed);

private:
    Logger *logger;
    Config *config;
    SoundKonverterView *soundKonverterView;

    QPointer<ReplayGainScanner> replayGainScanner;
    QPointer<LogViewer> logViewer;
    QPointer<KStatusNotifierItem> systemTray;

    /** exit soundkonverter after all files have been converted */
    bool autoCloseEnabled;

    void setupActions();
};

#endif // _SOUNDKONVERTER_H_
