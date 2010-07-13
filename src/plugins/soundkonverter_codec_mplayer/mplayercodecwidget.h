
#ifndef MPLAYERCODECWIDGET_H
#define MPLAYERCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;
class QCheckBox;
// class QLabel;

class MPlayerCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    MPlayerCodecWidget();
    ~MPlayerCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    QSlider *sBitrate;
    QSpinBox *iBitrate;
    QCheckBox *chChannels;
    KComboBox *cChannels;
    QCheckBox *chSamplerate;
    KComboBox *cSamplerate;

    QString currentFormat; // holds the current output file format

private slots:
    void qualitySliderChanged( int bitrate );
    void qualitySpinBoxChanged( int bitrate );
    void channelsToggled( bool enabled );
    void samplerateToggled( bool enabled );

signals:
    void somethingChanged();
};

#endif // MPLAYERCODECWIDGET_H