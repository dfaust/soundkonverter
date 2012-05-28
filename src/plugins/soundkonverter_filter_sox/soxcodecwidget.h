
#ifndef SOXCODECWIDGET_H
#define SOXCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;

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

#endif // SOXCODECWIDGET_H
