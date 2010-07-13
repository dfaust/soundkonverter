
#ifndef REPLAYGAINPLUGIN_H
#define REPLAYGAINPLUGIN_H

#include "backendplugin.h"

// #include <QMultiMap> // inherits QMap
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
  
//     ReplayGainPlugin::ApplyMode mode;
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
    ~ReplayGainPlugin();
    
    virtual QString type();

//     virtual QMap<QString,int> codecList() = 0; // TODO rating, etc.
    virtual QList<ReplayGainPipe> codecTable() = 0;
//     virtual bool canApply( const KUrl& filename ) = 0;
//     virtual bool isConfigSupported() = 0;
//     virtual bool showConfigDialog( const QString& format, QWidget *parent ) = 0;

    /** converts a file; TODO notify Conversion when the process has been finished */
    virtual int apply( const KUrl::List& fileList, ApplyMode mode = Add ) = 0;
    /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
//     virtual QString applyCommand( const KUrl::List& fileList, ApplyMode mode = Add ) = 0;
//     virtual bool pipesSupported() = 0;

protected:
    int lastId;
};

#define K_EXPORT_SOUNDKONVERTER_REPLAYGAIN(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_##libname, KGenericFactory<classname>("soundkonverter_replaygain_" #libname) )

#endif // REPLAYGAINPLUGIN_H

