
#ifndef FLAKECODECWIDGET_H
#define FLAKECODECWIDGET_H

#include "../../core/codecwidget.h"

class QComboBox;
class QSlider;
class QSpinBox;
class QCheckBox;
class QLineEdit;

class FlakeCodecWidget : public CodecWidget
{
    Q_OBJECT
public:
    FlakeCodecWidget();
    ~FlakeCodecWidget();

    ConversionOptions *currentConversionOptions();
    bool setCurrentConversionOptions( ConversionOptions *_options );
    void setCurrentFormat( const QString& format );
    QString currentProfile();
    bool setCurrentProfile( const QString& profile );
    int currentDataRate();

private:
    QSlider *sCompressionLevel;
    QSpinBox *iCompressionLevel;
    QCheckBox *cCmdArguments;
    QLineEdit *lCmdArguments;

    QString currentFormat; // holds the current output file format

private slots:
    void compressionLevelSliderChanged( int quality );
    void compressionLevelSpinBoxChanged( int quality );
};

#endif // FLAKECODECWIDGET_H
