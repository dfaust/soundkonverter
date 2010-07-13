
#include "aacreplaygainglobal.h"

#include "soundkonverter_replaygain_aacgain.h"


soundkonverter_replaygain_aacgain::soundkonverter_replaygain_aacgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    binaries["aacgain"] = "";
}

soundkonverter_replaygain_aacgain::~soundkonverter_replaygain_aacgain()
{}

QString soundkonverter_replaygain_aacgain::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_aacgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "aac";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["aacgain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for aac files, you need to install 'aacgain'.\nSince aac is a patented file format, aacgain may not be included in the default installation of your distribution.\nSome distributions offer aacgain in an additional software repository.");
    table.append( newPipe );

    newPipe.codecName = "mp3";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["aacgain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for aac files, you need to install 'aacgain'.\nSince aac is a patented file format, aacgain may not be included in the default installation of your distribution.\nSome distributions offer aacgain in an additional software repository.");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_aacgain::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "aac" )
    {
        info.lossless = false;
        info.description = i18n("Advanced Audio Coding is a lossy and popular audio format."); // http://en.wikipedia.org/wiki/Advanced_Audio_Coding
        info.mimeTypes.append( "audio/aac" );
        info.mimeTypes.append( "audio/aacp" );
        info.mimeTypes.append( "audio/mp4" );
        info.extensions.append( "aac" );
        info.extensions.append( "3gp" );
        info.extensions.append( "mp4" );
        info.extensions.append( "m4a" );
    }
    else if( codecName == "mp3" )
    {
        info.lossless = false;
        info.description = i18n("MP3 is a very popular lossy audio codec.");
        info.mimeTypes.append( "audio/x-mp3" );
        info.mimeTypes.append( "audio/mpeg" );
        info.extensions.append( "mp3" );
    }

    return info;
}

QString soundkonverter_replaygain_aacgain::getCodecFromFile( const KUrl& filename, const QString& mimeType )
{
    if( mimeType == "audio/aac" || mimeType == "audio/aacp" || mimeType == "audio/mp4" )
    {
        return "aac";
    }
    else if( mimeType == "audio/x-mp3" || mimeType == "audio/mp3" || mimeType == "audio/mpeg" )
    {
        return "mp3";
    }
    else if( mimeType == "application/octet-stream" )
    {
        if( filename.url().endsWith(".aac") ) return "aac";
        if( filename.url().endsWith(".mp3") ) return "mp3";
    }

    return "";
}

bool soundkonverter_replaygain_aacgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_aacgain::showInfo()
{}

int soundkonverter_replaygain_aacgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

//     newItem->mode = mode;

    (*newItem->process) << binaries["aacgain"];
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

// QString soundkonverter_replaygain_aacgain::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "aacgain";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "aacgain";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

float soundkonverter_replaygain_aacgain::parseOutput( const QString& output )
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

#include "soundkonverter_replaygain_aacgain.moc"


