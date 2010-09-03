
#ifndef SOUNDKONVERTER_RIPPER_CDPARANOIA_H
#define SOUNDKONVERTER_RIPPER_CDPARANOIA_H

#include "../../core/ripperplugin.h"

#include <KUrl>
#include <KProcess>
#include <QList>


class soundkonverter_ripper_cdparanoia : public RipperPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_ripper_cdparanoia( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_ripper_cdparanoia();

    QString name();
    
//     RipperInfo ripperInfo();
    QList<ConversionPipeTrunk> codecTable();

    BackendPlugin::FormatInfo formatInfo( const QString& codecName );
    QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
    
    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int rip( const QString& device, int track, int tracks, const KUrl& outputFile );
    QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile );
    float parseOutput( const QString& output, int *fromSector, int *toSector );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();
};

K_EXPORT_SOUNDKONVERTER_RIPPER( cdparanoia, soundkonverter_ripper_cdparanoia );


#endif // SOUNDKONVERTER_RIPPER_CDPARANOIA_H


