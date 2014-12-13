

#ifndef ABOUTPLUGINS_H
#define ABOUTPLUGINS_H

#include <QDialog>
#include <QMap>

class Config;
class BackendPlugin;
class QLabel;
class QPushButton;


/**
 * @short Shows the logs that are collected by the logger
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class AboutPlugins : public QDialog
{
    Q_OBJECT
public:
    /** Default Constructor */
    AboutPlugins( Config *_config, QWidget *parent=0, Qt::WindowFlags f=0 );

    /** Default Destructor */
    ~AboutPlugins();

private:
    Config *config;

    BackendPlugin *currentPlugin;

    QLabel *aboutPluginLabel;
    QPushButton *configurePlugin;

    QMap<QString,QString> problemInfos;

private slots:
    void currentPluginChanged( const QString& pluginName );
    void configurePluginClicked();
    void showProblemInfo( const QString& problemId );

};

#endif // ABOUTPLUGINS_H
