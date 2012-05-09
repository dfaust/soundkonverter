
#include "cdparanoiaripperglobal.h"

#include "soundkonverter_ripper_cdparanoia.h"

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QSlider>

soundkonverter_ripper_cdparanoia::soundkonverter_ripper_cdparanoia( QObject *parent, const QStringList& args  )
    : RipperPlugin( parent )
{
    Q_UNUSED(args)

    binaries["cdparanoia"] = "";
}

soundkonverter_ripper_cdparanoia::~soundkonverter_ripper_cdparanoia()
{}

QString soundkonverter_ripper_cdparanoia::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_ripper_cdparanoia::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "audio cd";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["cdparanoia"] != "" );
    newTrunk.data.canRipEntireCd = true;
    newTrunk.problemInfo = i18n( "In order to rip audio cds per track or to a single file, you need to install 'cdparanoia'.\n'cdparanoia' is usually shipped with your distribution, the package name can vary." );
    table.append( newTrunk );

    return table;
}

bool soundkonverter_ripper_cdparanoia::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_ripper_cdparanoia::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_ripper_cdparanoia::hasInfo()
{
    return false;
}

void soundkonverter_ripper_cdparanoia::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

int soundkonverter_ripper_cdparanoia::rip( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    QStringList command;

    command += binaries["cdparanoia"];
    command += "-e";
    command += "-d";
    command += device;
    if( track > 0 )
    {
        command += QString::number(track);
    }
    else
    {
        command += "1-" + QString::number(tracks);
    }
    command += "\"" + outputFile.toLocalFile() + "\"";

    RipperPluginItem *newItem = new RipperPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    logCommand( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_ripper_cdparanoia::ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile )
{
    Q_UNUSED(device)
    Q_UNUSED(track)
    Q_UNUSED(tracks)
    Q_UNUSED(outputFile)

    return QStringList();
}

float soundkonverter_ripper_cdparanoia::parseOutput( const QString& output, int *fromSector, int *toSector )
{
    // Ripping from sector       0 (track  1 [0:00.00])
    //           to sector   16361 (track  1 [3:38.11])

    // ##: -2 [wrote] @ 19242887\n

    if( output.contains("sector") )
    {
        if( fromSector && output.contains("from sector") )
        {
            QString data = output;
            data.remove( 0, data.indexOf("from sector") + 11 );
            data = data.left( data.indexOf("(") );
            data = data.simplified();
            *fromSector = data.toInt();
        }
        if( toSector && output.contains("to sector") )
        {
            QString data = output;
            data.remove( 0, data.indexOf("to sector") + 9 );
            data = data.left( data.indexOf("(") );
            data = data.simplified();
            *toSector = data.toInt();
        }
        return -1;
    }

    if( output == "" || !output.contains("@") ) return -1;
    if( !output.contains("[wrote] @") ) return 0;

    QString data = output;
    data.remove( 0, data.indexOf("[wrote] @") + 9 );
    data = data.left( data.indexOf("\n") );
    data = data.simplified();
    return data.toFloat() / 1176;
}

float soundkonverter_ripper_cdparanoia::parseOutput( const QString& output )
{
    return parseOutput( output, 0, 0 );
}

void soundkonverter_ripper_cdparanoia::processOutput()
{
    RipperPluginItem *pluginItem;
    float progress;
    for( int i=0; i<backendItems.size(); i++ )
    {
        if( backendItems.at(i)->process == QObject::sender() )
        {
            QString output = backendItems.at(i)->process->readAllStandardOutput().data();
            pluginItem = qobject_cast<RipperPluginItem*>(backendItems.at(i));

            progress = parseOutput( output, &pluginItem->data.fromSector, &pluginItem->data.toSector );

            if( progress == -1 && !output.simplified().isEmpty() )
                logOutput( backendItems.at(i)->id, output );

            progress = (progress-pluginItem->data.fromSector) * 100 / (pluginItem->data.toSector-pluginItem->data.fromSector);

            if( progress > backendItems.at(i)->progress )
                backendItems.at(i)->progress = progress;

            return;
        }
    }
}

#include "soundkonverter_ripper_cdparanoia.moc"
