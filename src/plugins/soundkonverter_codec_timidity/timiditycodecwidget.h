
#ifndef TIMIDITYCODECWIDGET_H
#define TIMIDITYCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;

class TimidityCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    TimidityCodecWidget();
    ~TimidityCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    QString currentFormat; // holds the current output file format

signals:
    void somethingChanged();
};

#endif // TIMIDITYCODECWIDGET_H
