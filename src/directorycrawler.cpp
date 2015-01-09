
#include "directorycrawler.h"

#include <QDir>

DirectoryCrawler::DirectoryCrawler(const QStringList &directories, bool recursive) :
    QThread()
{
    this->directories = directories;
    this->recursive = recursive;

    abortCrawl = false;
}

DirectoryCrawler::~DirectoryCrawler()
{
}

void DirectoryCrawler::run()
{
    int count = 0;

    foreach( const QString &directory, directories )
    {
        count = countFiles(directory, recursive, count);
    }

    fileCountFinished();

    count = 0;

    foreach( const QString &directory, directories )
    {
        count = listFiles(directory, recursive, count);
    }
}

int DirectoryCrawler::countFiles(const QString &directory, bool recursive, int count)
{
    if( abortCrawl )
        return count;

    QDir dir(directory);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Unsorted);

    foreach( const QFileInfo fileInfo, dir.entryInfoList() )
    {
        if( fileInfo.isFile() )
        {
            count++;
        }
        else if( recursive )
        {
            count = countFiles(fileInfo.absoluteFilePath(), recursive, count);
        }
    }

    msleep(1);
    emit fileCountChanged(count);

    return count;
}

int DirectoryCrawler::listFiles(const QString &directory, bool recursive, int count)
{
    if( abortCrawl )
        return count;

    QDir dir(directory);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::LocaleAware | QDir::DirsFirst);

    QStringList fileList;

    foreach( const QFileInfo fileInfo, dir.entryInfoList() )
    {
        if( fileInfo.isFile() )
        {
            fileList += fileInfo.absoluteFilePath();
        }
        else if( recursive )
        {
            count = listFiles(fileInfo.absoluteFilePath(), recursive, count);
        }
    }

    count += fileList.count();

    msleep(1);
    emit fileProgressChanged(count);
    emit filesFound(fileList);

    return count;
}

void DirectoryCrawler::abort()
{
    abortCrawl = true;
}
