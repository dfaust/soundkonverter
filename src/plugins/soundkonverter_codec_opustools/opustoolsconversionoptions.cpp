
#include "opustoolscodecglobal.h"

#include "opustoolsconversionoptions.h"


OpusToolsConversionOptions::OpusToolsConversionOptions()
    : ConversionOptions()
{
    pluginName = global_plugin_name;
}

OpusToolsConversionOptions::~OpusToolsConversionOptions()
{}

bool OpusToolsConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    OpusToolsConversionOptions *other = dynamic_cast<OpusToolsConversionOptions*>(_other);

    return ( equalsBasics(_other) && equalsFilters(_other) && data.floatBitrate == other->data.floatBitrate );
}

QDomElement OpusToolsConversionOptions::toXml( QDomDocument document )
{
    QDomElement conversionOptions = ConversionOptions::toXml( document );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = document.createElement("data");
    data.setAttribute("floatBitrate",OpusToolsConversionOptions::data.floatBitrate);
    encodingOptions.appendChild(data);

    return conversionOptions;
}

bool OpusToolsConversionOptions::fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions::fromXml( conversionOptions, filterOptionsElements );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    OpusToolsConversionOptions::data.floatBitrate = data.attribute("floatBitrate").toFloat();

    return true;
}














