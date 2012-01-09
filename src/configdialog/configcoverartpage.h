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
#ifndef CONFIGCOVERARTPAGE_H
#define CONFIGCOVERARTPAGE_H

#include "configpagebase.h"

class Config;
class QRadioButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
 */
class ConfigCoverArtPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    ConfigCoverArtPage( Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    virtual ~ConfigCoverArtPage();

private:
    QRadioButton *rWriteCoversAlways;
    QRadioButton *rWriteCoversAuto;
    QRadioButton *rWriteCoversNever;

//     QCheckBox *cCopyCover;
//     QCheckBox *cEmbedCover;
//     QListView *lCoverList;

    Config *config;

public slots:
    void resetDefaults();
    void saveSettings();

};

#endif
