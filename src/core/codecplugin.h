
#ifndef CODECPLUGIN_H
#define CODECPLUGIN_H

#include "backendplugin.h"
#include "conversionoptions.h"

class QWidget;
class CodecPlugin;
class TagData;


class KDE_EXPORT CodecPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    CodecPluginItem( QObject *parent=0 );
    ~CodecPluginItem();
  
    struct Data {
//         enum Mode { Enc, Dec } mode;
        int length;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KDE_EXPORT CodecPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    CodecPlugin( QObject *parent=0 );
    virtual ~CodecPlugin();
    
    virtual QString type();

    virtual QList<ConversionPipeTrunk> codecTable() = 0;
    virtual QWidget *newCodecWidget() = 0;
    virtual QWidget *deleteCodecWidget( QWidget *codecWidget );

    /** converts a file; TODO notify Conversion when the process has been finished */
    virtual int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;
    /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
    virtual QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;

    virtual ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions );
    
protected:
    ConversionOptions *lastUsedConversionOptions;

};

#define K_EXPORT_SOUNDKONVERTER_CODEC(libname, classname) \
        K_EXPORT_COMPONENT_FACTORY( soundkonverter_codec_##libname, KGenericFactory<classname>("soundkonverter_codec_" #libname) )

#endif // CODECPLUGIN_H

