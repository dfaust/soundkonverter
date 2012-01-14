
#ifndef OPTIONSDETAILED_H
#define OPTIONSDETAILED_H

#include <QWidget>
#include <QCheckBox>
#include <QDomDocument>

class Config;
class OutputDirectory;
class CodecPlugin;
class ConversionOptions;
// class CodecWidget;

class QGridLayout;
class QLabel;
class KComboBox;
class KPushButton;
class QToolButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class OptionsDetailed : public QWidget
{
    Q_OBJECT
public:
    OptionsDetailed( Config *_config, QWidget *parent=0 );
    ~OptionsDetailed();

    void init();

    /** Return the current options */
    ConversionOptions *currentConversionOptions();
    /** Set the current options */
    bool setCurrentConversionOptions( ConversionOptions *options );

    QString currentProfile();
    void setCurrentProfile( const QString& profile );
    QString currentFormat();
    void setCurrentFormat( const QString& format );
    bool loadCustomProfile( const QString& profile );
    void setReplayGainChecked( bool enabled );
//     void setBpmEnabled( bool enabled ) { cBpm->setChecked(enabled); }

    bool isReplayGainEnabled( QString *toolTip = 0 );
    bool isReplayGainChecked();
//     bool isBpmEnabled() { return cBpm->isChecked(); }
    CodecPlugin *getCurrentPlugin();

    OutputDirectory *outputDirectory; // this way it's easier to sync simple and detailed options

//     QSize sizeHint();

private:
    QGridLayout *grid;
    KComboBox *cFormat;
    KComboBox *cPlugin;
    QWidget *wPlugin;
    QCheckBox *cReplayGain;
//     QCheckBox *cBpm;
    QLabel *lEstimSize;
    KPushButton *pProfileSave;
    QToolButton *pProfileLoad;

    Config *config;

    CodecPlugin *currentPlugin;

//     QSize size_hint;

public slots:
    void somethingChanged();
    void loadCustomProfileButtonClicked();
    bool saveCustomProfile();
    void updateProfiles();

private slots:
    void formatChanged( const QString& format );
    void encoderChanged( const QString& encoder );

signals:
    // connected to optionsSimple
    void currentDataRateChanged( int rate );
//     void optionsChanged();
    void customProfilesEdited();
};

#endif
