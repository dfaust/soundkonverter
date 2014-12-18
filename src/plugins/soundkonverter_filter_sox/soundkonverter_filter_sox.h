
#ifndef SOUNDKONVERTER_FILTER_SOX_H
#define SOUNDKONVERTER_FILTER_SOX_H

#include "../../core/filterplugin.h"

#include <QDateTime>
#include <QSet>

class FilterOptions;
class QDialog;
class QComboBox;

class soundkonverter_filter_sox : public FilterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.filterplugin.sox" FILE "soundkonverter_filter_sox.json")
    Q_INTERFACES(FilterPlugin)

public:
    struct SoxCodecData
    {
        QString codecName;
        QString soxCodecName;
        bool external;
        bool experimental;
        bool enabled;
    };

    soundkonverter_filter_sox();
    ~soundkonverter_filter_sox();

    QString name();
    int version();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();
    FilterWidget *newFilterWidget();

    unsigned int convert( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

private:
    QList<SoxCodecData> codecList;
    QProcess* infoProcess;
    QString infoProcessOutputData;

    QDialog* configDialog;
    QComboBox *configDialogSamplingRateQualityComboBox;

    int configVersion;
    QString samplingRateQuality;
    bool experimentalEffectsEnabled;
    QDateTime soxLastModified;
    QSet<QString> soxCodecList;

    QString soxCodecName( const QString& codecName );

private slots:
    void configDialogSave();
    void configDialogDefault();

    void infoProcessOutput();
    void infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );
};

#endif // SOUNDKONVERTER_FILTER_SOX_H
