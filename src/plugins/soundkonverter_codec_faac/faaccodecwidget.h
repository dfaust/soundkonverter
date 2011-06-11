
#ifndef FAACCODECWIDGET_H
#define FAACCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QDoubleSpinBox;
class QCheckBox;

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
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    KComboBox *cMode;
    QSlider *sQuality;
    QDoubleSpinBox *dQuality;
    KComboBox *cSamplerate;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( double quality );
    double qualityForBitrate( int bitrate );

private slots:
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( double quality );

signals:
    void somethingChanged();
};

#endif // FAACCODECWIDGET_H
