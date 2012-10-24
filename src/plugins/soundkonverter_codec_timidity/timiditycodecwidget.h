
#ifndef TIMIDITYCODECWIDGET_H
#define TIMIDITYCODECWIDGET_H

#include "../../core/codecwidget.h"

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
    int currentDataRate();

private:
    QString currentFormat; // holds the current output file format
};

#endif // TIMIDITYCODECWIDGET_H
