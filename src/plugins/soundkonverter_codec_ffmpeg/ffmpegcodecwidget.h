
#ifndef FFMPEGCODECWIDGET_H
#define FFMPEGCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;
class QCheckBox;
class KLineEdit;

class FFmpegCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    FFmpegCodecWidget();
    ~FFmpegCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    QSlider *sBitrate;
    QSpinBox *iBitrate;
    KComboBox *cBitrate;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

private slots:
    void qualitySliderChanged( int bitrate );
    void qualitySpinBoxChanged( int bitrate );

signals:
    void somethingChanged();
};

#endif // FFMPEGCODECWIDGET_H