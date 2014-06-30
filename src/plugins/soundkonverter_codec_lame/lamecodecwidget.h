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


#ifndef LAMECODECWIDGET_H
#define LAMECODECWIDGET_H

#include "../../core/codecwidget.h"

#include <QGroupBox>

class KComboBox;
class QSpinBox;
class QCheckBox;
class QLabel;
class QSlider;
class KLineEdit;

class LameCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    LameCodecWidget();
    ~LameCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    // preset selection
    KComboBox *cPreset;
    QSpinBox *iPresetBitrate;
    QCheckBox *cPresetBitrateCbr;
    QCheckBox *cPresetFast;
    // user defined options
    QGroupBox *userdefinedBox;
    KComboBox *cMode;
    QSpinBox *iQuality;
    QSlider *sQuality;
    KComboBox *cBitrateMode;
    QSlider *sCompressionLevel;
    QSpinBox *iCompressionLevel;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( int quality );
    int qualityForBitrate( int bitrate );

private slots:
    // presets
    void presetChanged( const QString& preset );
    void presetBitrateChanged( int bitrate );
    // user defined options
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( int quality );
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );
};

#endif // LAMECODECWIDGET_H