
#ifndef SOUNDKONVERTER_REPLAYGAIN_METAFLAC_H
#define SOUNDKONVERTER_REPLAYGAIN_METAFLAC_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_replaygain_metaflac : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_metaflac( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_replaygain_metaflac();

    QString name();

//     QMap<QString,int> codecList();
    QList<ReplayGainPipe> codecTable();
    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
//     bool canApply( const KUrl& filename );
    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );
//     QString applyCommand( const KUrl::List& fileList, ApplyMode mode = Add );
};

// K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_metaflac, KGenericFactory<soundkonverter_replaygain_metaflac> );
K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( metaflac, soundkonverter_replaygain_metaflac );


#endif // _SOUNDKONVERTER_REPLAYGAIN_METAFLAC_H_


