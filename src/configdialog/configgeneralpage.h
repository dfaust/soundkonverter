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
class QComboBox;
class QSpinBox;
class QLineEdit;
class QPushButton;

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
    ~ConfigGeneralPage();

private:
    QComboBox *cStartTab;
    QComboBox *cDefaultProfile;
    QComboBox *cDefaultFormat;
//     QComboBox *cPriority;
//     QStringList sPriority;
    QComboBox *cConflictHandling;
    QSpinBox *iNumFiles;
    QCheckBox *cWaitForAlbumGain;
    QCheckBox *cCopyIfSameCodec;
    QComboBox *cReplayGainGrouping;
    QSpinBox *iNumReplayGainFiles;

    Config *config;

//     int profileIndex( const QString& string );
//     int formatIndex( const QString& string );

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();
//     void selectDir();
    void profileChanged();


};

#endif
