
#ifndef CONFIGBACKENDSPAGE_H
#define CONFIGBACKENDSPAGE_H

#include "configpagebase.h"

#include "ui_configbackendspage.h"

class QToolButton;
class QCheckBox;

class ConfigBackendsPage : public ConfigPageBase
{
    Q_OBJECT

public:
    ConfigBackendsPage(Config *_config, QWidget *parent=0);
    ~ConfigBackendsPage();

private:
    Ui::ConfigBackendsPage ui;

    Config *config;

    QList<QCheckBox*> filterCheckBoxes;
    QList<QToolButton*> filterConfigButtons;

    QString currentFormat;

private slots:
    void somethingChanged();
    void ripperChanged(const QString& pluginName);
    void formatChanged(const QString& format, bool ignoreChanges=false);
    void configureRipper();
    void configureFilter();
    void showOptimizations();

public slots:
    void resetDefaults();
    void saveSettings();
};

#endif
