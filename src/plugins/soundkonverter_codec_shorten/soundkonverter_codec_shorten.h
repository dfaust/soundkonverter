
#ifndef SOUNDKONVERTER_CODEC_SHORTEN_H
#define SOUNDKONVERTER_CODEC_SHORTEN_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_shorten : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_shorten( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_shorten();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();

    unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );
};

K_EXPORT_SOUNDKONVERTER_CODEC( shorten, soundkonverter_codec_shorten )


#endif // _SOUNDKONVERTER_CODEC_SHORTEN_H_


