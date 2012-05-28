
#ifndef SOUNDKONVERTER_CODEC_TTAENC_H
#define SOUNDKONVERTER_CODEC_TTAENC_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_ttaenc : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_ttaenc( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_ttaenc();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions )
    {
        return CodecPlugin::conversionOptionsFromXmlDefault( conversionOptions );
    }
};

K_EXPORT_SOUNDKONVERTER_CODEC( ttaenc, soundkonverter_codec_ttaenc )


#endif // _SOUNDKONVERTER_CODEC_TTAENC_H_


