
#ifndef SOUNDKONVERTER_FILTER_SOX_H
#define SOUNDKONVERTER_FILTER_SOX_H

#include "../../core/filterplugin.h"

#include <QWeakPointer>
#include <QDateTime>

class FilterOptions;
class KDialog;
class KComboBox;


class soundkonverter_filter_sox : public FilterPlugin
{
    Q_OBJECT
public:
    struct SoxCodecData
    {
        QString codecName;
        QString soxCodecName;
        bool external;
        bool experimental;
        bool enabled;
    };

    /** Default Constructor */
    soundkonverter_filter_sox( QObject *parent, const QStringList& args );

    /** Default Destructor */
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

    unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

private:
    QList<SoxCodecData> codecList;
    QWeakPointer<KProcess> infoProcess;
    QString infoProcessOutputData;

    QWeakPointer<KDialog> configDialog;
    KComboBox *configDialogSamplingRateQualityComboBox;

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

K_EXPORT_SOUNDKONVERTER_FILTER( sox, soundkonverter_filter_sox )


#endif // SOUNDKONVERTER_FILTER_SOX_H


