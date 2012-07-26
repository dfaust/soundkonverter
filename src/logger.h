

#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <KUrl>


/**
 * @short An item for every process that is logged
 * @author Daniel Faust <hessijames@gmail.com>
 */
class LoggerItem
{
public:
    LoggerItem();
    ~LoggerItem();

    QString identifier;
    int id;
    QStringList data;
    bool completed;
    bool succeeded;
    QTime time;
    QFile file;
    QTextStream textStream;
};


/**
 * @short All data about the processes are collected here
 * @author Daniel Faust <hessijames@gmail.com>
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    Logger( QObject *parent );
    ~Logger();

    /** Creates a new logger item and returns the id of it, @p filename is added to the new logger item */
    int registerProcess( const QString& identifier );

    /** Adds the string @p data to the data of the logger item with id @p id */
    void log( int id, const QString& data );

    /** Returns a list of all logger item titles + ids */
//     QList<LoggerItem*> getLogOverview();

    /** Returns the logger item with id @p id */
    LoggerItem* getLog( int id );

    /** Returns a list of all logger items */
    QList<LoggerItem*> getLogs();

private:
    /** the list of all logger items */
    QList<LoggerItem*> processes;

    bool writeLogFiles;

    /** returns an unused random id */
    int getNewID();

public slots:
    void processCompleted( int id, bool succeeded, bool waitingForAlbumGain = false );
    // connected to config
    void updateWriteSetting( bool _writeLogFiles );

signals:
    void removedProcess( int id );
    void updateProcess( int id );

};

#endif // LOGGER_H
