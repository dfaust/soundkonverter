
#ifndef SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>

class ConversionOptions;

class soundkonverter_replaygain_musepackgain : public ReplayGainPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.replaygainplugin.musepackgain" FILE "soundkonverter_replaygain_musepackgain.json")
    Q_INTERFACES(ReplayGainPlugin)

public:
    soundkonverter_replaygain_musepackgain();
    ~soundkonverter_replaygain_musepackgain();

    QString name();

    /** search for the backend binaries in the given directories */
    virtual void scanForBackends( const QStringList& directoryList = QStringList() );

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const QList<QUrl>& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );
};

#endif // _SOUNDKONVERTER_REPLAYGAIN_MUSEPACKGAIN_H_
