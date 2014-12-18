
#ifndef SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>

class ConversionOptions;

class soundkonverter_replaygain_wvgain : public ReplayGainPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.replaygainplugin.wvgain" FILE "soundkonverter_replaygain_wvgain.json")
    Q_INTERFACES(ReplayGainPlugin)

public:
    soundkonverter_replaygain_wvgain();
    ~soundkonverter_replaygain_wvgain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const QList<QUrl>& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );
};

#endif // _SOUNDKONVERTER_REPLAYGAIN_WVGAIN_H_
