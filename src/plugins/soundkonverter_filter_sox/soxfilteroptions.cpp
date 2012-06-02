
#include "soxfilterglobal.h"

#include "soxfilteroptions.h"
#include "../../core/conversionoptions.h"


SoxFilterOptions::SoxFilterOptions()
{
    pluginName = global_plugin_name;
}

SoxFilterOptions::~SoxFilterOptions()
{}

bool SoxFilterOptions::equals( FilterOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    SoxFilterOptions *other = dynamic_cast<SoxFilterOptions*>(_other);

    return ( FilterOptions::equals( _other ) && data.normalize == other->data.normalize );
}

QDomElement SoxFilterOptions::toXml( QDomElement _filterOptions )
{
    QDomElement filterOptions = FilterOptions::toXml( _filterOptions );
    filterOptions.setAttribute("normalize",data.normalize);
    filterOptions.setAttribute("normalizeVolume",data.normalizeVolume);

    return filterOptions;
}

bool SoxFilterOptions::fromXml( QDomElement filterOptions )
{
    FilterOptions::fromXml( filterOptions );
    data.normalize = filterOptions.attribute("normalize").toInt();
    data.normalizeVolume = filterOptions.attribute("normalizeVolume").toDouble();

    return true;
}
