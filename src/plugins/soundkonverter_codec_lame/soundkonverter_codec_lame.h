
#ifndef SOUNDKONVERTER_CODEC_LAME_H
#define SOUNDKONVERTER_CODEC_LAME_H

#include "../../core/codecplugin.h"

// #include <QWeakPointer>
#include <QUrl>

class ConversionOptions;
class QDialog;
class QComboBox;


class soundkonverter_codec_lame : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_lame( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_lame();

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
    QComboBox *configDialogStereoModeComboBox;

    int configVersion;
    QString stereoMode;

private slots:
    void configDialogSave();
    void configDialogDefault();

};

K_EXPORT_SOUNDKONVERTER_CODEC( lame, soundkonverter_codec_lame )


#endif // _SOUNDKONVERTER_CODEC_LAME_H_


