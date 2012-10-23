
#ifndef FILTERPLUGIN_H
#define FILTERPLUGIN_H

#include "codecplugin.h"
#include "conversionoptions.h"

class QWidget;
class FilterPlugin;
class FilterWidget;
class TagData;


// struct FilterPipe
// {
//     FilterPlugin *plugin;
//     bool enabled; // can we use this conversion pipe? (all needed backends installed?)
//     QString problemInfo; // howto message, if a backend is missing
// };


class KDE_EXPORT FilterPluginItem : public CodecPluginItem
{
    Q_OBJECT
public:
    FilterPluginItem( QObject *parent=0 );
    virtual ~FilterPluginItem();
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT FilterPlugin : public CodecPlugin
{
    Q_OBJECT
public:
    FilterPlugin( QObject *parent=0 );
    virtual ~FilterPlugin();

    virtual QString type();

    virtual FilterWidget *newFilterWidget() = 0;
    virtual FilterWidget *deleteFilterWidget( FilterWidget *filterWidget );

//     /**
//      * starts the conversion and returns either a conversion id or an error code:
//      *
//      * -1   unknown error
//      * -100 plugin not configured
//      */
//     virtual int filter( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions ) = 0;
//     /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
//     virtual QStringList filterCommand( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions ) = 0;

    virtual FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

protected:
    FilterOptions *lastUsedFilterOptions;

};

#define K_EXPORT_SOUNDKONVERTER_FILTER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_filter_##libname, KGenericFactory<classname>("soundkonverter_filter_" #libname) )

#endif // FILTERPLUGIN_H

