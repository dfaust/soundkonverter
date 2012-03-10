
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

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

};

K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( metaflac, soundkonverter_replaygain_metaflac )


#endif // _SOUNDKONVERTER_REPLAYGAIN_METAFLAC_H_


