
#include "musepackreplaygainglobal.h"

#include "soundkonverter_replaygain_musepackgain.h"


soundkonverter_replaygain_musepackgain::soundkonverter_replaygain_musepackgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    binaries["replaygain"] = "";
    
    allCodecs += "musepack";
}

soundkonverter_replaygain_musepackgain::~soundkonverter_replaygain_musepackgain()
{}

QString soundkonverter_replaygain_musepackgain::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_musepackgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "musepack";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["replaygain"] != "" );
    newPipe.problemInfo = standardMessage( "replygain_codec,backend", "musepack", "replaygain" ) + "\n" + standardMessage( "install_website_backend,url", "replaygain", "http://www.musepack.net" );
    table.append( newPipe );

    return table;
}

bool soundkonverter_replaygain_musepackgain::isConfigSupported( ActionType action, const QString& codecName )
{
    return false;
}

void soundkonverter_replaygain_musepackgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_replaygain_musepackgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_musepackgain::showInfo( QWidget *parent )
{}

int soundkonverter_replaygain_musepackgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

//     newItem->mode = mode;
    (*newItem->process) << binaries["replaygain"];
    (*newItem->process) << "--auto";
    for( int i=0; i<fileList.count(); i++ )
    {
        (*newItem->process) << fileList.at(i).toLocalFile();
    }
    newItem->process->start();

    backendItems.append( newItem );
    return newItem->id;
}

float soundkonverter_replaygain_musepackgain::parseOutput( const QString& output )
{
    // no progress provided
    
    return -1;
}

#include "soundkonverter_replaygain_musepackgain.moc"
