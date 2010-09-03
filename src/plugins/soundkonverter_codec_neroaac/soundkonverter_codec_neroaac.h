
#ifndef SOUNDKONVERTER_CODEC_NEROAAC_H
#define SOUNDKONVERTER_CODEC_NEROAAC_H

#include "../../core/codecplugin.h"

class ConversionOptions;


class soundkonverter_codec_neroaac : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_neroaac( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_neroaac();

    QString name();

    QList<ConversionPipeTrunk> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
    
    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    
    QWidget *newCodecWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output, int length );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();
};

K_EXPORT_SOUNDKONVERTER_CODEC( neroaac, soundkonverter_codec_neroaac );


#endif // _SOUNDKONVERTER_CODEC_NEROAAC_H_


