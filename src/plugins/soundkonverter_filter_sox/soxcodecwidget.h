
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
