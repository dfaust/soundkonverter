
#ifndef SOUNDKONVERTER_CODEC_FFMPEG_H
#define SOUNDKONVERTER_CODEC_FFMPEG_H

#include "../../core/codecplugin.h"

// #include <QWeakPointer>
#include <QDateTime>

class ConversionOptions;
class QDialog;
class QCheckBox;


class soundkonverter_codec_ffmpeg : public CodecPlugin
{
    Q_OBJECT
public:
    struct FFmpegCodecData
    {
        QString name;
        bool external;
        bool experimental;
    };

    struct CodecData
    {
        QString codecName;
        QList<FFmpegCodecData> ffmpegCodecList;
        FFmpegCodecData currentFFmpegCodec;
    };

    /** Default Constructor */
    soundkonverter_codec_ffmpeg( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_ffmpeg();

    QString name();
    int version();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();

    unsigned int convert( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output, int *length );
    float parseOutput( const QString& output );

private:
    QList<CodecData> codecList;
    QProcess* infoProcess;
    QString infoProcessOutputData;

    QDialog* configDialog;
    QCheckBox *configDialogExperimantalCodecsEnabledCheckBox;

    int configVersion;
    bool experimentalCodecsEnabled;
    int ffmpegVersionMajor;
    int ffmpegVersionMinor;
    QDateTime ffmpegLastModified;
    QSet<QString> ffmpegCodecList;

private slots:
    /** Get the process' output */
    void processOutput();

    void configDialogSave();
    void configDialogDefault();

    void infoProcessOutput();
    void infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );
};

K_EXPORT_SOUNDKONVERTER_CODEC( ffmpeg, soundkonverter_codec_ffmpeg )


#endif // _SOUNDKONVERTER_CODEC_FFMPEG_H_


