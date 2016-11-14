
#ifndef CODECPLUGIN_H
#define CODECPLUGIN_H

#include <kcoreaddons_export.h>
#include <QVariantList>

#include "backendplugin.h"
#include "conversionoptions.h"

class CodecPlugin;
class CodecWidget;
class TagData;

class KCOREADDONS_EXPORT CodecPluginItem : public BackendPluginItem
{
    Q_OBJECT
public:
    explicit CodecPluginItem( QObject *parent );
    virtual ~CodecPluginItem();

    struct Data {
        int length;
    } data;
};


/** @author Daniel Faust <hessijames@gmail.com> */
class KCOREADDONS_EXPORT CodecPlugin : public BackendPlugin
{
    Q_OBJECT
public:
    explicit CodecPlugin( QObject *parent );
    virtual ~CodecPlugin();

    virtual QString type() const;

    virtual QList<ConversionPipeTrunk> codecTable() = 0;
    virtual CodecWidget *newCodecWidget() = 0;
    virtual CodecWidget *deleteCodecWidget( CodecWidget *codecWidget );

    /**
     * starts the conversion and returns either a conversion id or an error code:
     *
     * -1   unknown error
     * -100 plugin not configured
     */
    virtual int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;
    /** returns a command for converting a file through a pipe; "" if pipes aren't supported */
    virtual QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false ) = 0;

    virtual ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

    const ConversionOptions* lastConversionOptions();

protected:
    ConversionOptions *lastUsedConversionOptions;

};

#endif // CODECPLUGIN_H
