
#ifndef SOUNDKONVERTER_CODEC_TWOLAME_H
#define SOUNDKONVERTER_CODEC_TWOLAME_H

#include "../../core/codecplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_codec_twolame : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_twolame( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_twolame();

    QString name();

    QList<ConversionPipeTrunk> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
//     QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );
    
    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions );
};

K_EXPORT_SOUNDKONVERTER_CODEC( twolame, soundkonverter_codec_twolame );


#endif // _SOUNDKONVERTER_CODEC_TWOLAME_H_


