
#ifndef SOUNDKONVERTER_FILTER_SOX_H
#define SOUNDKONVERTER_FILTER_SOX_H

#include "../../core/filterplugin.h"

#include <QWeakPointer>

class FilterOptions;
class KDialog;
class KComboBox;


class soundkonverter_filter_sox : public FilterPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_filter_sox( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_filter_sox();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();
    FilterWidget *newFilterWidget();

    int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 )
    {
        return CodecPlugin::conversionOptionsFromXmlDefault( conversionOptions, filterOptionsElements );
    }

    FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

private:
    QWeakPointer<KDialog> configDialog;
    KComboBox *configDialogSamplingRateQualityComboBox;

    int configVersion;
    int samplingRateQuality;

private slots:
    void configDialogSave();
    void configDialogDefault();

};

K_EXPORT_SOUNDKONVERTER_FILTER( sox, soundkonverter_filter_sox )


#endif // SOUNDKONVERTER_FILTER_SOX_H


