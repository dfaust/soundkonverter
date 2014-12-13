
#ifndef SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>

class ConversionOptions;


class soundkonverter_replaygain_vorbisgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_vorbisgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
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

K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( vorbisgain, soundkonverter_replaygain_vorbisgain )


#endif // _SOUNDKONVERTER_REPLAYGAIN_VORBISGAIN_H_


