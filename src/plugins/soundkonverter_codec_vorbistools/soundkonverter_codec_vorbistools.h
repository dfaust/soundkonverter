
#ifndef SOUNDKONVERTER_CODEC_VORBISTOOLS_H
#define SOUNDKONVERTER_CODEC_VORBISTOOLS_H

#include "../../core/codecplugin.h"

class ConversionOptions;

class soundkonverter_codec_vorbistools : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.vorbistools" FILE "soundkonverter_codec_vorbistools.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_vorbistools();
    ~soundkonverter_codec_vorbistools();

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

#endif // SOUNDKONVERTER_CODEC_VORBISTOOLS_H
