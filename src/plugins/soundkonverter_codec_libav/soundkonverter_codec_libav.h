
#ifndef SOUNDKONVERTER_CODEC_FFMPEG_H
#define SOUNDKONVERTER_CODEC_FFMPEG_H

#include "../../core/codecplugin.h"

#include <QWeakPointer>
#include <QDateTime>

class ConversionOptions;
class KDialog;
class QCheckBox;


class soundkonverter_codec_libav : public CodecPlugin
{
    Q_OBJECT
public:
    struct LibavCodecData
    {
        QString name;
        bool external;
        bool experimental;
    };

    struct CodecData
    {
        QString codecName;
        QList<LibavCodecData> libavCodecList;
        LibavCodecData currentLibavCodec;
    };

    /** Default Constructor */
    soundkonverter_codec_libav( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_libav();

    QString name();
    int version();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();

    unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output, int *length );
    float parseOutput( const QString& output );

private:
    QList<CodecData> codecList;
    QWeakPointer<KProcess> infoProcess;
    QString infoProcessOutputData;

    QWeakPointer<KDialog> configDialog;
    QCheckBox *configDialogExperimantalCodecsEnabledCheckBox;

    int configVersion;
    bool experimentalCodecsEnabled;
    int libavVersionMajor;
    int libavVersionMinor;
    QDateTime libavLastModified;
    QSet<QString> libavCodecList;

private slots:
    /** Get the process' output */
    void processOutput();

    void configDialogSave();
    void configDialogDefault();

    void infoProcessOutput();
    void infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );
};

K_EXPORT_SOUNDKONVERTER_CODEC( libav, soundkonverter_codec_libav )


#endif // _SOUNDKONVERTER_CODEC_FFMPEG_H_


