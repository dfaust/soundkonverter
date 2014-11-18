
#ifndef REPLAYGAINPLUGIN_H
#define REPLAYGAINPLUGIN_H

#include "backendplugin.h"

#include <QMap>
#include <KUrl>

class ReplayGainPlugin;


struct ReplayGainPipe
{
    QString codecName;

    ReplayGainPlugin *plugin;
    bool enabled; // can we use this conversion pipe? (all needed backends installed?)
    int rating;
    QString problemInfo; // howto message, if a backend is missing
};


class KDE_EXPORT ReplayGainPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    ReplayGainPluginItem( QObject *parent=0 );
    virtual ~ReplayGainPluginItem();

    struct Data // additional data
    {
        int fileCount;
        int processedFiles;
        float lastFileProgress;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT ReplayGainPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    enum ApplyMode {
        Add = 0,
        Remove = 1,
        Force = 2
    };

    ReplayGainPlugin( QObject *parent=0 );
    virtual ~ReplayGainPlugin();

    virtual QString type();

    virtual QList<ReplayGainPipe> codecTable() = 0;

    /** adds replaygain to one or more files */
    virtual unsigned int apply( const KUrl::List& fileList, ApplyMode mode = Add ) = 0;
};

#define K_EXPORT_SOUNDKONVERTER_REPLAYGAIN(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_##libname, KGenericFactory<classname>("soundkonverter_replaygain_" #libname) )

#endif // REPLAYGAINPLUGIN_H

