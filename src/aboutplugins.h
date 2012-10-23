

#ifndef ABOUTPLUGINS_H
#define ABOUTPLUGINS_H

#include <KDialog>

class Config;
class BackendPlugin;
class QLabel;
class KPushButton;


/**
 * @short Shows the logs that are collected by the logger
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class AboutPlugins : public KDialog
{
    Q_OBJECT
public:
    /** Default Constructor */
    AboutPlugins( Config *_config, QWidget *parent=0, Qt::WFlags f=0 );

    /** Default Destructor */
    ~AboutPlugins();

private:
    Config *config;

    BackendPlugin *currentPlugin;

    QLabel *aboutPluginLabel;
    KPushButton *configurePlugin;

    QMap<QString,QString> problemInfos;

private slots:
    void currentPluginChanged( const QString& pluginName );
    void configurePluginClicked();
    void showProblemInfo( const QString& problemId );

};

#endif // ABOUTPLUGINS_H
