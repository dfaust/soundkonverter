
#ifndef FFMPEGCODECWIDGET_H
#define FFMPEGCODECWIDGET_H

#include "../../core/codecwidget.h"

class QLabel;
class QSlider;
class QSpinBox;
class QCheckBox;
class KComboBox;
class KLineEdit;

class LibavCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    LibavCodecWidget();
    ~LibavCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    QLabel *lBitrate;
    QSlider *sBitrate;
    QSpinBox *iBitrate;
    KComboBox *cBitrate;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

private slots:
    void qualitySliderChanged( int bitrate );
    void qualitySpinBoxChanged( int bitrate );
};

#endif // FFMPEGCODECWIDGET_H