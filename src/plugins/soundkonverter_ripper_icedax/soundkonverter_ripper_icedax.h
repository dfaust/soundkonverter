
#ifndef SOUNDKONVERTER_RIPPER_ICEDAX_H
#define SOUNDKONVERTER_RIPPER_ICEDAX_H

#include "../../core/ripperplugin.h"

#include <QUrl>
#include <QProcess>
#include <QList>

class soundkonverter_ripper_icedax : public RipperPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.ripperplugin.icedax" FILE "soundkonverter_ripper_icedax.json")
    Q_INTERFACES(RipperPlugin)

public:
    soundkonverter_ripper_icedax();
    ~soundkonverter_ripper_icedax();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int rip( const QString& device, int track, int tracks, const QUrl& outputFile );
    QStringList ripCommand( const QString& device, int track, int tracks, const QUrl& outputFile );
    float parseOutput( const QString& output, RipperPluginItem *ripperItem );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();
};

#endif // SOUNDKONVERTER_RIPPER_ICEDAX_H
