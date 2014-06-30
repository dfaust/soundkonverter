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


#ifndef MUSEPACKCODECWIDGET_H
#define MUSEPACKCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class KLineEdit;
class QSlider;
class QDoubleSpinBox;
class QGroupBox;
class QCheckBox;

class MusePackCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    MusePackCodecWidget();
    ~MusePackCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    // preset selection
    KComboBox *cPreset;
    // user defined options
    QGroupBox *userdefinedBox;
    QSlider *sQuality;
    QDoubleSpinBox *dQuality;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( double quality );
    double qualityForBitrate( int bitrate );

private slots:
    // presets
    void presetChanged( const QString& preset );
    // user defined options
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );
};

#endif // MUSEPACKCODECWIDGET_H