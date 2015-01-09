
#ifndef DIRECTORYCRAWLER_H
#define DIRECTORYCRAWLER_H

#include <QThread>
#include <QStringList>

class DirectoryCrawler : public QThread
{
    Q_OBJECT

public:
    DirectoryCrawler(const QStringList &directories, bool recursive);
    ~DirectoryCrawler();

    void run();

public slots:
    void abort();

private:
    QStringList directories;
    bool recursive;
    bool abortCrawl;

    int countFiles(const QString &directory, bool recursive, int count=0);
    int listFiles(const QString &directory, bool recursive, int count=0);

signals:
    void fileCountChanged(int count);
    void fileCountFinished();
    void fileProgressChanged(int count);
    void filesFound(const QStringList &fileList);
};

#endif
