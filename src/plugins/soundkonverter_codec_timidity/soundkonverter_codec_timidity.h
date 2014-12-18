
#ifndef SOUNDKONVERTER_CODEC_TIMIDITY_H
#define SOUNDKONVERTER_CODEC_TIMIDITY_H

#include "../../core/codecplugin.h"

class ConversionOptions;

class soundkonverter_codec_timidity : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.timidity" FILE "soundkonverter_codec_timidity.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_timidity();
    ~soundkonverter_codec_timidity();

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
};

#endif // SOUNDKONVERTER_CODEC_TIMIDITY_H
