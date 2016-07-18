
#ifndef FILTERPLUGIN_H
#define FILTERPLUGIN_H

#ifdef SOUNDKONVERTER_KF5_BUILD
    #include <kcoreaddons_export.h>
    #include <QVariantList>
    #define VARG_TYPE QVariantList
#else
    #include <QStringList>
    #define VARG_TYPE QStringList
#endif

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


#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT FilterPluginItem : public CodecPluginItem
#else
class KDE_EXPORT FilterPluginItem : public CodecPluginItem
#endif
{
    Q_OBJECT
public:
    explicit FilterPluginItem( QObject *parent );
    virtual ~FilterPluginItem();
};


/** @author Daniel Faust <hessijames@gmail.com> */
#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT FilterPlugin : public CodecPlugin
#else
class KDE_EXPORT FilterPlugin : public CodecPlugin
#endif
{
    Q_OBJECT
public:
    explicit FilterPlugin( QObject *parent );
    virtual ~FilterPlugin();

    virtual QString type() const;

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

#ifndef SOUNDKONVERTER_KF5_BUILD
#define K_EXPORT_SOUNDKONVERTER_FILTER(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_filter_##libname, KGenericFactory<classname>("soundkonverter_filter_" #libname) )
#endif

#endif // FILTERPLUGIN_H

