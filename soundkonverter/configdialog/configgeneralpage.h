//
// C++ Interface: configgeneralpage
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGGENERALPAGE_H
#define CONFIGGENERALPAGE_H

#include "configpagebase.h"

class Config;
class QCheckBox;
class KComboBox;
class KIntSpinBox;
class KLineEdit;
class KPushButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigGeneralPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /** Default Constructor */
    ConfigGeneralPage( Config *_config, QWidget *parent=0 );

    /** Default Destructor */
    virtual ~ConfigGeneralPage();

private:
    KComboBox *cStartTab;
    KComboBox *cDefaultProfile;
    QStringList sDefaultProfile;
    KComboBox *cDefaultFormat;
    QStringList sDefaultFormat;
//     KComboBox *cPriority;
//     QStringList sPriority;
    QCheckBox *cUseVFATNames;
    KComboBox *cConflictHandling;
    QStringList sConflictHandling;
    KIntSpinBox *iNumFiles;
    KIntSpinBox *iUpdateDelay;

    Config *config;

//     int profileIndex( const QString& string );
//     int formatIndex( const QString& string );

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
//     void selectDir();
    void profileChanged();


};

#endif
