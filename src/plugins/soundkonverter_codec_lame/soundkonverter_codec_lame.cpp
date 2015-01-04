
#include "lamecodecglobal.h"
#include "ui_lameconfigdialog.h"

#include "soundkonverter_codec_lame.h"
#include "lameconversionoptions.h"
#include "lamecodecwidget.h"

#include <KLocalizedString>
#include <QMessageBox>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

soundkonverter_codec_lame::soundkonverter_codec_lame() :
    CodecPlugin()
{
    binaries["lame"] = "";

    allCodecs += "mp3";
    allCodecs += "mp2";
    allCodecs += "wav";

    KConfigGroup group(KSharedConfig::openConfig(), "Plugin-" global_plugin_name);
    configVersion = group.readEntry("configVersion", 0);
    compressionLevel = group.readEntry("compressionLevel", 2);
    stereoMode = group.readEntry("stereoMode", "automatic");
}

soundkonverter_codec_lame::~soundkonverter_codec_lame()
{
}

QString soundkonverter_codec_lame::name()
{
    return global_plugin_name;
}

QList<ConversionPipeTrunk> soundkonverter_codec_lame::codecTable()
{
    QList<ConversionPipeTrunk> table;
    ConversionPipeTrunk newTrunk;

    const bool lameFound = !binaries["lame"].isEmpty();

    newTrunk.codecFrom = "wav";
    newTrunk.codecTo = "mp3";
    newTrunk.rating = 100;
    newTrunk.enabled = lameFound;
    newTrunk.problemInfo = standardMessage("encode_codec,backend", "mp3", "lame") + "\n" + standardMessage("install_patented_backend", "lame");
    newTrunk.data.hasInternalReplayGain = false;
    table.append(newTrunk);

    newTrunk.codecFrom = "mp3";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 100;
    newTrunk.enabled = lameFound;
    newTrunk.problemInfo = standardMessage("decode_codec,backend", "mp3", "lame") + "\n" + standardMessage("install_patented_backend", "lame");
    newTrunk.data.hasInternalReplayGain = false;
    table.append(newTrunk);

    newTrunk.codecFrom = "mp3";
    newTrunk.codecTo = "mp3";
    newTrunk.rating = 100;
    newTrunk.enabled = lameFound;
    newTrunk.problemInfo = standardMessage("transcode_codec,backend", "mp3", "lame") + "\n" + standardMessage("install_patented_backend", "lame");
    newTrunk.data.hasInternalReplayGain = false;
    table.append(newTrunk);

    newTrunk.codecFrom = "mp2";
    newTrunk.codecTo = "wav";
    newTrunk.rating = 70;
    newTrunk.enabled = lameFound;
    newTrunk.problemInfo = standardMessage("decode_codec,backend", "mp2", "lame") + "\n" + standardMessage("install_patented_backend", "lame");
    newTrunk.data.hasInternalReplayGain = false;
    table.append(newTrunk);

    newTrunk.codecFrom = "mp2";
    newTrunk.codecTo = "mp3";
    newTrunk.rating = 70;
    newTrunk.enabled = lameFound;
    newTrunk.problemInfo = standardMessage("transcode_codec,backend", "mp2/mp3", "lame") + "\n" + standardMessage("install_patented_backend", "lame");
    newTrunk.data.hasInternalReplayGain = false;
    table.append(newTrunk);

    return table;
}

bool soundkonverter_codec_lame::isConfigSupported(ActionType action, const QString& codecName)
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    return true;
}

