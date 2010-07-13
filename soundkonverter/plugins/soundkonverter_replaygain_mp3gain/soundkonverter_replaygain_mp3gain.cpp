
#include "mp3replaygainglobal.h"

#include "soundkonverter_replaygain_mp3gain.h"


soundkonverter_replaygain_mp3gain::soundkonverter_replaygain_mp3gain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    binaries["mp3gain"] = "";
}

soundkonverter_replaygain_mp3gain::~soundkonverter_replaygain_mp3gain()
{}

QString soundkonverter_replaygain_mp3gain::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_mp3gain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "mp3";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["mp3gain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for mp3 files, you need to install 'mp3gain'.\nSince mp3 is a patented file format, mp3gain may not be included in the default installation of your distribution.\nSome distributions offer mp3gain in an additional software repository.");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_mp3gain::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "mp3" )
    {
        info.lossless = false;
        info.description = i18n("MP3 is a very popular lossy audio codec.");
        info.mimeTypes.append( "audio/x-mp3" );
        info.mimeTypes.append( "audio/mpeg" );
        info.extensions.append( "mp3" );
    }

    return info;
}

QString soundkonverter_replaygain_mp3gain::getCodecFromFile( const KUrl& filename, const QString& mimeType )
{
    if( mimeType == "audio/x-mp3" || mimeType == "audio/mp3" || mimeType == "audio/mpeg" )
    {
        return "mp3";
    }
    else if( mimeType == "application/octet-stream" )
    {
        if( filename.url().endsWith(".mp3") ) return "mp3";
    }

    return "";
}

bool soundkonverter_replaygain_mp3gain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_mp3gain::showInfo()
{}

int soundkonverter_replaygain_mp3gain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

//     newItem->mode = mode;

    (*newItem->process) << binaries["mp3gain"];
    if( mode == ReplayGainPlugin::Add )
    {
        (*newItem->process) << "-a";
    }
    else if( mode == ReplayGainPlugin::Force )
    {
        (*newItem->process) << "-s";
        (*newItem->process) << "r";
    }
    else
    {
        (*newItem->process) << "-s";
        (*newItem->process) << "d";
    }
    for( int i = 0; i < fileList.count(); i++ )
    {
        (*newItem->process) << fileList.at(i).toLocalFile();
    }
    newItem->process->start();

    backendItems.append( newItem );
    return newItem->id;
}

// QString soundkonverter_replaygain_mp3gain::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "mp3gain";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "mp3gain";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

float soundkonverter_replaygain_mp3gain::parseOutput( const QString& output )
{
    //  9% of 45218064 bytes analyzed
    // [1/10] 32% of 13066690 bytes analyzed
    
    float progress = -1.0f;

    QRegExp reg1("\\[(\\d+)/(\\d+)\\] (\\d+)%");
    QRegExp reg2("(\\d+)%");
    if( output.contains(reg1) )
    {
        float fraction = 1.0f/reg1.cap(2).toInt();
        progress = 100*(reg1.cap(1).toInt()-1)*fraction + reg1.cap(3).toInt()*fraction;
    }
    else if( output.contains(reg2) )
    {
        progress = reg2.cap(1).toInt();
    }
    
    return progress;
/*
    if( output == "" || !output.contains("%") ) return -1.0f;

    QString data = output;
    int space = data.indexOf(" ") + 1;
    int percent = data.indexOf("%");
    data = data.mid( space, percent-space );
    return data.toFloat();*/
}

#include "soundkonverter_replaygain_mp3gain.moc"


