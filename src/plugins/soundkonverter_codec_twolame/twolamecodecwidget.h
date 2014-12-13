
#ifndef TWOLAMECODECWIDGET_H
#define TWOLAMECODECWIDGET_H

#include "../../core/codecwidget.h"

#include <QGroupBox>

class QComboBox;
class QSpinBox;
class QCheckBox;
class QLabel;
class QSlider;
class QLineEdit;

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
    QComboBox *cMode;
    QSpinBox *iQuality;
    QSlider *sQuality;
    QCheckBox *cCmdArguments;
    QLineEdit *lCmdArguments;

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