
#ifndef FAACCODECWIDGET_H
#define FAACCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;

class FaacCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    FaacCodecWidget();
    ~FaacCodecWidget();

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

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( int quality );
    int qualityForBitrate( int bitrate );

private slots:
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( int quality );
};

#endif // FAACCODECWIDGET_H
