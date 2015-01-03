
#ifndef SOUNDKONVERTER_CODEC_LAME_H
#define SOUNDKONVERTER_CODEC_LAME_H

#include "../../core/codecplugin.h"

#include <QUrl>
#include <QPointer>

class ConversionOptions;
class QDialog;
class QComboBox;

namespace Ui {
    class LameConfigDialog;
}

class soundkonverter_codec_lame : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.lame" FILE "soundkonverter_codec_lame.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_lame();
    ~soundkonverter_codec_lame();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    CodecWidget *newCodecWidget();

    unsigned int convert( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

private:
    QPointer<QDialog> configDialog;
    Ui::LameConfigDialog *configDialogUi;

    int configVersion;
    QString stereoMode;

private slots:
    void configDialogSave();
    void configDialogDefault();
};

#endif // _SOUNDKONVERTER_CODEC_LAME_H_
