
#include "codecplugin.h"
#include "codecwidget.h"


//
// class CodecPluginItem
//
////////////////////

CodecPluginItem::CodecPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{}

CodecPluginItem::~CodecPluginItem()
{}


//
// class CodecPlugin
//
////////////////////

CodecPlugin::CodecPlugin( QObject *parent )
    : BackendPlugin( parent )
{
    lastUsedConversionOptions = 0;
}

CodecPlugin::~CodecPlugin()
{}

QString CodecPlugin::type()
{
    return "codec";
}

QWidget *CodecPlugin::deleteCodecWidget( QWidget *codecWidget )
{
    if( lastUsedConversionOptions )
        delete lastUsedConversionOptions;

    lastUsedConversionOptions = qobject_cast<CodecWidget*>(codecWidget)->currentConversionOptions();
    codecWidget->deleteLater();

    return 0;
}

ConversionOptions *CodecPlugin::conversionOptionsFromXml( QDomElement conversionOptions )
{
    ConversionOptions *options = new ConversionOptions();
    options->fromXml( conversionOptions );
    return options;
}

