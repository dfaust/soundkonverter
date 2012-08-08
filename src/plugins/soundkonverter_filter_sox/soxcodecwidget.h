
#ifndef SOXCODECWIDGET_H
#define SOXCODECWIDGET_H

#include "../../core/codecwidget.h"

// #include <QWeakPointer>

class KComboBox;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;

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
//     // flac
//     QWeakPointer<QLabel>         lCompressionLevel;
//     QWeakPointer<QSlider>        sCompressionLevel;
//     QWeakPointer<QSpinBox>       iCompressionLevel;
//     // mp3 and ogg vorbis
//     QWeakPointer<QLabel>         lMode;
//     QWeakPointer<KComboBox>      cMode;
//     QWeakPointer<QSlider>        sQuality;
//     QWeakPointer<QDoubleSpinBox> dQuality;
//     // amr nb and amr wb
//     QWeakPointer<QLabel>         lBitratePreset;
//     QWeakPointer<KComboBox>      cBitratePreset;

    // flac
    QLabel         *lCompressionLevel;
    QSlider        *sCompressionLevel;
    QSpinBox       *iCompressionLevel;
    // mp3 and ogg vorbis
    QLabel         *lMode;
    KComboBox      *cMode;
    QLabel         *lQuality;
    QSlider        *sQuality;
    QDoubleSpinBox *dQuality;
    // amr nb and amr wb
    QLabel         *lBitratePreset;
    KComboBox      *cBitratePreset;

    QString currentFormat; // holds the current output file format

private slots:
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );

    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );

signals:
    void somethingChanged();
};

#endif // SOXCODECWIDGET_H
