//
// C++ Interface: conversionoptionsmanager
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONVERSIONOPTIONSMANAGER_H
#define CONVERSIONOPTIONSMANAGER_H

#include <QList>

class ConversionOptions;
class PluginLoader;

/** @author Daniel Faust <hessijames@gmail.com> */
class ConversionOptionsManager
{
public:
    struct ConversionOptionsElement {
        int id;
        int references;
        ConversionOptions *conversionOptions;
    };

    ConversionOptionsManager( PluginLoader *_pluginLoader );
    ~ConversionOptionsManager();

    int addConversionOptions( ConversionOptions *conversionOptions );
    int increaseReferences( int id );
    ConversionOptions *getConversionOptions( int id );
    void removeConversionOptions( int id );
    int updateConversionOptions( int id, ConversionOptions *conversionOptions );
    
    QList<int> getAllIds();

private:
    PluginLoader *pluginLoader;

    QList<ConversionOptionsElement> elements;
    int idCounter;
};

#endif
