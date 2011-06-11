
#include "vorbisreplaygainglobal.h"

#include "soundkonverter_replaygain_vorbisgain.h"


soundkonverter_replaygain_vorbisgain::soundkonverter_replaygain_vorbisgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
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
    return false;
}

void soundkonverter_replaygain_vorbisgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_replaygain_vorbisgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_vorbisgain::showInfo( QWidget *parent )
{}

int soundkonverter_replaygain_vorbisgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    for( int i=0; i<fileList.count(); i++ )
    {
        newItem->data.lengthList += 200;
    }

//     newItem->mode = mode;
    (*newItem->process) << binaries["vorbisgain"];
    if( mode == ReplayGainPlugin::Add )
    {
        (*newItem->process) << "--album";
        (*newItem->process) << "--fast";
    }
    else if( mode == ReplayGainPlugin::Force )
    {
        (*newItem->process) << "--album";
    }
    else
    {
        (*newItem->process) << "--clean";
    }
    for( int i=0; i<fileList.count(); i++ )
    {
        (*newItem->process) << fileList.at(i).toLocalFile();
    }
    newItem->process->start();

    backendItems.append( newItem );
    return newItem->id;
}

// float soundkonverter_replaygain_vorbisgain::parseOutput( const QString& output, BackendPluginItem *backendItem ) TODO ogg replaygain fix
float soundkonverter_replaygain_vorbisgain::parseOutput( const QString& output )
{
    // 35% - /home/daniel/soundKonverter/LP3/2 - 04 - Ratatat - Mirando.ogg
    
    if( output.contains("Gain   |  Peak  | Scale | New Peak | Track") || output.contains("----------+--------+-------+----------+------") ) return 0.0f;
    if( output == "" || !output.contains("%") ) return -1.0f;

    QString data = output;
    data = data.left( data.indexOf("%") );
    return data.toFloat();
}

#include "soundkonverter_replaygain_vorbisgain.moc"
