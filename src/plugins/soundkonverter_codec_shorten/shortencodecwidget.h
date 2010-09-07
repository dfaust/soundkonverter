
#ifndef SHORTENCODECWIDGET_H
#define SHORTENCODECWIDGET_H

#include "../../core/codecwidget.h"

class KComboBox;
class QSlider;
class QSpinBox;
class QCheckBox;
class KLineEdit;

class ShortenCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    ShortenCodecWidget();
    ~ShortenCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    QDomDocument customProfile();
    bool setCustomProfile( const QString& profile, const QDomDocument& document );
    int currentDataRate();

private:
    QCheckBox *cCmdArguments;
    KLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

signals:
    void somethingChanged();
};

#endif // SHORTENCODECWIDGET_H
