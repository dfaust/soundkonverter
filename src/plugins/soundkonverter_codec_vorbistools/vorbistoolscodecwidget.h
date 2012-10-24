
#ifndef VORBISTOOLSCODECWIDGET_H
#define VORBISTOOLSCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QDoubleSpinBox;
class QCheckBox;
// class QLabel;

class VorbisToolsCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    VorbisToolsCodecWidget();
    ~VorbisToolsCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    KComboBox *cMode;
    QSlider *sQuality;
    QDoubleSpinBox *dQuality;
    KComboBox *cBitrateMode;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( double quality );
    double qualityForBitrate( int bitrate );

private slots:
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );
};

#endif // VORBISTOOLSCODECWIDGET_H