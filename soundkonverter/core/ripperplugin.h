
#ifndef RIPPERPLUGIN_H
#define RIPPERPLUGIN_H

#include "backendplugin.h"

#include <KUrl>

class RipperPlugin;


/*
struct RipperInfo
{
    RipperPlugin *plugin;
    bool canRipEntireCd;
    bool enabled; // can we use this plugin? (all needed backends installed?)
    int rating;
    QString problemInfo; // howto message, if a backend is missing
};
*/


class KDE_EXPORT RipperPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    RipperPluginItem( QObject *parent=0 );
    virtual ~RipperPluginItem();

    struct Data {
        int fromSector;
        int toSector;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT RipperPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    RipperPlugin( QObject *parent=0 );
    ~RipperPlugin();
    
    virtual QString type();

//     virtual RipperInfo ripperInfo() = 0;
    virtual QList<ConversionPipeTrunk> codecTable() = 0;

    /** converts a file; TODO notify Conversion when the process has been finished */
    virtual int rip( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
    /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
    virtual QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
};

#define K_EXPORT_SOUNDKONVERTER_RIPPER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_ripper_##libname, KGenericFactory<classname>("soundkonverter_ripper_" #libname) )

#endif // RIPPERPLUGIN_H

