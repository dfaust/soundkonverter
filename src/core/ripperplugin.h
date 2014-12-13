
#ifndef RIPPERPLUGIN_H
#define RIPPERPLUGIN_H

#include "backendplugin.h"

#include <QUrl>

class RipperPlugin;


class RipperPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    RipperPluginItem( QObject *parent=0 );
    virtual ~RipperPluginItem();

    struct Data {
        int fromSector; // cd paranoia
        int toSector;

        int fileCount; // icedax
        int processedFiles;
        float lastFileProgress;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class RipperPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    RipperPlugin( QObject *parent=0 );
    virtual ~RipperPlugin();

    virtual QString type();

    virtual QList<ConversionPipeTrunk> codecTable() = 0;

    /** rips a track */
    virtual unsigned int rip( const QString& device, int track, int tracks, const QUrl& outputFile ) = 0;
    /** returns a command for ripping a track through a pipe; "" if pipes aren't supported */
    virtual QStringList ripCommand( const QString& device, int track, int tracks, const QUrl& outputFile ) = 0;
};

#endif // RIPPERPLUGIN_H

