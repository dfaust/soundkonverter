
#include "normalizefilterglobal.h"

#include "soundkonverter_filter_normalize.h"
#include "../../core/conversionoptions.h"
#include "normalizefilterwidget.h"

#include <QFile>


soundkonverter_filter_normalize::soundkonverter_filter_normalize( QObject *parent, const QStringList& args  )
    : FilterPlugin( parent )
{
    Q_UNUSED(args)

    binaries["normalize"] = "";
}

soundkonverter_filter_normalize::~soundkonverter_filter_normalize()
{}

QString soundkonverter_filter_normalize::name()
{
    return global_plugin_name;
}

bool soundkonverter_filter_normalize::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_filter_normalize::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_filter_normalize::hasInfo()
{
    return false;
}

void soundkonverter_filter_normalize::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_filter_normalize::newFilterWidget()
{
    NormalizeFilterWidget *widget = new NormalizeFilterWidget();
    if( lastUsedFilterOptions )
    {
        widget->setCurrentFilterOptions( lastUsedFilterOptions );
        delete lastUsedFilterOptions;
        lastUsedFilterOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_filter_normalize::filter( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions )
{
    QStringList command = filterCommand( inputFile, outputFile, _filterOptions );
    if( command.isEmpty() )
        return -1;

    FilterPluginItem *newItem = new FilterPluginItem( this );
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

QStringList soundkonverter_filter_normalize::filterCommand( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions )
{
    if( !_filterOptions )
        return QStringList();

    if( inputFile.isEmpty() || outputFile.isEmpty() )
        return QStringList();

    QStringList command;
//     NormalizeFilterOptions *filterOptions = _filterOptions;

    command += binaries["normalize"];
    command += "\"" + escapeUrl(inputFile) + "\"";

    if( !command.isEmpty() )
        QFile::copy( inputFile.toLocalFile(), outputFile.toLocalFile() );

    return command;
}

float soundkonverter_filter_normalize::parseOutput( const QString& output )
{
    Q_UNUSED( output );

//     // 01-Unknown.wav: 98% complete, ratio=0,479    // encode
//     // 01-Unknown.wav: 27% complete                 // decode
//
//     QRegExp regEnc("(\\d+)% complete");
//     if( output.contains(regEnc) )
//     {
//         return (float)regEnc.cap(1).toInt();
//     }
//
    return -1;
}


#include "soundkonverter_filter_normalize.moc"

