
#ifndef CONFIGGENERALPAGE_H
#define CONFIGGENERALPAGE_H

#include "configpagebase.h"

#include "ui_configgeneralpage.h"

class Config;

class ConfigGeneralPage : public ConfigPageBase
{
    Q_OBJECT

public:
    ConfigGeneralPage(Config *_config, QWidget *parent=0);
    ~ConfigGeneralPage();

private:
    Ui::ConfigGeneralPage ui;

    Config *config;

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();
    void profileChanged();
};

#endif
