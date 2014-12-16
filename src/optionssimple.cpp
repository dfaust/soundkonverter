
#include "optionssimple.h"
#include "ui_optionssimple.h"
#include "config.h"
#include "outputdirectory.h"
#include "global.h"
#include "codecproblems.h"

#include <KLocalizedString>
#include <QMessageBox>
#include <QFile>
#include <QStandardPaths>

// FIXME when changing the output directory, check if the profile is a user defined and set it to 'User defined', if it is

// TODO hide lossless when not available

OptionsSimple::OptionsSimple(QWidget *parent) :
    QWidget( parent ),
    ui(new Ui::OptionsSimple)
{
    ui->setupUi(this);

//     connect(ui->formatComboBox, SIGNAL(activated(int)), this, SLOT(formatChanged()));
    connect(ui->deleteProfilePushButton, SIGNAL(clicked()), this, SLOT(profileRemove()));
    connect(ui->infoProfilePushButton, SIGNAL(clicked()), this, SLOT(profileInfo()));

    connect(ui->formatComboBox, SIGNAL(activated(int)), this, SLOT(somethingChanged()));
    connect(ui->infoFormatPushButton, SIGNAL(clicked()), this, SLOT(formatInfo()));
//     connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()));

//     connect(ui->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(ui->outputDirectoryChanged()));
//     connect(ui->outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(ui->outputDirectoryChanged()));

//     const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();
//
//     grid->setMargin( fontHeight );
//     grid->setSpacing( fontHeight );
//
//     topBoxQuality->addSpacing( 0.25*fontHeight );
//     ui->deleteProfilePushButton->hide();

//     topBoxFormat->addSpacing( 0.25*fontHeight );
//     QLabel *formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("More formats...") + "</a>", this );

//
//     lEstimSize = new QLabel( QString(QChar(8776))+"? B / min." );
//     lEstimSize->hide(); // hide for now because most plugins report inaccurate data
//
//     connect(ui->replayGainCheckBox, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
}

OptionsSimple::~OptionsSimple()
{
}

void OptionsSimple::init(Config *config, const QString &text)
{
    this->config = config;

    ui->infoLabel->setText(text);

    updateProfiles();
}

void OptionsSimple::setReplayGainEnabled(bool enabled, const QString& toolTip)
{
    ui->replayGainCheckBox->setEnabled(enabled);
    ui->replayGainCheckBox->setToolTip(toolTip);

    if( !enabled )
    {
        QPalette notificationPalette = ui->replayGainCheckBox->palette();
        notificationPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(174,127,130));
        ui->replayGainCheckBox->setPalette(notificationPalette);
    }
}

void OptionsSimple::setReplayGainChecked(bool enabled)
{
    ui->replayGainCheckBox->setChecked(enabled);
}

QString OptionsSimple::currentProfile()
{
    return ui->profileComboBox->currentText();
}

QString OptionsSimple::currentFormat()
{
    return ui->formatComboBox->currentText();
}

bool OptionsSimple::isReplayGainChecked()
{
    return ui->replayGainCheckBox->isChecked();
}

void OptionsSimple::updateProfiles()
{
    const QString lastProfile = ui->profileComboBox->currentText();
    QStringList sProfile;
    ui->profileComboBox->clear();

    sProfile += i18n("Very low");
    sProfile += i18n("Low");
    sProfile += i18n("Medium");
    sProfile += i18n("High");
    sProfile += i18n("Very high");
    sProfile += i18n("Lossless");
    sProfile += config->customProfiles();
    sProfile += i18n("User defined");
    ui->profileComboBox->addItems( sProfile );

    if( ui->profileComboBox->findText(lastProfile) != -1 )
    {
        ui->profileComboBox->setCurrentIndex(ui->profileComboBox->findText(lastProfile));
    }
    else
    {
        profileChanged();
    }
}

