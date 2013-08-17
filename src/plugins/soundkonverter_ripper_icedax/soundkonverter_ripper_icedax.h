
#ifndef SOUNDKONVERTER_RIPPER_ICEDAX_H
#define SOUNDKONVERTER_RIPPER_ICEDAX_H

#include "../../core/ripperplugin.h"

#include <KUrl>
#include <KProcess>
#include <QList>


class soundkonverter_ripper_icedax : public RipperPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_ripper_icedax( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_ripper_icedax();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int rip( const QString& device, int track, int tracks, const KUrl& outputFile );
    QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile );
    float parseOutput( const QString& output, RipperPluginItem *ripperItem );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();
};

K_EXPORT_SOUNDKONVERTER_RIPPER( icedax, soundkonverter_ripper_icedax )


#endif // SOUNDKONVERTER_RIPPER_ICEDAX_H


