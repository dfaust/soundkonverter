
#ifndef SOUNDKONVERTER_CODEC_TIMIDITY_H
#define SOUNDKONVERTER_CODEC_TIMIDITY_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_timidity : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_timidity( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_timidity();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& format );
    void showConfigDialog( ActionType action, const QString& format, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );
};

K_EXPORT_SOUNDKONVERTER_CODEC( timidity, soundkonverter_codec_timidity )


#endif // SOUNDKONVERTER_CODEC_TIMIDITY_H


