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


#ifndef SOXCODECWIDGET_H
#define SOXCODECWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;
class KComboBox;
class KLineEdit;


class SoxCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    SoxCodecWidget();
    ~SoxCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    // flac
    QLabel         *lCompressionLevel;
    QSlider        *sCompressionLevel;
    QSpinBox       *iCompressionLevel;
    // mp2, mp3 and ogg vorbis
    QLabel         *lMode;
    KComboBox      *cMode;
    QLabel         *lQuality;
    QSlider        *sQuality;
    QDoubleSpinBox *dQuality;
    // amr nb and amr wb
    QLabel         *lBitratePreset;
    KComboBox      *cBitratePreset;

    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

private slots:
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );

    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );
};

#endif // SOXCODECWIDGET_H
