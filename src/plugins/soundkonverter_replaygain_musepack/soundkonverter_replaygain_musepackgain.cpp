
#include "musepackreplaygainglobal.h"

#include "soundkonverter_replaygain_musepackgain.h"

#include <KStandardDirs>
#include <QFile>


soundkonverter_replaygain_musepackgain::soundkonverter_replaygain_musepackgain( QObject *parent, const QVariantList& args  )
    : ReplayGainPlugin( parent )
{
    Q_UNUSED(args)

    binaries["mpcgain"] = "";

    allCodecs += "musepack";
}

soundkonverter_replaygain_musepackgain::~soundkonverter_replaygain_musepackgain()
{}

QString soundkonverter_replaygain_musepackgain::name() const
{
    return global_plugin_name;
}

QList<ReplayGainPipe> soundkonverter_replaygain_musepackgain::codecTable()
{
    QList<ReplayGainPipe> table;
    ReplayGainPipe newPipe;

    newPipe.codecName = "musepack";
    newPipe.rating = 100;
    newPipe.enabled = ( binaries["mpcgain"] != "" );
    newPipe.problemInfo = standardMessage( "replygain_codec,backend", "musepack", "mpcgain" ) + "\n" + standardMessage( "install_website_backend,url", "mpcgain", "http://www.musepack.net" );
    table.append( newPipe );

    return table;
}

bool soundkonverter_replaygain_musepackgain::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_replaygain_musepackgain::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_replaygain_musepackgain::hasInfo()
{
    return false;
}

void soundkonverter_replaygain_musepackgain::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

int soundkonverter_replaygain_musepackgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
{
    if( fileList.count() <= 0 )
        return BackendPlugin::UnknownError;

    if( mode == ReplayGainPlugin::Remove )
        return BackendPlugin::FeatureNotSupported; // NOTE mpc gain does not support removing Replay Gain tags

    ReplayGainPluginItem *newItem = new ReplayGainPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    QStringList command;
    command += binaries["mpcgain"];
    foreach( const KUrl& file, fileList )
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

float soundkonverter_replaygain_musepackgain::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no progress provided

    return -1;
}

K_PLUGIN_FACTORY(replaygain_musepackgain, registerPlugin<soundkonverter_replaygain_musepackgain>();)

#include "soundkonverter_replaygain_musepackgain.moc"
