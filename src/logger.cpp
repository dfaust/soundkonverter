
#include "logger.h"

#include <KLocale>
#include <KStandardDirs>
#include <KConfigGroup>

#include <cstdlib>
#include <ctime>


#define MAX_LOGS  20
#define MAX_LINES 10000


LoggerItem::LoggerItem( int logId, const QString& logIdentifier )
{
    id = logId;
    identifier = logIdentifier;
    file.setFileName( KStandardDirs::locateLocal("data",QString("soundkonverter/log/%1.log").arg(id)) );
    completed = false;
    succeeded = false;
}

LoggerItem::~LoggerItem()
{
    if( file.isOpen() )
        file.close();

    if( file.exists() )
        file.remove();
}


Logger::Logger( QObject *parent)
    : QObject( parent )
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;
    group = conf->group( "General" );
    writeLogFiles = group.readEntry( "writeLogFiles", false );

    LoggerItem *item = new LoggerItem( 1000, "soundKonverter" );
    item->completed = true;
    item->succeeded = true;
    if( writeLogFiles )
    {
        // TODO error handling
        item->file.open( QIODevice::WriteOnly );
        item->textStream.setDevice( &(item->file) );
    }

    processes.insert( item->id, item );

    srand( (unsigned)time(NULL) );
}

Logger::~Logger()
{
    qDeleteAll(processes);
}

int Logger::registerProcess( const QString& identifier )
{
    LoggerItem *item = new LoggerItem( getNewID(), identifier );
    if( writeLogFiles )
    {
        // TODO error handling
        item->file.open( QIODevice::WriteOnly );
        item->textStream.setDevice( &(item->file) );
    }

    processes.insert( item->id, item );

    log( item->id, i18n("Identifier") + ": " + item->identifier );
    log( item->id, i18n("Log ID") + ": " + QString::number(item->id) );

    emit updateProcess( item->id );

    return item->id;
}

void Logger::log( int id, const QString& data )
{
    if( processes.contains(id) )
    {
        LoggerItem* const process = processes.value(id);

        process->data.append( data );

        while( process->data.count() > MAX_LINES )
            process->data.removeFirst();

        if( writeLogFiles && process->file.isOpen() )
        {
            process->textStream << data;
            process->textStream << "\n";
            process->textStream.flush();
        }

        if( id == 1000 )
            emit updateProcess( id );
    }
}

int Logger::getNewID()
{
    int id;

    do {
        id = rand();
    } while( processes.contains(id) );

    return id;
}

const LoggerItem* Logger::getLog( int id ) const
{
    return processes.value(id, 0);
}

QList< QPair<int, QString> > Logger::getLogs() const
{
    QList< QPair<int, QString> > logs;

    foreach( LoggerItem* process, processes )
    {
        logs << QPair<int, QString>(process->id, process->identifier);
    }

    return logs;
}

void Logger::processCompleted( int id, bool succeeded, bool waitingForAlbumGain )
{
    Q_UNUSED( waitingForAlbumGain )

    if( processes.contains(id) )
    {
        LoggerItem* process = processes.value(id);

        process->succeeded = succeeded;
        process->completed = true;
        process->time = process->time.currentTime();
        process->data.append( i18n("Finished logging") );
        if( process->file.isOpen() )
        {
            process->textStream << i18n("Finished logging");
            process->file.close();
        }
        emit updateProcess( id );
    }

    if( processes.count() > MAX_LOGS )
    {
        QTime time = QTime::currentTime();

        int removeId = -1;
        foreach( const LoggerItem* process, processes.values() )
        {
            if( process->time < time && process->completed && process->succeeded && process->id != 1000 )
            {
                time = process->time;
                removeId = process->id;
            }
        }

        if( removeId > -1 )
        {
            emit removedProcess( removeId );
            delete processes.value( removeId );
            processes.remove( removeId );
        }
    }
}

void Logger::updateWriteSetting( bool _writeLogFiles )
{
    writeLogFiles = _writeLogFiles;
}
