
#ifndef BACKENDPLUGIN_H
#define BACKENDPLUGIN_H

#include <KGenericFactory>
#include <KProcess>
#include <KUrl>
#include <QList>
#include <QObject>

class BackendPlugin;


struct ConversionPipeTrunk
{
    QString codecFrom, codecTo;

    BackendPlugin *plugin;
    bool enabled; // can we use this conversion pipe? (all needed backends installed?)
    int rating;
    QString problemInfo; // howto message, if a backend is missing
    
    struct Data // additional data
    {
        bool hasInternalReplayGain;
        bool canRipEntireCd;
    } data;
};


class BackendPluginItem : public QObject
{
    Q_OBJECT
public:
    BackendPluginItem( QObject *parent=0 );
    virtual ~BackendPluginItem();

    KProcess *process;
    int id;
    float progress;
};

/**
    @author Daniel Faust <hessijames@gmail.com>
*/
class KDE_EXPORT BackendPlugin : public QObject
{
    Q_OBJECT
public:
    struct FormatInfo
    {
        QString codecName;      // the internal name of this codec
        bool lossless;          // for calculating a possible conversion pipe
        QString description;    // description for the simple tab
        QStringList mimeTypes;  // TODO this is obsolete - delete - really? what's with dolphin's actions context menu
        QStringList extensions;
    };
  
    BackendPlugin( QObject *parent=0 );
    virtual ~BackendPlugin();

    virtual QString name() = 0;
    virtual QString type() = 0;

    virtual FormatInfo formatInfo( const QString& codecName ) = 0;
    virtual QString getCodecFromFile( const KUrl& filename, const QString& mimeType = "application/octet-stream" );
    virtual bool hasInfo() = 0;
    virtual void showInfo() = 0;
    virtual bool pause( int id );
    virtual bool resume( int id );
    virtual bool kill( int id );
//     virtual void setPriority( int _priority );
    virtual float progress( int id );
    virtual float parseOutput( const QString& output ) = 0;

    /** search for the backend binaries in the given directories */
    virtual void scanForBackends( const QStringList& directoryList );
    /** holds all backend binaries and their location if they were found */
    QMap<QString,QString> binaries;

protected:
    QList<BackendPluginItem*> backendItems;
    int lastId;
    QStringList allCodecs;
//     int priority;

signals:
    void log( int id, const QString& message );
    void jobFinished( int id, int exitCode );

private slots:
    /** Get the process' output */
    virtual void processOutput();

    /** The process has exited */
    virtual void processExit( int exitCode, QProcess::ExitStatus /*exitStatus*/ );
};

#endif // BACKENDPLUGIN_H

