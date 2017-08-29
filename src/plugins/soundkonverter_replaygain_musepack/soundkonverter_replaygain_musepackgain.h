
#ifndef SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

class ConversionOptions;


class soundkonverter_replaygain_musepackgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_musepackgain( QObject *parent, const QVariantList& args );

    /** Default Destructor */
    ~soundkonverter_replaygain_musepackgain();

    QString name() const;

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

};

#endif // _SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H_
