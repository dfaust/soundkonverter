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


#ifndef OPTIONSDETAILED_H
#define OPTIONSDETAILED_H

#include <QCheckBox>
#include <QDomDocument>
#include <QMap>
#include <QWidget>

class CodecPlugin;
class Config;
class ConversionOptions;
class FilterPlugin;
class OutputDirectory;
class CodecWidget;
class FilterWidget;

class KComboBox;
class KPushButton;
class QGridLayout;
class QLabel;
class QToolButton;


/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class OptionsDetailed : public QWidget
{
    Q_OBJECT
public:
    OptionsDetailed( Config *_config, QWidget *parent=0 );
    ~OptionsDetailed();

    void init();

    /** Return the current options */
    ConversionOptions *currentConversionOptions( bool saveLastUsed = true );
    /** Set the current options */
    bool setCurrentConversionOptions( ConversionOptions *options );

    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QString currentFormat();
    void setCurrentFormat( const QString& format );
    void resetFilterOptions();
    void setReplayGainChecked( bool enabled );

    bool isReplayGainEnabled( QString *toolTip = 0 );
    bool isReplayGainChecked();
    CodecPlugin *getCurrentPlugin();

    OutputDirectory *outputDirectory; // this way it's easier to sync simple and detailed options

//     QSize sizeHint();

private:
    QGridLayout *grid;
    KComboBox *cFormat;
    QLabel *lPlugin;
    KComboBox *cPlugin;
    KPushButton *pConfigurePlugin;
    CodecWidget *wPlugin;
    QMap<FilterWidget*,FilterPlugin*> wFilter;
    QCheckBox *cReplayGain;
    QLabel *lEstimSize;
    KPushButton *pProfileSave;
    QToolButton *pProfileLoad;

    Config *config;

    CodecPlugin *currentPlugin;

//     QSize size_hint;

public slots:
    void somethingChanged();
    void loadCustomProfileButtonClicked();
    bool saveCustomProfile( bool lastUsed = false );
    void updateProfiles();

private slots:
    void formatChanged( const QString& format );
    void encoderChanged( const QString& encoder );
    void configurePlugin();

signals:
    // connected to optionsSimple
    void currentDataRateChanged( int rate );
//     void optionsChanged();
    void customProfilesEdited();
};

#endif
