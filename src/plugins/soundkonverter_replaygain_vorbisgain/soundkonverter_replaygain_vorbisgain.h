
#ifndef SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_replaygain_vorbisgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_vorbisgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_replaygain_vorbisgain();

    QString name();

//     QMap<QString,int> codecList();
    QList<ReplayGainPipe> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
//     QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
//     bool canApply( const KUrl& filename );
    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );
//     QString applyCommand( const KUrl::List& fileList, ApplyMode mode = Add );
};

// K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_vorbisgain, KGenericFactory<soundkonverter_replaygain_vorbisgain> );
K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( vorbisgain, soundkonverter_replaygain_vorbisgain );


#endif // _SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H_


