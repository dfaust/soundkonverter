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
