//
// C++ Implementation: conversionoptionsmanager
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "conversionoptionsmanager.h"
#include "core/conversionoptions.h"
#include "pluginloader.h"

ConversionOptionsManager::ConversionOptionsManager( PluginLoader *_pluginLoader )
    : pluginLoader( _pluginLoader )
{
    idCounter = 0;
}

ConversionOptionsManager::~ConversionOptionsManager()
{
    for( int i=0; i<elements.size(); i++ )
    {
        delete elements.at(i).conversionOptions;
    }
    elements.clear();
}

int ConversionOptionsManager::addConversionOptions( ConversionOptions *conversionOptions )
{
    for( int i=0; i<elements.size(); i++ )
    {
        if( elements.at(i).conversionOptions->pluginName == conversionOptions->pluginName )
        {
//             CodecPlugin *plugin = pluginLoader->codecPluginByName( conversionOptions->pluginName );
//             if( !plugin ) break;
//             if( plugin->equal(elements.at(i).conversionOptions,conversionOptions) )
            if( elements.at(i).conversionOptions->equals(conversionOptions) ) // FIXME use the plugin's equal function / or does ist already work?
            {
                elements[i].references++;
                return elements.at(i).id;
            }
        }
    }

    ConversionOptionsElement newElement;
    newElement.id = idCounter++;
    newElement.conversionOptions = conversionOptions;
    newElement.references = 1;
    elements.append( newElement );
    return newElement.id;
}

int ConversionOptionsManager::increaseReferences( int id )
{
    for( int i=0; i<elements.size(); i++ )
    {
        if( elements.at(i).id == id )
        {
            elements[i].references++;
            return id;
        }
    }
    return -1;
}

ConversionOptions *ConversionOptionsManager::getConversionOptions( int id )
{
    for( int i=0; i<elements.size(); i++ )
    {
        if( elements.at(i).id == id ) return elements.at(i).conversionOptions;
    }
    return 0;
}

void ConversionOptionsManager::removeConversionOptions( int id )
{
    for( int i=0; i<elements.size(); i++ )
    {
        if( elements.at(i).id == id )
        {
            elements[i].references--;
            if( elements.at(i).references <= 0 )
            {
                delete elements.at(i).conversionOptions;
                elements.removeAt(i);
            }
            return;
        }
    }
}

int ConversionOptionsManager::updateConversionOptions( int id, ConversionOptions *conversionOptions )
{
    removeConversionOptions( id );
    return addConversionOptions( conversionOptions );
}

QList<int> ConversionOptionsManager::getAllIds()
{
    QList<int> ids;
  
    for( int i=0; i<elements.size(); i++ )
    {
        ids += elements.at(i).id;
    }
    
    return ids;
}
