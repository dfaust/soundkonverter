
#include "musepackcodecglobal.h"

#include "soundkonverter_codec_musepack.h"
#include "musepackconversionoptions.h"
#include "musepackcodecwidget.h"


soundkonverter_codec_musepack::soundkonverter_codec_musepack( QObject *parent, const QStringList& args  )
    : CodecPlugin( parent )
{
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

QList<ConversionPipeTrunk> soundkonverter_codec_musepack::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "musepack";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mppenc"] != "" );
    newTrunk.problemInfo = i18n("In order to encode musepack files, you need to install 'mppenc'.\nYou can get it at http://www.musepack.net");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    newTrunk.codecFrom = "musepack";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["mppdec"] != "" );
    newTrunk.problemInfo = i18n("In order to decode musepack files, you need to install 'mppdec'.\nYou can get it at http://www.musepack.net");
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

BackendPlugin::FormatInfo soundkonverter_codec_musepack::formatInfo( const QString& codecName )
{
    if( codecName == "musepack" )
    {
        BackendPlugin::FormatInfo info;
        info.codecName = codecName;
        info.lossless = false;
        info.description = i18n("Musepack is a free and lossy file format based on mp2 and optimized for high quality."); // http://en.wikipedia.org/wiki/Musepack
        info.mimeTypes.append( "audio/x-musepack" );
        info.mimeTypes.append( "audio/musepack" );
        info.extensions.append( "mpc" );
        info.extensions.append( "mp+" );
        info.extensions.append( "mpp" );
        return info;
    }

    return BackendPlugin::formatInfo( codecName );
}

// QString soundkonverter_codec_musepack::getCodecFromFile( const KUrl& filename, const QString& mimeType )
// {
//     if( mimeType == "application/x-ogg" || mimeType == "application/ogg" || mimeType == "audio/ogg" || mimeType == "audio/vorbis" || mimeType == "audio/x-vorbis+ogg" )
//     {
//         return "ogg vorbis";
//     }
//     else if( mimeType == "audio/x-wav" || mimeType == "audio/wav" )
//     {
//         return "wav";
//     }
//     else if( mimeType == "application/octet-stream" )
//     {
//         if( filename.url().endsWith(".ogg") ) return "ogg vorbis";
//         if( filename.url().endsWith(".wav") ) return "wav";
//     }
// 
//     return "";
// }

bool soundkonverter_codec_musepack::isConfigSupported( ActionType action, const QString& format )
{
    return false;
}

void soundkonverter_codec_musepack::showConfigDialog( ActionType action, const QString& format, QWidget *parent )
{}

bool soundkonverter_codec_musepack::hasInfo()
{
    return false;
}

void soundkonverter_codec_musepack::showInfo( QWidget *parent )
{}

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
    if( command.isEmpty() ) return -1;

    CodecPluginItem *newItem = new CodecPluginItem( this );
    newItem->id = lastId++;
    newItem->process = new KProcess( newItem );
    newItem->process->setOutputChannelMode( KProcess::MergedChannels );
    connect( newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()) );
    connect( newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)) );

    newItem->process->clearProgram();
    newItem->process->setShellCommand( command.join(" ") );
    newItem->process->start();

    emit log( newItem->id, command.join(" ") );

    backendItems.append( newItem );
    return newItem->id;
}

QStringList soundkonverter_codec_musepack::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    if( !_conversionOptions ) return QStringList();
    
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
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
    }
    else
    {
        command += binaries["mppdec"];
        command += "\"" + inputFile.toLocalFile() + "\"";
        command += "\"" + outputFile.toLocalFile() + "\"";
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


#include "soundkonverter_codec_musepack.moc"
