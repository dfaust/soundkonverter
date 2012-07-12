
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

unsigned int soundkonverter_replaygain_wvgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 )
        return BackendPlugin::UnknownError;

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    QStringList command;
    command += binaries["wvgain"];
    if( mode == ReplayGainPlugin::Add )
    {
        command += "-a";
        command += "-n";
    }
    else if( mode == ReplayGainPlugin::Force )
    {
        command += "-a";
    }
    else
    {
        command += "-c";
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
