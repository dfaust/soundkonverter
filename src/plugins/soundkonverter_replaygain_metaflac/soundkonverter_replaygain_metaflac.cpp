
#include "flacreplaygainglobal.h"

#include "soundkonverter_replaygain_metaflac.h"


soundkonverter_replaygain_metaflac::soundkonverter_replaygain_metaflac( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    binaries["metaflac"] = "";

    allCodecs += "flac";
}

soundkonverter_replaygain_metaflac::~soundkonverter_replaygain_metaflac()
{}

QString soundkonverter_replaygain_metaflac::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_metaflac::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "flac";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["metaflac"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for flac files, you need to install 'metaflac'. metaflac is usually in the package 'flac' which should be shipped with your distribution.");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_metaflac::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "flac" )
    {
        info.lossless = true;
        info.description = i18n("FLAC is a free and lossless audio codec.");
        info.mimeTypes.append( "audio/x-flac" );
        info.mimeTypes.append( "audio/x-flac+ogg" );
        info.mimeTypes.append( "audio/x-oggflac" );
        info.extensions.append( "flac" );
        info.extensions.append( "fla" );
//         info.extensions.append( "ogg" );
    }

    return info;
}

bool soundkonverter_replaygain_metaflac::isConfigSupported( ActionType action, const QString& codecName )
{
    return true;
}

void soundkonverter_replaygain_metaflac::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_replaygain_metaflac::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_metaflac::showInfo( QWidget *parent )
{}

int soundkonverter_replaygain_metaflac::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

//     newItem->mode = mode;
    (*newItem->process) << binaries["metaflac"];
    if( mode == ReplayGainPlugin::Add || mode == ReplayGainPlugin::Force )
    {
        (*newItem->process) << "--add-replay-gain";
    }
    else
    {
        (*newItem->process) << "--remove-replay-gain";
    }
    for( int i=0; i<fileList.count(); i++ )
    {
        (*newItem->process) << fileList.at(i).toLocalFile();
    }
    newItem->process->start();

    backendItems.append( newItem );
    return newItem->id;
}

// QString soundkonverter_replaygain_metaflac::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "metaflac";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "metaflac";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

float soundkonverter_replaygain_metaflac::parseOutput( const QString& output )
{
    // metaflac doesn't provide any progress data
    return -1;
}

#include "soundkonverter_replaygain_metaflac.moc"
