
#include "wvreplaygainglobal.h"

#include "soundkonverter_replaygain_wvgain.h"


soundkonverter_replaygain_wvgain::soundkonverter_replaygain_wvgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    binaries["wvgain"] = "";

    allCodecs += "wavpack";
}

soundkonverter_replaygain_wvgain::~soundkonverter_replaygain_wvgain()
{}

QString soundkonverter_replaygain_wvgain::name()
{
    return global_plugin_name;
}

// QMap<QString,int> soundkonverter_replaygain_wvgain::codecList()
// {
//     QMap<QString,int> list;
// 
//     if( binaries["wvgain"] != "" )
//     {
//         list.insert( "ogg vorbis", 100 );
//     }
// 
//     return list;
// }

QList<ReplayGainPipe> soundkonverter_replaygain_wvgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "wavpack";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["wvgain"] != "" );
    newPipe.problemInfo = i18n("In order to calculate Replay Gain tags for wavpack files, you need to install 'wvgain'. wvgain is usually in the package 'wavpack'.");
    table.append( newPipe );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_replaygain_wvgain::formatInfo( const QString& codecName )
{
    BackendPlugin::FormatInfo info;
    info.codecName = codecName;

    if( codecName == "wavpack" )
    {
        info.lossless = true;
        info.description = i18n("WavPack is a free and lossless audio codec.\nFor more information see: http://www.wavpack.com");
        info.mimeTypes.append( "audio/x-wavpack" );
        info.extensions.append( "wv" );
        info.extensions.append( "wvp" );
    }

    return info;
}

/*bool soundkonverter_replaygain_wvgain::canApply( const KUrl& filename )
{
    if( filename.url().endsWith(".ogg") ) return true;

    return false;
}*/

bool soundkonverter_replaygain_wvgain::isConfigSupported( ActionType action, const QString& codecName )
{
    return true;
}

void soundkonverter_replaygain_wvgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{}

bool soundkonverter_replaygain_wvgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_wvgain::showInfo( QWidget *parent )
{}

int soundkonverter_replaygain_wvgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 ) return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

//     newItem->mode = mode;
    (*newItem->process) << binaries["wvgain"];
    if( mode == ReplayGainPlugin::Add )
    {
        (*newItem->process) << "-a";
        (*newItem->process) << "-n";
    }
    else if( mode == ReplayGainPlugin::Force )
    {
        (*newItem->process) << "-a";
    }
    else
    {
        (*newItem->process) << "-c";
    }
    for( int i=0; i<fileList.count(); i++ )
    {
        (*newItem->process) << fileList.at(i).toLocalFile();
    }
    newItem->process->start();

    backendItems.append( newItem );
    return newItem->id;
}

// QString soundkonverter_replaygain_wvgain::applyCommand( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
// {
//     QString command;
// 
//     if( fileList.count() <= 0 ) return command;
// 
//     if( mode == ReplayGainPlugin::Add )
//     {
//         command += "wvgain";
//         command += " --album";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
//     else
//     {
//         command += "wvgain";
//         command += " --clean";
//         for( int i = 0; i < fileList.count(); i++ )
//         {
//             command += " \"" + fileList.at(i).toLocalFile() + "\"";
//         }
//     }
// 
//     return command;
// }

float soundkonverter_replaygain_wvgain::parseOutput( const QString& output )
{
    // analyzing test.wv,  35% done...
  
    QRegExp reg("\\s+(\\d+)% done");
    if( output.contains(reg) )
    {
        return (float)reg.cap(1).toInt();
    }

    return -1;
}

#include "soundkonverter_replaygain_wvgain.moc"
