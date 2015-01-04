
#ifndef LAMECODECWIDGET_H
#define LAMECODECWIDGET_H

#include "ui_lamecodecwidget.h"
#include "../../core/codecwidget.h"

#include <QGroupBox>

class LameCodecWidget : public CodecWidget
{
    Q_OBJECT

public:
    LameCodecWidget();
    ~LameCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    Ui::LameCodecWidget ui;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality(int quality);
    int qualityForBitrate(int bitrate);

private slots:
    // presets
    void presetChanged(const QString& preset);
    void presetBitrateChanged(int bitrate);
    // user defined options
    void modeChanged(int mode);
    void qualitySliderChanged(int quality);
    void qualitySpinBoxChanged(int quality);
    void showManpage();
};

#endif // LAMECODECWIDGET_H
