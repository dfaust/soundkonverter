
#ifndef RIPPERPLUGIN_H
#define RIPPERPLUGIN_H

#ifdef SOUNDKONVERTER_KF5_BUILD
    #include <kcoreaddons_export.h>
    #include <QVariantList>
    #define VARG_TYPE QVariantList
#else
    #include <QStringList>
    #define VARG_TYPE QStringList
#endif

#include "backendplugin.h"

#include <KUrl>

class RipperPlugin;


#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT RipperPluginItem : public BackendPluginItem
#else
class KDE_EXPORT RipperPluginItem : public BackendPluginItem
#endif
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
#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT RipperPlugin : public BackendPlugin
#else
class KDE_EXPORT RipperPlugin : public BackendPlugin
#endif
{
    Q_OBJECT
public:
    RipperPlugin( QObject *parent=0 );
    virtual ~RipperPlugin();

    virtual QString type();

    virtual QList<ConversionPipeTrunk> codecTable() = 0;

    /** rips a track */
    virtual unsigned int rip( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
    /** returns a command for ripping a track through a pipe; "" if pipes aren't supported */
    virtual QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile ) = 0;
};

#ifndef SOUNDKONVERTER_KF5_BUILD
#define K_EXPORT_SOUNDKONVERTER_RIPPER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_ripper_##libname, KGenericFactory<classname>("soundkonverter_ripper_" #libname) )
#endif

#endif // RIPPERPLUGIN_H

