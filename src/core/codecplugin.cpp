
#include "codecplugin.h"
#include "codecwidget.h"


CodecPluginItem::CodecPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{
    data.length = 0;
}

CodecPluginItem::~CodecPluginItem()
{}


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

CodecWidget *CodecPlugin::deleteCodecWidget( CodecWidget *codecWidget )
{
    if( !codecWidget )
        return 0;

    if( lastUsedConversionOptions )
        delete lastUsedConversionOptions;

    lastUsedConversionOptions = codecWidget->currentConversionOptions();
    delete codecWidget;

    return 0;
}

ConversionOptions *CodecPlugin::conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions *options = new ConversionOptions();
    options->fromXml( conversionOptions, filterOptionsElements );
    return options;
}

