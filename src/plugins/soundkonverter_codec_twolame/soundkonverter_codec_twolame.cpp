
#include "twolamecodecglobal.h"

#include "soundkonverter_codec_twolame.h"
#include "twolamecodecwidget.h"


#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLocale>
#include <KComboBox>
#include <KDialog>
#include <QSpinBox>
#include <QGroupBox>
#include <QSlider>

soundkonverter_codec_twolame::soundkonverter_codec_twolame( QObject *parent, const QVariantList& args  )
    : CodecPlugin( parent )
{
    Q_UNUSED(args)

    binaries["twolame"] = "";

    allCodecs += "mp2";
    allCodecs += "wav";
}

soundkonverter_codec_twolame::~soundkonverter_codec_twolame()
{}

QString soundkonverter_codec_twolame::name() const
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_twolame::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "mp2";
    newTrunk.rating = 100;
    newTrunk.enabled = ( binaries["twolame"] != "" );
    newTrunk.problemInfo = standardMessage( "encode_codec,backend", "mp2", "twolame" );
    newTrunk.data.hasInternalReplayGain = false;
    table.append( newTrunk );

    return table;
}

bool soundkonverter_codec_twolame::isConfigSupported( ActionType action, const QString& codecName )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return false;
}

void soundkonverter_codec_twolame::showConfigDialog( ActionType action, const QString& codecName, QWidget *parent )
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)
    Q_UNUSED(parent)

//     KDialog *dialog = new KDialog( parent );
//     dialog->setCaption( i18n("Configure %1",*global_plugin_name) );
//     dialog->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );

//     QWidget *widget = new QWidget( dialog );


//     dialog->setMainWidget( widget );
//     connect( dialog, SIGNAL( applyClicked() ), widget, SLOT( save() ) );
//     connect( dialog, SIGNAL( okClicked() ), widget, SLOT( save() ) );
//     connect( widget, SIGNAL( changed( bool ) ), dialog, SLOT( enableButtonApply( bool ) ) );

//     dialog->enableButtonApply( false );
//     dialog->show();
}

bool soundkonverter_codec_twolame::hasInfo()
{
    return true;
}

void soundkonverter_codec_twolame::showInfo( QWidget *parent )
{
    KDialog *dialog = new KDialog( parent );
    dialog->setCaption( i18n("About %1",*global_plugin_name) );
    dialog->setButtons( KDialog::Ok );

    QLabel *widget = new QLabel( dialog );

    widget->setText( i18n("TwoLame is a free MP2 encoder.\nYou can get it at: http://www.twolame.org") );

    dialog->setMainWidget( widget );

    dialog->enableButtonApply( false );
    dialog->show();
}

CodecWidget *soundkonverter_codec_twolame::newCodecWidget()
{
    TwoLameCodecWidget *widget = new TwoLameCodecWidget();
    return qobject_cast<CodecWidget*>(widget);
}

int soundkonverter_codec_twolame::convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    QStringList command = convertCommand( inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain );
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

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

QStringList soundkonverter_codec_twolame::convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, const ConversionOptions *_conversionOptions, TagData *tags, bool replayGain )
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    QStringList command;
    const ConversionOptions *conversionOptions = _conversionOptions;

    if( outputCodec == "mp2" )
    {
        command += binaries["twolame"];
        if( inputFile.isEmpty() )
        {
            command += "-r";
        }
        if( conversionOptions->qualityMode == ConversionOptions::Quality )
        {
            command += "-V";
            command += QString::number(conversionOptions->quality);
        }
        else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
        {
            command += "-b";
            command += QString::number(conversionOptions->bitrate);
        }
        if( conversionOptions->pluginName == name() )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_twolame::parseOutput( const QString& output )
{
    // decoding
    // Frame#  1398/8202   256 kbps  L  R (...)

    // encoding
    // \r  3600/3696   (97%)|    0:05/    0:05|    0:05/    0:05|   18.190x|    0:00

    QString data = output;
    QString frame, count;

    if( output.contains("Frame#") )
    {
        data.remove( 0, data.indexOf("Frame#")+7 );
        frame = data.left( data.indexOf("/") );
        data.remove( 0, data.indexOf("/")+1 );
        count = data.left( data.indexOf(" ") );
        return frame.toFloat()/count.toFloat()*100.0f;
    }
    if( output.contains("%") )
    {
        frame = data.left( data.indexOf("/") );
        frame.remove( 0, frame.lastIndexOf(" ")+1 );
        data.remove( 0, data.indexOf("/")+1 );
        count = data.left( data.indexOf(" ") );
        return frame.toFloat()/count.toFloat()*100.0f;
    }
    /*if( output.contains("%") )
    {
        data.remove( 0, data.indexOf("(")+1 );
        data.remove( data.indexOf("%"), data.length()-data.indexOf("%") );
        return data.toFloat();
    }*/

    return -1;
}

K_PLUGIN_FACTORY(codec_twolame, registerPlugin<soundkonverter_codec_twolame>();)

#include "soundkonverter_codec_twolame.moc"