void OptionsSimple::profileInfo()
{
    const QString sProfileString = ui->profileComboBox->currentText();

    QString info;

    if( sProfileString == i18n("Very low") )
    {
        info = i18n("Encoding files with a very low quality can be useful if you have a mobile device where your storage space is limited.\nIt is not recommended to save your music in this quality without a copy of higher quality.");
    }
    else if( sProfileString == i18n("Low") )
    {
        info = i18n("Encoding files with a low quality can be useful if you have a mobile device where your storage space is limited.\nIt is not recommended to save your music in this quality without a copy of higher quality.");
    }
    else if( sProfileString == i18n("Medium") )
    {
        info = i18n("Encoding files with a medium quality is a compromise between quality and file size.\nMost people might not hear much of a difference to the original files but you should only use it to save music if your storage space is limited.");
    }
    else if( sProfileString == i18n("High") )
    {
        info = i18n("Encoding files with a high quality is a good choice if you have enough storage space available. You can use this to save your music.");
    }
    else if( sProfileString == i18n("Very high") )
    {
        info = i18n("Encoding files with a very high quality is fine if you have enough storage space.");
    }
    else if( sProfileString == i18n("Lossless") )
    {
        info = i18n("Encoding files lossless will preserve the quality of the original file.\nIt is only then useful if the original file has a good quality (e.g. audio CD).\nThe file size will be big, so you should only use it if you have enough storage space.");
    }
    else if( sProfileString == i18n("Hybrid") )
    {
        // info = i18n("This produces two files. One lossy compressed playable file and one correction file.\nBoth files together result in a file that is equivalent to the input file.");
    }
    else if( sProfileString == i18n("User defined") )
    {
        info = i18n("You can define your own profile in the \"Detailed\" tab.");
    }
    else // the info button is hidden when showing user defined profiles
    {
        info = i18n("This is a user defined profile.");
    }

    QMessageBox::information(this, info, i18n("Profile info for %1", sProfileString));
}

void OptionsSimple::profileRemove()
{
    const QString profileName = ui->profileComboBox->currentText();

    const int ret = QMessageBox::question(this, i18n("Remove profile?"), i18n("Do you really want to remove the profile: %1").arg(profileName));
    if( ret == QMessageBox::Yes )
    {
        QDomDocument list("soundkonverter_profilelist");

        QFile listFile(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/soundkonverter/profiles.xml");
        if( listFile.open( QIODevice::ReadOnly ) )
        {
            if( list.setContent(&listFile) )
            {
                QDomElement root = list.documentElement();
                if( root.nodeName() == "soundkonverter" && root.attribute("type") == "profilelist" )
                {
                    QDomElement profileElement;
                    QDomNodeList conversionOptionsElements = root.elementsByTagName("conversionOptions");
                    for( int i=0; i<conversionOptionsElements.count(); i++ )
                    {
                        if( conversionOptionsElements.at(i).toElement().attribute("profileName") == profileName )
                        {
                            delete config->data.profiles[profileName];
                            config->data.profiles.remove(profileName);
                            root.removeChild(conversionOptionsElements.at(i));
                            break;
                        }
                    }
                }
            }
            listFile.close();
        }

        if( listFile.open(QIODevice::WriteOnly) )
        {
            updateProfiles();
            emit customProfilesEdited();

            listFile.write(list.toString().toUtf8().data());
            listFile.close();
        }
    }
}

void OptionsSimple::formatInfo()
{
    const QString format = ui->formatComboBox->currentText();
    const QString info = config->pluginLoader()->codecDescription(format);

    if( !info.isEmpty() )
    {
        QMessageBox::information(this, i18n("Format info for %1",format), info);
    }
    else
    {
        QMessageBox::information(this, "soundKonverter", i18n("Sorry, no format information available."));
    }
}

void OptionsSimple::profileChanged()
{
    const QString profile = ui->profileComboBox->currentText();
    const QString lastFormat = ui->formatComboBox->currentText();
    ui->formatComboBox->clear();

    ui->deleteProfilePushButton->hide();
    ui->infoProfilePushButton->show();

    if( profile == i18n("Very low") || profile == i18n("Low") || profile == i18n("Medium") || profile == i18n("High") || profile == i18n("Very high") )
    {
        ui->formatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::Lossy));
    }
    else if( profile == i18n("Lossless") )
    {
        ui->formatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::Lossless));
    }
    else if( profile == i18n("Hybrid") )
    {
        ui->formatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::Hybrid));
    }
    else if( profile == i18n("User defined") )
    {
        ui->formatComboBox->addItems(config->pluginLoader()->formatList(PluginLoader::Encode, PluginLoader::CompressionType(PluginLoader::InferiorQuality | PluginLoader::Lossy | PluginLoader::Lossless | PluginLoader::Hybrid)));
    }
    else
    {
        foreach( const QString& profileName, config->data.profiles.keys() )
        {
            if( profileName == profile )
            {
                ConversionOptions *conversionOptions = config->data.profiles.value(profileName);
                if( conversionOptions )
                {
                    ui->formatComboBox->addItem(conversionOptions->codecName);
                    ui->outputDirectory->setMode((OutputDirectory::Mode)conversionOptions->outputDirectoryMode);
                    ui->outputDirectory->setDirectory(conversionOptions->outputDirectory);
                    ui->replayGainCheckBox->setChecked(conversionOptions->replaygain);
                    ui->deleteProfilePushButton->show();
                    ui->infoProfilePushButton->hide();
                }
                break;
            }
        }
    }

    if( ui->formatComboBox->findText(lastFormat) != -1 )
    {
        ui->formatComboBox->setCurrentIndex(ui->formatComboBox->findText(lastFormat));
    }

    somethingChanged();
}

