//
// C++ Interface: configdialog
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <KPageDialog>

class Config;
class ConfigGeneralPage;
class ConfigAdvancedPage;
class ConfigBackendsPage;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigDialog : public KPageDialog
{
    Q_OBJECT
public:
    ConfigDialog( Config *_config, QWidget *parent = 0/*, Page startPage = GeneralPage*/ );

    ~ConfigDialog();

private:
    Config *config;

    KPageWidgetItem *generalPage;
    ConfigGeneralPage *configGeneralPage;
    KPageWidgetItem *advancedPage;
    ConfigAdvancedPage *configAdvancedPage;
    KPageWidgetItem *backendsPage;
    ConfigBackendsPage *configBackendsPage;

    bool lastUseVFATNames;
    int lastConflictHandling;

private slots:
    void pageChanged( KPageWidgetItem *current, KPageWidgetItem *before );
    void configChanged( bool state );
    void okClicked();
    void applyClicked();
    void defaultClicked();

signals:
    void updateFileList();
};

#endif
