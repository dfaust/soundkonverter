
#ifndef SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>

class ConversionOptions;

class soundkonverter_replaygain_vorbisgain : public ReplayGainPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.replaygainplugin.vorbisgain" FILE "soundkonverter_replaygain_vorbisgain.json")
    Q_INTERFACES(ReplayGainPlugin)

public:
    soundkonverter_replaygain_vorbisgain();
    ~soundkonverter_replaygain_vorbisgain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const QList<QUrl>& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output, ReplayGainPluginItem *replayGainItem );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();
};

#endif // _SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H_
