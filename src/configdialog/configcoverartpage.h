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
class QLabel;
class QAbstractButton;
class KLineEdit;

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
    ~ConfigCoverArtPage();

private:
    QRadioButton *rWriteCoversAlways;
    QRadioButton *rWriteCoversAuto;
    QRadioButton *rWriteCoversNever;

    QLabel       *lWriteCoverName;
    QRadioButton *rWriteCoverNameTitle;
    QRadioButton *rWriteCoverNameDefault;
    QLabel       *lWriteCoverNameDefaultLabel;
    KLineEdit    *lWriteCoverNameDefaultEdit;

//     QCheckBox *cCopyCover;
//     QCheckBox *cEmbedCover;
//     QListView *lCoverList;

    Config *config;

private slots:
    void somethingChanged();
    void writeCoversChanged( QAbstractButton *button );

public slots:
    void resetDefaults();
    void saveSettings();

};

#endif
