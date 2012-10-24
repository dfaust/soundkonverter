
#ifndef OPUSTOOLSCODECWIDGET_H
#define OPUSTOOLSCODECWIDGET_H

#include "../../core/codecwidget.h"

class QSlider;
class QDoubleSpinBox;
class KComboBox;

class OpusToolsCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    OpusToolsCodecWidget();
    ~OpusToolsCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    QSlider *sQuality;
    QDoubleSpinBox *dQuality;
    KComboBox *cBitrateMode;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( double quality );
    double qualityForBitrate( int bitrate );

private slots:
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );
};

#endif // OPUSTOOLSCODECWIDGET_H