
#ifndef SOUNDKONVERTER_CODEC_VORBISTOOLS_H
#define SOUNDKONVERTER_CODEC_VORBISTOOLS_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_vorbistools : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_vorbistools( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_vorbistools();

    QString name();

    QList<ConversionPipeTrunk> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
    QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
    bool isConfigSupported( ActionType action );
    void showConfigDialog( ActionType action, const QString& format, QWidget *parent );
    bool hasInfo();
    void showInfo();
    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );
};

K_EXPORT_SOUNDKONVERTER_CODEC( vorbistools, soundkonverter_codec_vorbistools );


#endif // SOUNDKONVERTER_CODEC_VORBISTOOLS_H


