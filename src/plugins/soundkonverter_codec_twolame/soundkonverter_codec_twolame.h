
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
    ~soundkonverter_codec_twolame();

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

K_EXPORT_SOUNDKONVERTER_CODEC( twolame, soundkonverter_codec_twolame )


#endif // _SOUNDKONVERTER_CODEC_TWOLAME_H_


