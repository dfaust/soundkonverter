
#ifndef LAMECODECWIDGET_H
#define LAMECODECWIDGET_H

#include "../../core/codecwidget.h"

#include <QGroupBox>

class QComboBox;
class QSpinBox;
class QCheckBox;
class QLabel;
class QSlider;
class QLineEdit;

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
    // preset selection
    QComboBox *cPreset;
    QSpinBox *iPresetBitrate;
    QCheckBox *cPresetBitrateCbr;
    QCheckBox *cPresetFast;
    // user defined options
    QGroupBox *userdefinedBox;
    QComboBox *cMode;
    QSpinBox *iQuality;
    QSlider *sQuality;
    QComboBox *cBitrateMode;
    QSlider *sCompressionLevel;
    QSpinBox *iCompressionLevel;
    QCheckBox *cCmdArguments;
    QLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

    int bitrateForQuality( int quality );
    int qualityForBitrate( int bitrate );

private slots:
    // presets
    void presetChanged( const QString& preset );
    void presetBitrateChanged( int bitrate );
    // user defined options
    void modeChanged( int mode );
    void qualitySliderChanged( int quality );
    void qualitySpinBoxChanged( int quality );
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );
};

#endif // LAMECODECWIDGET_H