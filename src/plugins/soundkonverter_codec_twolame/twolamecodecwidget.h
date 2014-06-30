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


#ifndef TWOLAMECODECWIDGET_H
#define TWOLAMECODECWIDGET_H

#include "../../core/codecwidget.h"

#include <QGroupBox>

class KComboBox;
class QSpinBox;
class QCheckBox;
class QLabel;
class QSlider;
class KLineEdit;

class TwoLameCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    TwoLameCodecWidget();
    ~TwoLameCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    // user defined options
    KComboBox *cMode;
    QSpinBox *iQuality;
    QSlider *sQuality;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( int quality );
    int qualityForBitrate( int bitrate );

private slots:
    // user defined options
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( int quality );
};

#endif // TWOLAMECODECWIDGET_H