
#include "soxfilterglobal.h"

#include "soundkonverter_filter_sox.h"
#include "../../core/conversionoptions.h"
#include "soxfilteroptions.h"
#include "soxfilterwidget.h"
#include "soxcodecwidget.h"

#include <KMessageBox>

soundkonverter_filter_sox::soundkonverter_filter_sox( QObject *parent, const QStringList& args  )
    : FilterPlugin( parent )
{
    Q_UNUSED(args)

    binaries["sox"] = "";

    allCodecs += "wav";
}

soundkonverter_filter_sox::~soundkonverter_filter_sox()
{}

QString soundkonverter_filter_sox::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_filter_sox::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    QStringList filter;
    filter += "normalize";

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["sox"] != "" );
    newTrunk.problemInfo = standardMessage( "filter,backend", filter.join(", "), "multiple", "sox" ) + "\n" + standardMessage( "install_opensource_backend", "sox" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "flac";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["sox"] != "" );
    newTrunk.problemInfo = standardMessage( "filter,backend", filter.join(", "), "multiple", "sox" ) + "\n" + standardMessage( "install_opensource_backend", "sox" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_filter_sox::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_filter_sox::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_filter_sox::hasInfo()
{
    return false;
}

void soundkonverter_filter_sox::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

FilterWidget *soundkonverter_filter_sox::newFilterWidget()
{
    SoxFilterWidget *widget = new SoxFilterWidget();
    if( lastUsedFilterOptions )
    {
        widget->setCurrentFilterOptions( lastUsedFilterOptions );
        delete lastUsedFilterOptions;
        lastUsedFilterOptions = 0;
    }
    return qobject_cast<FilterWidget*>(widget);
}

QWidget *soundkonverter_filter_sox::newCodecWidget()
{
    SoxCodecWidget *widget = new SoxCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_filter_sox::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
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

QStringList soundkonverter_filter_sox::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED( tags );
    Q_UNUSED( replayGain );

    if( !_conversionOptions )
        return QStringList();

    QStringList command;

    foreach( FilterOptions *_filterOptions,_conversionOptions->filterOptions )
    {
        if( _filterOptions->pluginName == global_plugin_name )
        {
            SoxFilterOptions *filterOptions = dynamic_cast<SoxFilterOptions*>(_filterOptions);
            if( filterOptions->data.normalize )
            {
                command += binaries["sox"];
                command += "--norm=" + QString::number(filterOptions->data.normalizeVolume);
                if( inputFile.isEmpty() )
                {
                    command += "-t";
                    command += inputCodec;
                }
                command += "\"" + escapeUrl(inputFile) + "\"";
                if( outputFile.isEmpty() )
                {
                    command += "-t";
                    command += outputCodec;
                }
                command += "\"" + escapeUrl(outputFile) + "\"";
            }
        }
    }

    return command;
}

float soundkonverter_filter_sox::parseOutput( const QString& output )
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

FilterOptions *soundkonverter_filter_sox::filterOptionsFromXml( QDomElement filterOptions )
{
    KMessageBox::information(0,"soundkonverter_filter_sox::filterOptionsFromXml");
    SoxFilterOptions *options = new SoxFilterOptions();
    options->fromXml( filterOptions );
    return options;
}


#include "soundkonverter_filter_sox.moc"

