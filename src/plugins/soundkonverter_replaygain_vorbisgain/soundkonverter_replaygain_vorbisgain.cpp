
#include "vorbisreplaygainglobal.h"

#include "soundkonverter_replaygain_vorbisgain.h"


soundkonverter_replaygain_vorbisgain::soundkonverter_replaygain_vorbisgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    Q_UNUSED(args)

    binaries["vorbisgain"] = "";

    allCodecs += "ogg vorbis";
}

soundkonverter_replaygain_vorbisgain::~soundkonverter_replaygain_vorbisgain()
{}

QString soundkonverter_replaygain_vorbisgain::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_vorbisgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "ogg vorbis";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["vorbisgain"] != "" );
    newPipe.problemInfo = standardMessage( "replygain_codec,backend", "ogg vorbis", "vorbisgain" ) + "\n" + standardMessage( "install_opensource_backend", "vorbisgain" );
    table.append( newPipe );

    return table;
}

bool soundkonverter_replaygain_vorbisgain::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_replaygain_vorbisgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_replaygain_vorbisgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_vorbisgain::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

unsigned int soundkonverter_replaygain_vorbisgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 )
        return BackendPlugin::UnknownError;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->data.fileCount = fileList.count();

    QStringList command;
    command += binaries["vorbisgain"];
    if( mode == ReplayGainPlugin::Add )
    {
        command += "--album";
        command += "--fast";
    }
    else if( mode == ReplayGainPlugin::Force )
    {
        command += "--album";
    }
    else
    {
        command += "--clean";
    }
    foreach( const KUrl file, fileList )
    {
        command += "\"" + escapeUrl(file) + "\"";
    }

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    logCommand( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

// float soundkonverter_replaygain_vorbisgain::parseOutput( const QString& output, BackendPluginItem *backendItem ) TODO ogg replaygain fix
float soundkonverter_replaygain_vorbisgain::parseOutput( const QString& output, ReplayGainPluginItem *replayGainItem )
{
    float progress = -1;

    // -12.14 dB |  46927 |  0.25 |    11599 | 03 - Sugar.ogg
    //   59% - 04 - Suggestions.ogg

    QRegExp regApply("(\\d+)%");
    if( output.contains(regApply) )
    {
        progress = (float)regApply.cap(1).toInt();
    }

    if( progress == -1 )
        return -1;

    if( !replayGainItem )
        return progress;

    if( progress > 90 && replayGainItem->data.lastFileProgress <= 90 )
    {
        replayGainItem->data.processedFiles++;
    }
    replayGainItem->data.lastFileProgress = progress;

    int processedFiles = replayGainItem->data.processedFiles;
    if( progress > 90 )
        processedFiles--;

    return float( processedFiles * 100 + progress ) / replayGainItem->data.fileCount;
}

float soundkonverter_replaygain_vorbisgain::parseOutput( const QString& output )
{
    return parseOutput( output, 0 );
}

void soundkonverter_replaygain_vorbisgain::processOutput()
{
    ReplayGainPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            const QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<ReplayGainPluginItem*>(backendItems.at(i));
            progress = parseOutput( output, pluginItem );

            if( progress == -1 && !output.simplified().isEmpty() )
                emit log( backendItems.at(i)->id, output );

            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

#include "soundkonverter_replaygain_vorbisgain.moc"
