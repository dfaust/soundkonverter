
#ifndef CONFIGADVANCEDPAGE_H
#define CONFIGADVANCEDPAGE_H

#include "configpagebase.h"

#include "ui_configadvancedpage.h"

class Config;

class ConfigAdvancedPage : public ConfigPageBase
{
    Q_OBJECT

public:
    ConfigAdvancedPage(Config *_config, QWidget *parent=0);
    ~ConfigAdvancedPage();

private:
    Ui::ConfigAdvancedPage ui;

    Config *config;

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();
};

#endif