void soundkonverter_codec_lame::showConfigDialog(ActionType action, const QString& codecName, QWidget *parent)
{
    Q_UNUSED(action)
    Q_UNUSED(codecName)

    if( !configDialog )
    {
        configDialog = new QDialog(parent);
        configDialog->setAttribute(Qt::WA_DeleteOnClose);

        configDialogUi = new Ui::LameConfigDialog();

        configDialogUi->setupUi(configDialog);

        configDialogUi->compressionLevelSlider->setToolTip(i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.\nA value of %3 is recommended.", 9, 0, 2));
        configDialogUi->compressionLevelSpinBox->setToolTip(i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.\nA value of %3 is recommended.", 9, 0, 2));

        configDialogUi->stereoModeComboBox->addItem(i18n("Automatic"),           "automatic");
        configDialogUi->stereoModeComboBox->addItem(i18n("Joint Stereo"),        "joint stereo");
        configDialogUi->stereoModeComboBox->addItem(i18n("Simple Stereo"),       "simple stereo");
        configDialogUi->stereoModeComboBox->addItem(i18n("Forced Joint Stereo"), "forced joint stereo");
        configDialogUi->stereoModeComboBox->addItem(i18n("Dual Mono"),           "dual mono");

        connect(configDialogUi->compressionLevelSlider, SIGNAL(valueChanged(int)),  this, SLOT(configDialogCompressionLevelSliderChanged(int)));
        connect(configDialogUi->compressionLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(configDialogCompressionLevelSpinBoxChanged(int)));

        connect(configDialogUi->defaultsButton, SIGNAL(clicked()), this, SLOT(configDialogDefault()));
        connect(configDialogUi->okButton, SIGNAL(clicked()),       this, SLOT(configDialogSave()));
        connect(configDialogUi->cancelButton, SIGNAL(clicked()),   configDialog, SLOT(close()));
    }

    configDialogUi->compressionLevelSpinBox->setValue(compressionLevel);
    configDialogUi->stereoModeComboBox->setCurrentIndex(configDialogUi->stereoModeComboBox->findData(stereoMode));
    configDialog->show();
}

void soundkonverter_codec_lame::configDialogSave()
{
    if( configDialog )
    {
        compressionLevel = configDialogUi->compressionLevelSpinBox->value();
        stereoMode = configDialogUi->stereoModeComboBox->itemData(configDialogUi->stereoModeComboBox->currentIndex()).toString();

        KConfigGroup group(KSharedConfig::openConfig(), "Plugin-" global_plugin_name);
        group.writeEntry("compressionLevel", compressionLevel);
        group.writeEntry("stereoMode", stereoMode);

        configDialog->close();
    }
}

void soundkonverter_codec_lame::configDialogDefault()
{
    if( configDialog )
    {
        configDialogUi->compressionLevelSpinBox->setValue(2);
        configDialogUi->stereoModeComboBox->setCurrentIndex(configDialogUi->stereoModeComboBox->findData("automatic"));
    }
}

void soundkonverter_codec_lame::configDialogCompressionLevelSliderChanged(int quality)
{
    if( configDialog )
    {
        configDialogUi->compressionLevelSpinBox->setValue(9 - quality);
    }
}

void soundkonverter_codec_lame::configDialogCompressionLevelSpinBoxChanged(int quality)
{
    if( configDialog )
    {
        configDialogUi->compressionLevelSlider->setValue(9 - quality);
    }
}

bool soundkonverter_codec_lame::hasInfo()
{
    return true;
}

void soundkonverter_codec_lame::showInfo(QWidget *parent)
{
    QMessageBox messageBox(parent);
    messageBox.setTextFormat(Qt::RichText);
    messageBox.setWindowTitle(i18n("About %1").arg(global_plugin_name));
    messageBox.setText(i18n("LAME is a free high quality MP3 encoder.<br>You can get it at: <a href=\"http://lame.sourceforge.net\">http://lame.sourceforge.net</a>"));
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.exec();
}

CodecWidget *soundkonverter_codec_lame::newCodecWidget()
{
    LameCodecWidget *widget = new LameCodecWidget();
    return dynamic_cast<CodecWidget*>(widget);
}

unsigned int soundkonverter_codec_lame::convert(const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain)
{
    QStringList command = convertCommand(inputFile, outputFile, inputCodec, outputCodec, _conversionOptions, tags, replayGain);
    if( command.isEmpty() )
        return BackendPlugin::UnknownError;

    CodecPluginItem *newItem = new CodecPluginItem(this);
    newItem->id = lastId++;
    newItem->process = new QProcess(newItem);
    newItem->process->setProcessChannelMode(QProcess::MergedChannels);
    connect(newItem->process, SIGNAL(readyRead()), this, SLOT(processOutput()));
    connect(newItem->process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExit(int,QProcess::ExitStatus)));

    newItem->process->start(command.join(" "));

    logCommand(newItem->id, command.join(" "));

    backendItems.append(newItem);
    return newItem->id;
}

QStringList soundkonverter_codec_lame::convertCommand(const QUrl& inputFile, const QUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags, bool replayGain)
{
    Q_UNUSED(inputCodec)
    Q_UNUSED(tags)
    Q_UNUSED(replayGain)

    if( !_conversionOptions )
        return QStringList();

    if( inputFile.isEmpty() )
        return QStringList();

    QStringList command;
    ConversionOptions *conversionOptions = _conversionOptions;
    LameConversionOptions *lameConversionOptions = 0;
    if( conversionOptions->pluginName == name() )
    {
        lameConversionOptions = static_cast<LameConversionOptions*>(conversionOptions);
    }

    if( outputCodec == "mp3" )
    {
        command += binaries["lame"];
        command += "--nohist";
        command += "--pad-id3v2";
        if( conversionOptions->pluginName == name() )
        {
            command += "-q";
            command += QString::number((int)conversionOptions->compressionLevel);
        }
//         if( conversionOptions->replaygain && replayGain )
//         {
//             command += "--replaygain-accurate";
//         }
//         else
//         {
//             command += "--noreplaygain";
//         }
        if( conversionOptions->pluginName != name() || !conversionOptions->cmdArguments.contains("replaygain") )
        {
            command += "--noreplaygain";
        }
        if( lameConversionOptions && lameConversionOptions->data.preset != LameConversionOptions::Data::UserDefined )
        {
            command += "--preset";
            if( lameConversionOptions->data.presetFast )
            {
                command += "fast";
            }
            if( lameConversionOptions->data.preset == LameConversionOptions::Data::Medium )
            {
                command += "medium";
            }
            else if( lameConversionOptions->data.preset == LameConversionOptions::Data::Standard )
            {
                command += "standard";
            }
            else if( lameConversionOptions->data.preset == LameConversionOptions::Data::Extreme )
            {
                command += "extreme";
            }
            else if( lameConversionOptions->data.preset == LameConversionOptions::Data::Insane )
            {
                command += "insane";
            }
            else if( lameConversionOptions->data.preset == LameConversionOptions::Data::SpecifyBitrate )
            {
                if( lameConversionOptions->data.presetBitrateCbr )
                {
                    command += "cbr";
                }
                command += QString::number(lameConversionOptions->data.presetBitrate);
            }
        }
        else
        {
            if( conversionOptions->qualityMode == ConversionOptions::Quality )
            {
                if( conversionOptions->pluginName != name() || !conversionOptions->cmdArguments.contains("--vbr-old") )
                {
                    command += "--vbr-new";
                }
                command += "-V";
                command += QString::number(conversionOptions->quality);
            }
            else if( conversionOptions->qualityMode == ConversionOptions::Bitrate )
            {
                if( conversionOptions->bitrateMode == ConversionOptions::Abr )
                {
                    command += "--abr";
                    command += QString::number(conversionOptions->bitrate);
                }
                else if( conversionOptions->bitrateMode == ConversionOptions::Cbr )
                {
                    command += "--cbr";
                    command += "-b";
                    command += QString::number(conversionOptions->bitrate);
                }
            }
        }
        if( stereoMode != "automatic" )
        {
            command += "-m";
            if( stereoMode == "joint stereo" )
            {
                command += "j";
            }
            else if( stereoMode == "simple stereo" )
            {
                command += "s";
            }
            else if( stereoMode == "forced joint stereo" )
            {
                command += "f";
            }
            else if( stereoMode == "dual mono" )
            {
                command += "d";
            }
        }
        if( conversionOptions->pluginName == name() )
        {
            command += conversionOptions->cmdArguments;
        }
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }
    else
    {
        command += binaries["lame"];
        command += "--decode";
        command += "\"" + escapeUrl(inputFile) + "\"";
        command += "\"" + escapeUrl(outputFile) + "\"";
    }

    return command;
}

float soundkonverter_codec_lame::parseOutput(const QString& output)
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

ConversionOptions *soundkonverter_codec_lame::conversionOptionsFromXml(QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements)
{
    LameConversionOptions *options = new LameConversionOptions();
    options->fromXml(conversionOptions, filterOptionsElements);
    return options;
}