// void OptionsSimple::formatChanged()
// {
//     QStringList errorList;
//     ui->replayGainCheckBox->setEnabled( config->pluginLoader()->canReplayGain(ui->formatComboBox->currentText(),currentPlugin,&errorList) );
//     if( !ui->replayGainCheckBox->isEnabled() )
//     {
//         if( !errorList.isEmpty() )
//         {
//             errorList.prepend( i18n("Replay Gain is not supported for the %1 file format.\nPossible solutions are listed below.",ui->formatComboBox->currentText()) );
//         }
//         else
//         {
//             errorList += i18n("Replay Gain is not supported for the %1 file format.\nPlease check your distribution's package manager in order to install an additional Replay Gain plugin.",ui->formatComboBox->currentText());
//         }
//         ui->replayGainCheckBox->setToolTip( errorList.join("\n\n") );
//     }
//     else
//     {
//         ui->replayGainCheckBox->setToolTip( "" );
//     }
// }

void OptionsSimple::outputDirectoryChanged()
{
    const QString profileName = ui->profileComboBox->currentText();
    ConversionOptions *conversionOptions = config->data.profiles.value(profileName);
    if( conversionOptions )
    {
        if( conversionOptions->outputDirectoryMode != ui->outputDirectory->mode() || conversionOptions->outputDirectory != ui->outputDirectory->directory() )
        {
            ui->profileComboBox->setCurrentIndex(ui->profileComboBox->findText(i18n("User defined")));
            profileChanged();
        }
    }
}

void OptionsSimple::somethingChanged()
{
    emit optionsChanged();
}

void OptionsSimple::currentDataRateChanged(int dataRate)
{
//     if( dataRate > 0 )
//     {
//         const QString dataRateString = Global::prettyNumber(dataRate,"B");
//         lEstimSize->setText( QString(QChar(8776))+" "+dataRateString+" / min." );
//         lEstimSize->setToolTip( i18n("Using the current conversion options will create files with approximately %1 per minute.").arg(dataRateString) );
//     }
//     else
//     {
//         lEstimSize->setText( QString(QChar(8776))+" ? B / min." );
//         lEstimSize->setToolTip( "" );
//     }
}

void OptionsSimple::setCurrentProfile(const QString& profile)
{
    // TODO check profile (and don't change, if not available)
    ui->profileComboBox->setCurrentIndex(ui->profileComboBox->findText(profile));
    profileChanged();
}

void OptionsSimple::setCurrentFormat(const QString& format)
{
    ui->formatComboBox->setCurrentIndex(ui->formatComboBox->findText(format));
//     formatChanged();
}

void OptionsSimple::setCurrentOutputDirectory(const QString& directory)
{
    ui->outputDirectory->setDirectory(directory);
    outputDirectoryChanged();
}

void OptionsSimple::setCurrentOutputDirectoryMode(int mode)
{
    ui->outputDirectory->setMode((OutputDirectory::Mode)mode);
    outputDirectoryChanged();
}

void OptionsSimple::showHelp()
{
    QList<CodecProblems::Problem> problemList;
    QMap<QString, QStringList> problems = config->pluginLoader()->encodeProblems();
    foreach( const QString& codecName, problems.keys() )
    {
        if( codecName != "wav" )
        {
            CodecProblems::Problem problem;
            problem.codecName = codecName;
            problem.solutions = problems.value(codecName);
            problemList += problem;
        }
    }
    CodecProblems *problemsDialog = new CodecProblems(CodecProblems::Debug, problemList, this);
    problemsDialog->exec();
}
