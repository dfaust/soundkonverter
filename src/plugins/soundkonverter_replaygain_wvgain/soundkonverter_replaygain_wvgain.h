
#ifndef SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_replaygain_wvgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_wvgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_replaygain_wvgain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

};

K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( wvgain, soundkonverter_replaygain_wvgain )


#endif // _SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H_


