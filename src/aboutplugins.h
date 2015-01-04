
#ifndef ABOUTPLUGINS_H
#define ABOUTPLUGINS_H

#include "ui_aboutplugins.h"

#include <QDialog>
#include <QMap>

class Config;
class BackendPlugin;

/** Shows the logs that are collected by the logger */
class AboutPlugins : public QDialog
{
    Q_OBJECT

public:
    AboutPlugins(Config *_config, QWidget *parent=0, Qt::WindowFlags f=0);
    ~AboutPlugins();

private:
    Ui::AboutPlugins ui;

    Config *config;

    BackendPlugin *currentPlugin;

    QMap<QString,QString> problemInfos;

private slots:
    void currentPluginChanged(const QString& pluginName);
    void configurePluginButtonClicked();
    void showProblemInfo(const QString& problemId);
};

#endif // ABOUTPLUGINS_H
