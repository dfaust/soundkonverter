
#ifndef FLACCODECWIDGET_H
#define FLACCODECWIDGET_H

#include "../../core/codecwidget.h"

class QCheckBox;
class KLineEdit;

class TTAEncCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    TTAEncCodecWidget();
    ~TTAEncCodecWidget();

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

#endif // FLACCODECWIDGET_H
