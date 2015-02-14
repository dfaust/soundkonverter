
#ifndef CODECPLUGIN_H
#define CODECPLUGIN_H

#ifdef SOUNDKONVERTER_KF5_BUILD
    #include <kcoreaddons_export.h>
    #include <QVariantList>
    #define VARG_TYPE QVariantList
#else
    #include <QStringList>
    #define VARG_TYPE QStringList
#endif

#include "backendplugin.h"
#include "conversionoptions.h"

class CodecPlugin;
class CodecWidget;
class TagData;

#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT CodecPluginItem : public BackendPluginItem
#else
class KDE_EXPORT CodecPluginItem : public BackendPluginItem
#endif
{
    Q_OBJECT
public:
    CodecPluginItem( QObject *parent=0 );
    virtual ~CodecPluginItem();

    struct Data {
        int length;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
#ifdef SOUNDKONVERTER_KF5_BUILD
class KCOREADDONS_EXPORT CodecPlugin : public BackendPlugin
#else
class KDE_EXPORT CodecPlugin : public BackendPlugin
#endif
{
    Q_OBJECT
public:
    CodecPlugin( QObject *parent=0 );
    virtual ~CodecPlugin();

    virtual QString type();

    virtual QList<ConversionPipeTrunk> codecTable() = 0;
    virtual CodecWidget *newCodecWidget() = 0;
    virtual CodecWidget *deleteCodecWidget( CodecWidget *codecWidget );

    /**
     * starts the conversion and returns either a conversion id or an error code:
     *
     * -1   unknown error
     * -100 plugin not configured
     */
    virtual unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;
    /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
    virtual QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;

    virtual ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

    ConversionOptions *lastUsedConversionOptions;

};

#ifndef SOUNDKONVERTER_KF5_BUILD
#define K_EXPORT_SOUNDKONVERTER_CODEC(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_codec_##libname, KGenericFactory<classname>("soundkonverter_codec_" #libname) )
#endif

#endif // CODECPLUGIN_H

