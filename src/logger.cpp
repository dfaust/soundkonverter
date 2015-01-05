
#include "logger.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QStandardPaths>
#include <QDir>

#include <cstdlib>
#include <ctime>

LoggerItem::LoggerItem()
{
}

LoggerItem::~LoggerItem()
{
}

Logger::Logger(QObject *parent) :
    QObject(parent)
{
    KConfigGroup group(KSharedConfig::openConfig(), "General");
    writeLogFiles = group.readEntry("writeLogFiles", false);

    logPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/log/";

    LoggerItem *process = new LoggerItem();
    process->identifier = "soundKonverter";
    process->id = 0;
    process->completed = true;
    process->succeeded = true;

    if( writeLogFiles )
    {
        QDir dir = QDir::root();
        dir.mkpath(logPath);

        process->file.setFileName(logPath + "soundkonverter.log");

        if( process->file.open(QIODevice::WriteOnly) )
        {
            process->textStream.setDevice(&(process->file));
        }
        else
        {
            log(i18n("Can't open file \"%1\" for writing", process->file.fileName()));
        }
    }

    processes.append(process);

    srand((unsigned)time(NULL));
}

Logger::~Logger()
{
    foreach( LoggerItem *process, processes )
    {
        if( process->file.isOpen() )
            process->file.close();

        if( process->file.exists() )
            process->file.remove();

        delete process;
    }
}

int Logger::registerProcess(const QString &identifier)
{
    LoggerItem *process = new LoggerItem();
    process->identifier = identifier;
    process->id = generateId();
    process->completed = false;

    if( writeLogFiles )
    {
        QDir dir = QDir::root();
        dir.mkpath(logPath);

        process->file.setFileName(logPath + QString::number(process->id) + ".log");

        if( process->file.open(QIODevice::WriteOnly) )
        {
            process->textStream.setDevice(&(process->file));
        }
        else
        {
            log(i18n("Can't open file \"%1\" for writing", process->file.fileName()));
        }
    }

    processes.append(process);

    log(process->id, i18n("Identifier: %1", process->identifier));
    log(process->id, i18n("Log ID: %1", process->id));

    emit updateProcess(process->id);

    return process->id;
}

void Logger::log(int id, const QString &data)
{
    foreach( LoggerItem *process, processes )
    {
        if( process->id == id )
        {
            process->data.append(data);

            if( process->data.count() > 10000 )
                process->data.removeFirst();

            if( writeLogFiles && process->file.isOpen() )
            {
                process->textStream << data << "\n";
                process->textStream.flush();
            }

            if( id == 0 )
                emit updateProcess(id);

            break;
        }
    }
}

void Logger::log(const QString &data)
{
    log(0, data);
}

int Logger::generateId()
{
    bool ok;
    int id;

    do
    {
        id = rand();
        ok = true;

        foreach( const LoggerItem *process, processes )
        {
            if( process->id == id )
            {
                ok = false;
                break;
            }
        }

    } while( !ok );

    return id;
}

const LoggerItem *Logger::getLog(int id)
{
    foreach( const LoggerItem *process, processes )
    {
        if( process->id == id )
        {
            return process;
        }
    }

    return 0;
}

QList<LoggerItem *> Logger::getLogs()
{
    return processes;
}

void Logger::processCompleted(int id, bool succeeded, bool waitingForAlbumGain)
{
    Q_UNUSED(waitingForAlbumGain)

    LoggerItem *removeItem = 0;
    QTime time = QTime::currentTime();

    foreach( LoggerItem *process, processes )
    {
        if( process->time < time && process->completed && process->succeeded && process->id != 0 )
        {
            time = process->time;
            removeItem = process;
        }
        else if( process->id == id )
        {
            process->succeeded = succeeded;
            process->completed = true;
            process->time = process->time.currentTime();
            process->data.append(i18n("Finished logging"));

            if( process->file.isOpen() )
            {
                process->textStream << i18n("Finished logging");
                process->file.close();
            }

            emit updateProcess(id);
        }
    }

    if( removeItem && processes.count() > 11 )
    {
        emit removedProcess(removeItem->id);

        if( removeItem->file.exists() )
            removeItem->file.remove();

        processes.removeAt(processes.indexOf(removeItem));
        delete removeItem;
    }
}

void Logger::updateWriteSetting(bool writeLogFiles)
{
    this->writeLogFiles = writeLogFiles;
}
