
#ifndef SOUNDKONVERTER_CODEC_TTAENC_H
#define SOUNDKONVERTER_CODEC_TTAENC_H

#include "../../core/codecplugin.h"

class ConversionOptions;

class soundkonverter_codec_ttaenc : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.ttaenc" FILE "soundkonverter_codec_ttaenc.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_ttaenc();
    ~soundkonverter_codec_ttaenc();

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
};

#endif // _SOUNDKONVERTER_CODEC_TTAENC_H_
