
#ifndef CONVERSIONOPTIONSMANAGER_H
#define CONVERSIONOPTIONSMANAGER_H

#include <QList>
#include <QHash>

class ConversionOptions;
class PluginLoader;

class ConversionOptionsManager : public QObject
{
    Q_OBJECT

public:
    struct ConversionOptionsElement {
        int id;
        int references;
        ConversionOptions *conversionOptions;
    };

    ConversionOptionsManager( PluginLoader *_pluginLoader, QObject *parent );
    ~ConversionOptionsManager();

    int addConversionOptions( ConversionOptions *conversionOptions );
    int increaseReferences( int id );
    const ConversionOptions *getConversionOptions( int id ) const;
    void removeConversionOptions( int id );
    int updateConversionOptions( int id, ConversionOptions *conversionOptions );
    
    QList<int> getAllIds() const;

private:
    PluginLoader *pluginLoader;

    QHash<int, ConversionOptionsElement> elements;
    int idCounter;
};

#endif
