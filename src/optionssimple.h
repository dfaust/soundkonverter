

#ifndef OPTIONSSIMPLE_H
#define OPTIONSSIMPLE_H

// #include "outputdirectory.h"
// #include "conversionoptions.h"

#include <QWidget>

class Config;
class OutputDirectory;
// class ConversionOptions;
// class OptionsDetailed;

class KToolBarButton;
class KPushButton;
class QLabel;
class QCheckBox;
class KComboBox;
class CodecPlugin;

/**
 * @short The options widget for setting up the output options very easy
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class OptionsSimple : public QWidget
{
    Q_OBJECT
public:
    /** Constructor */
    OptionsSimple( Config *_config, /*OptionsDetailed*,*/ const QString &text, QWidget* parent=0 );

    /** Detructor */
    virtual ~OptionsSimple();

    void init();

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
//     void setBpmChecked( bool enabled ) { return cBpm->setChecked( enabled ); }
    void setCurrentPlugin( CodecPlugin *plugin ) { currentPlugin = plugin; }

    QString currentProfile();
    QString currentFormat();
    bool isReplayGainChecked();
//     bool isBpmChecked() { return cBpm->isChecked(); }

    OutputDirectory *outputDirectory; // this way it's easier to sync simple and detailed options

private:
    KComboBox* cProfile;
    KPushButton* pProfileRemove;
    KPushButton* pProfileInfo;
    KComboBox* cFormat;
    KPushButton* pFormatInfo;

    QCheckBox *cReplayGain;
//     QCheckBox *cBpm;
    QLabel *lEstimSize;

    Config *config;
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
