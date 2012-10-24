
#ifndef AFTENCODECWIDGET_H
#define AFTENCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;

class AftenCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    AftenCodecWidget();
    ~AftenCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    KComboBox *cMode;
    QSlider *sQuality;
    QSpinBox *dQuality;
    KComboBox *cBitrate;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( int quality );
    int qualityForBitrate( int bitrate );

private slots:
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( int quality );
};

#endif // AFTENCODECWIDGET_H
