
#include "musepackcodecglobal.h"

#include "soundkonverter_codec_musepack.h"
#include "musepackconversionoptions.h"
#include "musepackcodecwidget.h"

#include <KStandardDirs>
#include <QFile>


soundkonverter_codec_musepack::soundkonverter_codec_musepack( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["mppenc"] = "";
    binaries["mppdec"] = "";

    allCodecs += "musepack";
    allCodecs += "wav";
}

soundkonverter_codec_musepack::~soundkonverter_codec_musepack()
{}

QString soundkonverter_codec_musepack::name()
{
    return global_plugin_name;
}

void soundkonverter_codec_musepack::scanForBackends( const QStringList& directoryList )
{
    binaries["mppenc"] = KStandardDirs::findExe( "mppenc" );
    if( binaries["mppenc"].isEmpty() )
        binaries["mppenc"] = KStandardDirs::findExe( "mpcenc" );

    if( binaries["mppenc"].isEmpty() )
    {
        for( QList<QString>::const_iterator b = directoryList.begin(); b != directoryList.end(); ++b )
        {
            if( QFile::exists((*b) + "/mppenc") )
            {
                binaries["mppenc"] = (*b) + "/mppenc";
                break;
            }
            else if( QFile::exists((*b) + "/mpcenc") )
            {
                binaries["mppenc"] = (*b) + "/mpcenc";
                break;
            }
        }
    }

    binaries["mppdec"] = KStandardDirs::findExe( "mppdec" );
    if( binaries["mppdec"].isEmpty() )
        binaries["mppdec"] = KStandardDirs::findExe( "mpcdec" );

    if( binaries["mppdec"].isEmpty() )
    {
        for( QList<QString>::const_iterator b = directoryList.begin(); b != directoryList.end(); ++b )
        {
            if( QFile::exists((*b) + "/mppdec") )
            {
                binaries["mppdec"] = (*b) + "/mppdec";
                break;
            }
            else if( QFile::exists((*b) + "/mpcdec") )
            {
                binaries["mppdec"] = (*b) + "/mpcdec";
                break;
            }
        }
    }
}

QList<ConversionPipeTrunk> soundkonverter_codec_musepack::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "musepack";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mppenc"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "musepack", "mppenc" ) + "\n" + standardMessage( "install_website_backend,url", "mppenc", "http://www.musepack.net" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "musepack";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mppdec"] != "" );
    newTrunk.problemInfo = standardMessage( "decode_codec,backend", "musepack", "mppdec" ) + "\n" + standardMessage( "install_website_backend,url", "mppdec", "http://www.musepack.net" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_musepack::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_musepack::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)
}

bool soundkonverter_codec_musepack::hasInfo()
{
    return false;
}

void soundkonverter_codec_musepack::showInfo( QWidget *parent )
{
    Q_UNUSED(parent)
}

QWidget *soundkonverter_codec_musepack::newCodecWidget()
{
    MusePackCodecWidget *widget = new MusePackCodecWidget();
    if( lastUsedConversionOptions )
    {
        widget->setCurrentConversionOptions( lastUsedConversionOptions );
        delete lastUsedConversionOptions;
        lastUsedConversionOptions = 0;
    }
    return qobject_cast<QWidget*>(widget);
}

int soundkonverter_codec_musepack::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return -1;

    CodecPluginItem *newItem = new CodecPluginItem( this );
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

QStringList soundkonverter_codec_musepack::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;
    MusePackConversionOptions *musepackConversionOptions = 0;
    if( conversionOptions->pluginName == name() )
    {
        musepackConversionOptions = static_cast<MusePackConversionOptions*>(conversionOptions);
    }

    if( outputCodec == "musepack" )
    {
        command += binaries["mppenc"];
        if( musepackConversionOptions && musepackConversionOptions->data.preset != MusePackConversionOptions::Data::UserDefined )
        {
            if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Telephone )
            {
                command += "--telephone";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Thumb )
            {
                command += "--thumb";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Radio )
            {
                command += "--radio";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Standard )
            {
                command += "--standard";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Extreme )
            {
                command += "--extreme";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Insane )
            {
                command += "--insane";
            }
            else if( musepackConversionOptions->data.preset == MusePackConversionOptions::Data::Braindead )
            {
                command += "--braindead";
            }
        }
        else
        {
            command += "--quality";
            command += QString::number(conversionOptions->quality);
            if( conversionOptions->pluginName == name() )
            {
                command += conversionOptions->cmdArguments;
            }
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["mppdec"];
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_musepack::parseOutput( const QString& output )
{
    //  47.4  143.7 kbps 23.92x     1:43.3    3:38.1     0:04.3    0:09.1     0:04.8

    QRegExp reg("(\\d+\\.\\d)\\s+\\d+\\.\\d kbps");
    if( output.contains(reg) )
    {
        return reg.cap(1).toFloat();
    }

    return -1;
}

ConversionOptions *soundkonverter_codec_musepack::conversionOptionsFromXml( QDomElement conversionOptions )
{
    MusePackConversionOptions *options = new MusePackConversionOptions();
    options->fromXml( conversionOptions );
    return options;
}


#include "soundkonverter_codec_musepack.moc"
