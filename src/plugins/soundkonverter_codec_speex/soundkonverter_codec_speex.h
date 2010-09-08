
#ifndef SOUNDKONVERTER_CODEC_SPEEX_H
#define SOUNDKONVERTER_CODEC_SPEEX_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_speex : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_speex( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_speex();

    QString name();

    QList<ConversionPipeTrunk> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
//     QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
    bool isConfigSupported( ActionType action, const QString& format );
    void showConfigDialog( ActionType action, const QString& format, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );
};

K_EXPORT_SOUNDKONVERTER_CODEC( speex, soundkonverter_codec_speex );


#endif // SOUNDKONVERTER_CODEC_SPEEX_H


