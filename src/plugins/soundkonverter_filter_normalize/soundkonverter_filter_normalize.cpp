
#include "normalizefilterglobal.h"

#include "soundkonverter_filter_normalize.h"
#include "../../core/conversionoptions.h"
#include "normalizefilteroptions.h"
#include "normalizefilterwidget.h"

#include <QFile>


soundkonverter_filter_normalize::soundkonverter_filter_normalize( QObject *parent, const QStringList& args  )
    : FilterPlugin( parent )
{
    Q_UNUSED(args)

    binaries["normalize"] = "";

    allCodecs += "wav";
}

soundkonverter_filter_normalize::~soundkonverter_filter_normalize()
{}

QString soundkonverter_filter_normalize::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_filter_normalize::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["normalize"] != "" );
    newTrunk.problemInfo = standardMessage( "filter,backend", "normalize", "normalize" ) + "\n" + standardMessage( "install_opensource_backend", "normalize" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
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

FilterWidget *soundkonverter_filter_normalize::newFilterWidget()
{
    NormalizeFilterWidget *widget = new NormalizeFilterWidget();
    if( lastUsedFilterOptions )
    {
        widget->setCurrentFilterOptions( lastUsedFilterOptions );
        delete lastUsedFilterOptions;
        lastUsedFilterOptions = 0;
    }
    return qobject_cast<FilterWidget*>(widget);
}

CodecWidget *soundkonverter_filter_normalize::newCodecWidget()
{
//     CodecWidget *widget = new CodecWidget();
//     if( lastUsedConversionOptions )
//     {
//         widget->setCurrentConversionOptions( lastUsedConversionOptions );
//         delete lastUsedConversionOptions;
//         lastUsedConversionOptions = 0;
//     }
//     return qobject_cast<CodecWidget*>(widget);
return 0;
}

unsigned int soundkonverter_filter_normalize::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

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

QStringList soundkonverter_filter_normalize::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED( inputCodec );
    Q_UNUSED( outputCodec );
    Q_UNUSED( tags );
    Q_UNUSED( replayGain );

    if( !_conversionOptions )
        return QStringList();

    if( inputFile.isEmpty() || outputFile.isEmpty() )
        return QStringList();

    QStringList command;

    foreach( FilterOptions *_filterOptions,_conversionOptions->filterOptions )
    {
        if( _filterOptions->pluginName == global_plugin_name )
        {
            NormalizeFilterOptions *filterOptions = dynamic_cast<NormalizeFilterOptions*>(_filterOptions);
            if( filterOptions->data.normalize )
            {
                command += binaries["normalize"];
                command += "\"" + escapeUrl(outputFile) + "\"";

                if( !command.isEmpty() )
                    QFile::copy( inputFile.toLocalFile(), outputFile.toLocalFile() );
            }
        }
    }

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

FilterOptions *soundkonverter_filter_normalize::filterOptionsFromXml( QDomElement filterOptions )
{
    NormalizeFilterOptions *options = new NormalizeFilterOptions();
    options->fromXml( filterOptions );
    return options;
}


#include "soundkonverter_filter_normalize.moc"

