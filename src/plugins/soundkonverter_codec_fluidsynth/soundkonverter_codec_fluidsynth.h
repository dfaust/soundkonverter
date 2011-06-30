
#ifndef SOUNDKONVERTER_CODEC_FLUIDSYNTH_H
#define SOUNDKONVERTER_CODEC_FLUIDSYNTH_H

#include "../../core/codecplugin.h"

#include <KUrl>
#include <QWeakPointer>

class ConversionOptions;
class KDialog;
class KUrlRequester;


class soundkonverter_codec_fluidsynth : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_fluidsynth( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_codec_fluidsynth();

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

private:
    QWeakPointer<KDialog> configDialog;
    KUrlRequester *configDialogSoundFontUrlRequester;

    KUrl soundFontFile;

private slots:
    void configDialogSave();

};

K_EXPORT_SOUNDKONVERTER_CODEC( fluidsynth, soundkonverter_codec_fluidsynth );


#endif // SOUNDKONVERTER_CODEC_FLUIDSYNTH_H


