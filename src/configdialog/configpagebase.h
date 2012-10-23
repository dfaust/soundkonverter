//
// C++ Interface: configpagebase
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGPAGEBASE_H
#define CONFIGPAGEBASE_H

#include <QWidget>

class Config;

#define ConfigDialogOffset 20

#define ConfigDialogSpacingSmall 5
#define ConfigDialogSpacingMedium 10
#define ConfigDialogSpacingBig 20

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigPageBase : public QWidget
{
    Q_OBJECT
public:
    ConfigPageBase( QWidget *parent=0 );
    virtual ~ConfigPageBase();

public slots:
    virtual void resetDefaults();
    virtual void saveSettings();

signals:
    void configChanged( bool state = true );

};

#endif
