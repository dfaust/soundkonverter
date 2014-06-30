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

//
// C++ Interface: configbackendspage
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGBACKENDSPAGE_H
#define CONFIGBACKENDSPAGE_H

#include "configpagebase.h"

#include <QGroupBox>

class KComboBox;
class KPushButton;
class QCheckBox;
class QListWidget;
class QToolButton;
class QListWidgetItem;


/**
	@author Daniel Faust <hessijames@gmail.com>
 */
class BackendsListWidget : public QGroupBox
{
    Q_OBJECT
public:
    /** Default Constructor */
    BackendsListWidget( const QString& _name, Config *_config, QWidget *parent=0 );

    /** Default Destructor*/
    ~BackendsListWidget();

    void setFormat( const QString& _format );
    void addItem( const QString& item );
    void clear();
    QStringList getList();
    bool changed();
    void resetOrder();

private:
    Config *config;
    QString name;
    QString format;

    QStringList originalOrder;

    QListWidget *lBackends;
    QToolButton *pUp;
    QToolButton *pDown;
    QToolButton *pConfigure;
    QToolButton *pInfo;

private slots:
    void itemSelected( int item );
    void up();
    void down();
    void configure();
    void info();

signals:
    void orderChanged();
};



/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigBackendsPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /** Default Constructor */
    ConfigBackendsPage( Config *_config, QWidget *parent=0 );

    /** Default Destructor */
    ~ConfigBackendsPage();

private:
    Config *config;

    KComboBox *cSelectorRipper;
    KComboBox *cSelectorFormat;

    QList<QCheckBox*> filterCheckBoxes;
    QList<KPushButton*> filterConfigButtons;

    BackendsListWidget *decoderList;
    BackendsListWidget *encoderList;
    BackendsListWidget *replaygainList;

    KPushButton *pConfigureRipper;
    KPushButton *pShowOptimizations;

    QString currentFormat;

private slots:
    void somethingChanged();
    void ripperChanged( const QString& pluginName );
    void formatChanged( const QString& format, bool ignoreChanges = false );
    void configureRipper();
    void configureFilter();
    void showOptimizations();

public slots:
    void resetDefaults();
    void saveSettings();
};

#endif
