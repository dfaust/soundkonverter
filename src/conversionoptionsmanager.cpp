
#include "conversionoptionsmanager.h"
#include "core/conversionoptions.h"
#include "pluginloader.h"

ConversionOptionsManager::ConversionOptionsManager( PluginLoader *_pluginLoader, QObject *parent )
    : QObject( parent ),
    pluginLoader( _pluginLoader )
{
    idCounter = 0;
}

ConversionOptionsManager::~ConversionOptionsManager()
{
    foreach( const ConversionOptionsElement& element, elements.values() )
    {
        delete element.conversionOptions;
    }
    elements.clear();
}

int ConversionOptionsManager::addConversionOptions( ConversionOptions *conversionOptions )
{
    if( !conversionOptions )
    {
        // FIXME error message, null pointer for conversion options
        return -1;
    }

    foreach( const ConversionOptionsElement& element, elements )
    {
        if( element.conversionOptions->pluginName == conversionOptions->pluginName )
        {
            if( element.conversionOptions->equals(conversionOptions) ) // NOTE equals gets reimplemented by the plugins
            {
                elements[element.id].references++;
                return element.id;
            }
        }
    }

    ConversionOptionsElement element;
    element.id = idCounter++;
    element.conversionOptions = conversionOptions;
    element.references = 1;
    elements.insert( element.id, element );
    return element.id;
}

int ConversionOptionsManager::increaseReferences( int id )
{
    if( elements.contains(id) )
    {
        elements[id].references++;
        return id;
    }

    return -1;
}

const ConversionOptions *ConversionOptionsManager::getConversionOptions( int id ) const
{
    if( elements.contains(id) )
    {
        return elements[id].conversionOptions;
    }

    return 0;
}

void ConversionOptionsManager::removeConversionOptions( int id )
{
    if( elements.contains(id) )
    {
        elements[id].references--;

        if( elements.value(id).references <= 0 )
        {
            delete elements.value(id).conversionOptions;
            elements.remove(id);
        }
    }
}

int ConversionOptionsManager::updateConversionOptions( int id, ConversionOptions *conversionOptions )
{
    removeConversionOptions( id );
    return addConversionOptions( conversionOptions );
}

QList<int> ConversionOptionsManager::getAllIds() const
{
    return elements.keys();
}
