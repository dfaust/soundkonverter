
#include "normalizefilterglobal.h"

#include "normalizefilteroptions.h"
#include "../../core/conversionoptions.h"


NormalizeFilterOptions::NormalizeFilterOptions()
{
    pluginName = global_plugin_name;
}

NormalizeFilterOptions::~NormalizeFilterOptions()
{}

bool NormalizeFilterOptions::equals( FilterOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    NormalizeFilterOptions *other = dynamic_cast<NormalizeFilterOptions*>(_other);

    return ( FilterOptions::equals( _other ) && data.normalize == other->data.normalize );
}

QDomElement NormalizeFilterOptions::toXml( QDomDocument document, const QString elementName )
{
    QDomElement filterOptions = FilterOptions::toXml( document,elementName );
    filterOptions.setAttribute("normalize",data.normalize);

    return filterOptions;
}

bool NormalizeFilterOptions::fromXml( QDomElement filterOptions )
{
    FilterOptions::fromXml( filterOptions );
    data.normalize = filterOptions.attribute("normalize").toInt();

    return true;
}
