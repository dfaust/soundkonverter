
#ifndef SOUNDKONVERTER_CODEC_OPUSTOOLS_H
#define SOUNDKONVERTER_CODEC_OPUSTOOLS_H

#include "../../core/codecplugin.h"

class ConversionOptions;
class QDialog;
class QCheckBox;

class soundkonverter_codec_opustools : public CodecPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.codecplugin.opustools" FILE "soundkonverter_codec_opustools.json")
    Q_INTERFACES(CodecPlugin)

public:
    soundkonverter_codec_opustools();
    ~soundkonverter_codec_opustools();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    CodecWidget *newCodecWidget();

    unsigned int convert( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

private:
    QDialog* configDialog;
    QCheckBox *configDialogUncoupledChannelsCheckBox;

    int configVersion;
    bool uncoupledChannels;

private slots:
    void configDialogSave();
    void configDialogDefault();
};

#endif // SOUNDKONVERTER_CODEC_OPUSTOOLS_H
