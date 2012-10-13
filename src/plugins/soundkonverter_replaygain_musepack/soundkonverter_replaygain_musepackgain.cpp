
#include "musepackreplaygainglobal.h"

#include "soundkonverter_replaygain_musepackgain.h"

#include <KStandardDirs>
#include <QFile>


soundkonverter_replaygain_musepackgain::soundkonverter_replaygain_musepackgain( QObject *parent, const QStringList& args  )
    : ReplayGainPlugin( parent )
{
    Q_UNUSED(args)

    binaries["replaygain"] = "";

    allCodecs += "musepack";
}

soundkonverter_replaygain_musepackgain::~soundkonverter_replaygain_musepackgain()
{}

QString soundkonverter_replaygain_musepackgain::name()
{
    return global_plugin_name;
}

void soundkonverter_replaygain_musepackgain::scanForBackends( const QStringList& directoryList )
{
    binaries["replaygain"] = KStandardDirs::findExe( "replaygain" ); // sv7
    if( binaries["replaygain"].isEmpty() )
        binaries["replaygain"] = KStandardDirs::findExe( "mpcgain" ); // sv8

    if( binaries["replaygain"].isEmpty() )
    {
        for( QList<QString>::const_iterator b = directoryList.begin(); b != directoryList.end(); ++b )
        {
            if( QFile::exists((*b) + "/replaygain") )
            {
                binaries["replaygain"] = (*b) + "/replaygain";
                break;
            }
            else if( QFile::exists((*b) + "/mpcgain") )
            {
                binaries["replaygain"] = (*b) + "/mpcgain";
                break;
            }
        }
    }
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

unsigned int soundkonverter_replaygain_musepackgain::apply( const KUrl::List& fileList, ReplayGainPlugin::ApplyMode mode )
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
    command += binaries["replaygain"];
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

float soundkonverter_replaygain_musepackgain::parseOutput( const QString& output )
{
    Q_UNUSED(output)

    // no progress provided

    return -1;
}

#include "soundkonverter_replaygain_musepackgain.moc"
