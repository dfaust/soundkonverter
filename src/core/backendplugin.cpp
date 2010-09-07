
#include "backendplugin.h"

#include <QFile>


BackendPluginItem::BackendPluginItem( QObject *parent )
    : QObject( parent )
{
    process = 0;
    id = -1;
    progress = 0.0f;
}

BackendPluginItem::~BackendPluginItem()
{}


BackendPlugin::BackendPlugin( QObject *parent )
    : QObject( parent )
{
    lastId = 0;
}

BackendPlugin::~BackendPlugin()
{}

BackendPlugin::FormatInfo BackendPlugin::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "wav" )
    {
        info.lossless = true;
        info.description = i18n("Wave won't compress the audio stream.");
        info.mimeTypes.append( "audio/x-wav" );
        info.mimeTypes.append( "audio/wav" );
        info.extensions.append( "wav" );
    }

    return info;
}

void BackendPlugin::scanForBackends( const QStringList& directoryList )
{
    for( QMap<QString, QString>::Iterator a = binaries.begin(); a != binaries.end(); ++a )
    {
        a.value() = "";
        for( QList<QString>::const_iterator b = directoryList.begin(); b != directoryList.end(); ++b )
        {
            if( QFile::exists((*b) + "/" + a.key()) )
            {
                a.value() = (*b) + "/" + a.key();
            }
        }
    }
}

QString BackendPlugin::getCodecFromFile( const KUrl& filename, const QString& mimeType )
{
    for( int i=0; i<allCodecs.count(); i++ )
    {
        if( formatInfo(allCodecs.at(i)).mimeTypes.indexOf(mimeType) != -1 )
        {
            return allCodecs.at(i);
        }
    }
    
    const QString extension = filename.url().mid( filename.url().lastIndexOf(".") + 1 );

    for( int i=0; i<allCodecs.count(); i++ )
    {
        if( formatInfo(allCodecs.at(i)).extensions.indexOf(extension) != -1 )
        {
            return allCodecs.at(i);
        }
    }
        
    return "";
}

bool BackendPlugin::pause( int id )
{
    return false;
}

bool BackendPlugin::resume( int id )
{
    return false;
}

bool BackendPlugin::kill( int id )
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->id == id && backendItems.at(i)->process != 0 )
        {
            backendItems.at(i)->process->kill();
            emit log( id, i18n("Killing process on user request") );
            return true;
        }
    }
    return false;
}

float BackendPlugin::progress( int id )
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->id == id )
        {
            return backendItems.at(i)->progress;
        }
    }
    return 0.0f;
}

// void BackendPlugin::setPriority( int _priority )
// {
//     priority = _priority;
//     
//     for( int i=0; i<backendItems.size(); i++ )
//     {
//         backendItems.at(i)->process->setPriority( priority );
//     }    
// }

void BackendPlugin::processOutput()
{
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            progress = parseOutput( output );
            if( progress > backendItems.at(i)->progress ) backendItems.at(i)->progress = progress;
            if( progress == -1 ) emit log( backendItems.at(i)->id, output );
            return;
        }
    }
}

void BackendPlugin::processExit( int exitCode, QProcess::ExitStatus /*exitStatus*/ )
{
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            emit jobFinished( backendItems.at(i)->id, exitCode );
            delete backendItems.at(i);
            backendItems.removeAt(i);
            return;
        }
    }
}
