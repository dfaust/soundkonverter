
#ifndef SOUNDKONVERTER_CODEC_TWOLAME_H
#define SOUNDKONVERTER_CODEC_TWOLAME_H

#include "../../core/codecplugin.h"

#include <QUrl>

class ConversionOptions;

class soundkonverter_codec_twolame : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.twolame" FILE "soundkonverter_codec_twolame.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_twolame();
    ~soundkonverter_codec_twolame();

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

#endif // _SOUNDKONVERTER_CODEC_TWOLAME_H_
