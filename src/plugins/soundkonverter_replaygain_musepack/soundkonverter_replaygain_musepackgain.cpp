
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

// QMap<QString,int> soundkonverter_replaygain_replaygain::codecList()
// {
//     QMap<QString,int> list;
// 
//     if( binaries["replaygain"] != "" )
//     {
//         list.insert( "ogg vorbis", 100 );
//     }
// 
//     return list;
// }

QList<ReplayGainPipe> soundkonverter_replaygain_musepackgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "musepack";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["replaygain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for musepack files, you need to install 'replaygain'.\nYou can get it at http://www.musepack.net");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_musepackgain::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "musepack" )
    {
        info.lossless = false;
        info.description = i18n("Musepack is a free and lossy file format based on mp2 and optimized for high quality."); // http://en.wikipedia.org/wiki/Musepack
        info.mimeTypes.append( "audio/x-musepack" );
        info.mimeTypes.append( "audio/musepack" );
        info.extensions.append( "mpc" );
        info.extensions.append( "mp+" );
        info.extensions.append( "mpp" );
    }

    return info;
}

// QString soundkonverter_replaygain_replaygain::getCodecFromFile( const KUrl& filename, const QString& mimeType )
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

/*bool soundkonverter_replaygain_replaygain::canApply( const KUrl& filename )
{
    if( filename.url().endsWith(".ogg") ) return true;

    return false;
}*/

bool soundkonverter_replaygain_musepackgain::isConfigSupported( ActionType action, const QString& codecName )
{
    return true;
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

// QString soundkonverter_replaygain_replaygain::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "replaygain";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "replaygain";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

float soundkonverter_replaygain_musepackgain::parseOutput( const QString& output )
{
    // no progress provided
    
    return -1;
}

#include "soundkonverter_replaygain_musepackgain.moc"
