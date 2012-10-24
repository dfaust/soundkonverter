
#ifndef FLUIDSYNTHCODECWIDGET_H
#define FLUIDSYNTHCODECWIDGET_H

#include "../../core/codecwidget.h"

class FluidsynthCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    FluidsynthCodecWidget();
    ~FluidsynthCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    QString currentFormat; // holds the current output file format
};

#endif // FLUIDSYNTHCODECWIDGET_H
