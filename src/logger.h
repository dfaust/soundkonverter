
#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QTime>
#include <QFile>
#include <QTextStream>
// #include <QUrl>

/** An item for every process that is logged */
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

/** All data about the processes are collected here */
class Logger : public QObject
{
    Q_OBJECT

public:
    Logger(QObject *parent);
    ~Logger();

    /** Creates a new logger item and returns the id of it, @p filename is added to the new logger item */
    int registerProcess(const QString& identifier);

    /** Adds the string @p data to the data of the logger item with id @p id */
    void log(int id, const QString& data);
    void log(const QString& data);

    /** Returns a list of all logger item titles + ids */
//     QList<LoggerItem*> getLogOverview();

    /** Returns the logger item with id @p id */
    const LoggerItem* getLog(int id);

    /** Returns a list of all logger items */
    QList<LoggerItem*> getLogs();

private:
    /** the list of all logger items */
    QList<LoggerItem*> processes;

    bool writeLogFiles;

    QString logPath;

    /** returns an unused random id */
    int generateId();

public slots:
    void processCompleted(int id, bool succeeded, bool waitingForAlbumGain=false);
    // connected to config
    void updateWriteSetting(bool _writeLogFiles);

signals:
    void removedProcess(int id);
    void updateProcess(int id);
};

#endif // LOGGER_H
