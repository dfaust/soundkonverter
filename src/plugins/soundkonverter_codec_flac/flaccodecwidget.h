
#ifndef FLACCODECWIDGET_H
#define FLACCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;

class FlacCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    FlacCodecWidget();
    ~FlacCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    QSlider *sCompressionLevel;
    QSpinBox *iCompressionLevel;

    QString currentFormat; // holds the current output file format

private slots:
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );

signals:
    void somethingChanged();
};

#endif // FLACCODECWIDGET_H
