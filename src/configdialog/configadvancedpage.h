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
#ifndef CONFIGADVANCEDPAGE_H
#define CONFIGADVANCEDPAGE_H

#include "configpagebase.h"

class Config;
class QCheckBox;
class KIntSpinBox;
class KComboBox;

/**
	@author Daniel Faust <hessijames@gmail.com>
 */
class ConfigAdvancedPage : public ConfigPageBase
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    ConfigAdvancedPage( Config *_config, QWidget *parent=0 );

    /**
     * Default Destructor
     */
    ~ConfigAdvancedPage();

private:
    KComboBox *cPreferredOggVorbisExtension;
    KComboBox *cPreferredVorbisCommentCommentTag;
    KComboBox *cPreferredVorbisCommentTrackTotalTag;
    KComboBox *cPreferredVorbisCommentDiscTotalTag;
    QCheckBox *cUseVFATNames;
    QCheckBox *cEjectCdAfterRip;
    QCheckBox *cWriteLogFiles;
    QCheckBox *cUseSharedMemoryForTempFiles;
    KIntSpinBox *iMaxSizeForSharedMemoryTempFiles;
    QCheckBox *cUsePipes;

    Config *config;

public slots:
    void resetDefaults();
    void saveSettings();

private slots:
    void somethingChanged();

};

#endif
