
#include "wvreplaygainglobal.h"

#include "soundkonverter_replaygain_wvgain.h"


soundkonverter_replaygain_wvgain::soundkonverter_replaygain_wvgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    Q_UNUSED(args)

    binaries["wvgain"] = "";

    allCodecs += "wavpack";
}

soundkonverter_replaygain_wvgain::~soundkonverter_replaygain_wvgain()
{}

QString soundkonverter_replaygain_wvgain::name()
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_wvgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "wavpack";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["wvgain"] != "" );
    newPipe.problemInfo = standardMessage( "replaygain_codec,backend", "wavpack", "wvgain" ) + "\n" + i18n( "'%1' is usually in the package '%2' which you can download at %3", QString("wvgain"), QString("wavpack"), QString("http://www.wavpack.com") );
    table.append( newPipe );

    return table;
}

bool soundkonverter_replaygain_wvgain::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_replaygain_wvgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_replaygain_wvgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_wvgain::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

int soundkonverter_replaygain_wvgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 )
        return -1;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

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
