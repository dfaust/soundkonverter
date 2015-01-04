

#ifndef OPTIONSSIMPLE_H
#define OPTIONSSIMPLE_H

#include "ui_optionssimple.h"

#include <QWidget>
#include <QPointer>

class Config;
class OutputDirectory;

class CodecPlugin;

/** The options widget for setting up the output options very easy */
class OptionsSimple : public QWidget
{
    Q_OBJECT
public:
    OptionsSimple(QWidget* parent=0);
    ~OptionsSimple();

    void init(Config *config, const QString &text);

    /** Set the current options */
//     void setCurrentOptions( const ConversionOptions& );

    /** Refills the whole form (e.g. after a config change) */
//     void refill(); // TODO syncronize with optionsDetailed
//
    void setCurrentProfile( const QString& profile );
    void setCurrentFormat( const QString& format );
    void setCurrentOutputDirectory( const QString& directory );
    void setCurrentOutputDirectoryMode( int mode );
    void setReplayGainEnabled( bool enabled, const QString& toolTip = "" );
    void setReplayGainChecked( bool enabled );
    void setCurrentPlugin( CodecPlugin *plugin ) { currentPlugin = plugin; }

    QString currentProfile();
    QString currentFormat();
    bool isReplayGainChecked();

    OutputDirectory *outputDirectory; // this way it's easier to sync simple and detailed options

private:
    Ui::OptionsSimple ui;

    QPointer<Config> config;
    CodecPlugin *currentPlugin;

public slots:
    void currentDataRateChanged( int dataRate );
    void updateProfiles();

private slots:
    void showHelp();
    void profileInfo();
    void profileRemove();
    void formatInfo();
    void profileChanged();
//     void formatChanged();
    void outputDirectoryChanged();

    void somethingChanged();

signals:
    void optionsChanged();
    void customProfilesEdited();
};

#endif
