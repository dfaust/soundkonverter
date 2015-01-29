
#ifndef CONFIGCOVERARTPAGE_H
#define CONFIGCOVERARTPAGE_H

#include "configpagebase.h"

#include "ui_configcoverartpage.h"

class Config;
class QAbstractButton;

class ConfigCoverArtPage : public ConfigPageBase
{
    Q_OBJECT

public:
    ConfigCoverArtPage(Config *_config, QWidget *parent=0);
    ~ConfigCoverArtPage();

private:
    Ui::ConfigCoverArtPage ui;

    Config *config;

private slots:
    void somethingChanged();
    void writeCoversChanged(QAbstractButton *button);

public slots:
    void resetDefaults();
    void saveSettings();
};

#endif
