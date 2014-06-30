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



#ifndef OPTIONSSIMPLE_H
#define OPTIONSSIMPLE_H

// #include "outputdirectory.h"
// #include "conversionoptions.h"

#include <QWidget>

class Config;
class OutputDirectory;
// class ConversionOptions;
// class OptionsDetailed;

class KToolBarButton;
class KPushButton;
class QLabel;
class QCheckBox;
class KComboBox;
class CodecPlugin;

/**
 * @short The options widget for setting up the output options very easy
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class OptionsSimple : public QWidget
{
    Q_OBJECT
public:
    /** Constructor */
    OptionsSimple( Config *_config, /*OptionsDetailed*,*/ const QString &text, QWidget* parent=0 );

    /** Detructor */
    ~OptionsSimple();

    void init();

    /** Set the current options */
//     void setCurrentOptions( const ConversionOptions& );

    /** Refills the whole form (e.g. after a config change) */
//     void refill(); // TODO syncronize with optionsDetailed
//
    void setCurrentProfile( const QString& profile );
    void setCurrentFormat( const QString& format );
    void setCurrentOutputDirectory( const QString& directory );
    void setCurrentOutputDirectoryMode( int mode );
    void setReplayGainEnabled( bool enabled, const QString& toolTip = "" );
    void setReplayGainChecked( bool enabled );
//     void setBpmChecked( bool enabled ) { return cBpm->setChecked( enabled ); }
    void setCurrentPlugin( CodecPlugin *plugin ) { currentPlugin = plugin; }

    QString currentProfile();
    QString currentFormat();
    bool isReplayGainChecked();
//     bool isBpmChecked() { return cBpm->isChecked(); }

    OutputDirectory *outputDirectory; // this way it's easier to sync simple and detailed options

private:
    KComboBox* cProfile;
    KPushButton* pProfileRemove;
    KPushButton* pProfileInfo;
    KComboBox* cFormat;
    KPushButton* pFormatInfo;

    QCheckBox *cReplayGain;
//     QCheckBox *cBpm;
    QLabel *lEstimSize;

    Config *config;
    CodecPlugin *currentPlugin;

public slots:
    void currentDataRateChanged( int dataRate );
    void updateProfiles();

private slots:
    void showHelp();
    void profileInfo();
    void profileRemove();
    void formatInfo();
    void profileChanged();
//     void formatChanged();
    void outputDirectoryChanged();

    void somethingChanged();

signals:
    void optionsChanged();
    void customProfilesEdited();
};

#endif
