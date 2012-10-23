
#ifndef SOUNDKONVERTER_CODEC_OPUSTOOLS_H
#define SOUNDKONVERTER_CODEC_OPUSTOOLS_H

#include "../../core/codecplugin.h"

#include <QWeakPointer>

class ConversionOptions;
class KDialog;
class QCheckBox;


class soundkonverter_codec_opustools : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_opustools( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_opustools();

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

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

private:
    QWeakPointer<KDialog> configDialog;
    QCheckBox *configDialogUncoupledChannelsCheckBox;

    int configVersion;
    bool uncoupledChannels;

private slots:
    void configDialogSave();
    void configDialogDefault();

};

K_EXPORT_SOUNDKONVERTER_CODEC( opustools, soundkonverter_codec_opustools )


#endif // SOUNDKONVERTER_CODEC_OPUSTOOLS_H


