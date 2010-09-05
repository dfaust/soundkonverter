
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
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& data );
    int currentDataRate();

private:
    // user defined options
    KComboBox *cMode;
    QSpinBox *iQuality;
    QSlider *sQuality;
    QCheckBox *chChannels;
    KComboBox *cChannels;
    QCheckBox *chSamplerate;
    KComboBox *cSamplerate;
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

signals:
    void somethingChanged();
};

#endif // TWOLAMECODECWIDGET_H