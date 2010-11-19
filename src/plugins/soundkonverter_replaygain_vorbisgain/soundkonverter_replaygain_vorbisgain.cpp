
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

// QMap<QString,int> soundkonverter_replaygain_vorbisgain::codecList()
// {
//     QMap<QString,int> list;
// 
//     if( binaries["vorbisgain"] != "" )
//     {
//         list.insert( "ogg vorbis", 100 );
//     }
// 
//     return list;
// }

QList<ReplayGainPipe> soundkonverter_replaygain_vorbisgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "ogg vorbis";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["vorbisgain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for ogg vorbis files, you need to install 'vorbisgain'. vorbisgain is usually in the package 'vorbisgain' which should be shipped with your distribution.");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_vorbisgain::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "ogg vorbis" )
    {
        info.lossless = false;
        info.description = i18n("Ogg Vorbis is a free and lossy high quality audio codec.");
        info.mimeTypes.append( "application/ogg" );
        info.mimeTypes.append( "audio/vorbis" );
        info.mimeTypes.append( "application/x-ogg" );
        info.mimeTypes.append( "audio/ogg" );
        info.mimeTypes.append( "audio/x-vorbis+ogg" );
        info.extensions.append( "ogg" );
    }

    return info;
}

// QString soundkonverter_replaygain_vorbisgain::getCodecFromFile( const KUrl& filename, const QString& mimeType )
// {
//     if( mimeType == "application/x-ogg" || mimeType == "application/ogg" || mimeType == "audio/ogg" || mimeType == "audio/vorbis" || mimeType == "audio/x-vorbis+ogg" )
//     {
//         return "ogg vorbis";
//     }
//     else if( mimeType == "application/octet-stream" )
//     {
//         if( filename.url().endsWith(".ogg") ) return "ogg vorbis";
//     }
// 
//     return "";
// }

/*bool soundkonverter_replaygain_vorbisgain::canApply( const KUrl& filename )
{
    if( filename.url().endsWith(".ogg") ) return true;

    return false;
}*/

bool soundkonverter_replaygain_vorbisgain::isConfigSupported( ActionType action, const QString& codecName )
{
    return true;
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

// QString soundkonverter_replaygain_vorbisgain::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "vorbisgain";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "vorbisgain";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

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
