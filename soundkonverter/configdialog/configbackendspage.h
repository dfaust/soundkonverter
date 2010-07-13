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
    /**
     * Default Constructor
     */
    BackendsListWidget( const QString& _name, Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    virtual ~BackendsListWidget();

    void addItem( const QString& item );
    void clear();
    QStringList getList();
    bool changed();
    void resetOrder();

private:
    Config *config;
    QString name;

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
    /**
     * Default Constructor
     */
    ConfigBackendsPage( Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    virtual ~ConfigBackendsPage();

private:
    Config *config;

    QGroupBox *formatGroup;
    KComboBox *cRipper;
    KComboBox *cSelectorFormat;

    BackendsListWidget *decoderList;
    BackendsListWidget *encoderList;
    BackendsListWidget *replaygainList;
    
    QString currentFormat;

private slots:
    void formatChanged( const QString& format );
//     void encoderChanged( const QString& encoder );
//     void configureEncoder();
//     void orderChanged();

public slots:
    void resetDefaults();
    void saveSettings();
};

#endif
