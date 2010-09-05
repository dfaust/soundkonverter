
#ifndef WAVPACKCODECWIDGET_H
#define WAVPACKCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QCheckBox;
class KLineEdit;

class WavPackCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    WavPackCodecWidget();
    ~WavPackCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    KComboBox *cCompressionLevel;
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

signals:
    void somethingChanged();
};

#endif // WAVPACKCODECWIDGET_H
