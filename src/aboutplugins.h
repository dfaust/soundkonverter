/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/



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
