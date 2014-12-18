
#ifndef SOUNDKONVERTER_CODEC_FLUIDSYNTH_H
#define SOUNDKONVERTER_CODEC_FLUIDSYNTH_H

#include "../../core/codecplugin.h"

#include <QUrl>

class ConversionOptions;
class QDialog;
class KUrlRequester;

class soundkonverter_codec_fluidsynth : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.fluidsynth" FILE "soundkonverter_codec_fluidsynth.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_fluidsynth();
    ~soundkonverter_codec_fluidsynth();

    QString name();

    QList<ConversionPipeTrunk> codecTable();
    bool isConfigSupported( ActionType action, const QString& format );
    void showConfigDialog( ActionType action, const QString& format, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    CodecWidget *newCodecWidget();

    unsigned int convert( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

private:
    QDialog* configDialog;
    KUrlRequester *configDialogSoundFontUrlRequester;

    QUrl soundFontFile;

private slots:
    void configDialogSave();
};

#endif // SOUNDKONVERTER_CODEC_FLUIDSYNTH_H
