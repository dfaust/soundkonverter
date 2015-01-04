
#ifndef OPTIONSDETAILED_H
#define OPTIONSDETAILED_H

#include "ui_optionsdetailed.h"

#include <QDomDocument>
#include <QMap>
#include <QWidget>
#include <QPointer>

class CodecPlugin;
class Config;
class ConversionOptions;
class FilterPlugin;
class CodecWidget;
class FilterWidget;

class OptionsDetailed : public QWidget
{
    Q_OBJECT
public:
    OptionsDetailed(QWidget *parent=0);
    ~OptionsDetailed();

    void init(Config *config);

    /** Return the current options */
    ConversionOptions *currentConversionOptions( bool saveLastUsed = true );
    /** Set the current options */
    bool setCurrentConversionOptions( ConversionOptions *options );

    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QString currentFormat();
    void setCurrentFormat( const QString& format );
    void resetFilterOptions();
    void setReplayGainChecked( bool enabled );

    bool isReplayGainEnabled( QString *toolTip = 0 );
    bool isReplayGainChecked();
    CodecPlugin *getCurrentPlugin();

private:
    Ui::OptionsDetailed ui;

    QPointer<Config> config;

    QPointer<CodecPlugin> currentPlugin;
    QPointer<CodecWidget> pluginWidget;
    QMap<FilterWidget*, FilterPlugin*> filterWidgets;

public slots:
    void somethingChanged();
    void loadCustomProfileButtonClicked();
    bool saveCustomProfile( bool lastUsed = false );
    void updateProfiles();

private slots:
    void formatChanged( const QString& format );
    void encoderChanged( const QString& encoder );
    void configurePlugin();

signals:
    // connected to optionsSimple
    void currentDataRateChanged( int rate );
//     void optionsChanged();
    void customProfilesEdited();
};

#endif
