
#include "optionssimple.h"
#include "config.h"
#include "outputdirectory.h"
#include "global.h"
#include "codecproblems.h"

#include <QLayout>
#include <QLabel>
#include <QString>
#include <QToolTip>

#include <KLocale>
#include <KIcon>
#include <KMessageBox>
#include <KComboBox>
#include <QCheckBox>
#include <KPushButton>
#include <QFile>
#include <KStandardDirs>


// FIXME when changing the output directory, check if the profile is a user defined and set it to 'User defined', if it is

// TODO hide lossless/hybrid/etc. when not available
OptionsSimple::OptionsSimple( Config *_config, /*OptionsDetailed* _optionsDetailed,*/ const QString &text, QWidget *parent )
    : QWidget( parent ),
    config( _config )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 6, 6, 6, 6 );
    grid->setSpacing( 6 );

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lQuality = new QLabel( i18n("Quality")+":", this );
    topBox->addWidget( lQuality );
    cProfile = new KComboBox( this );
    topBox->addWidget( cProfile );
    connect( cProfile, SIGNAL(activated(int)), this, SLOT(profileChanged()) );
    topBox->addSpacing( 3 );
    pProfileRemove = new KPushButton( KIcon("edit-delete"), i18n("Remove"), this );
    topBox->addWidget( pProfileRemove );
    pProfileRemove->setToolTip( i18n("Remove the selected profile") );
    pProfileRemove->hide();
    connect( pProfileRemove, SIGNAL(clicked()), this, SLOT(profileRemove()) );
    pProfileInfo = new KPushButton( KIcon("dialog-information"), i18n("Info"), this );
    topBox->addWidget( pProfileInfo );
    pProfileInfo->setToolTip( i18n("Information about the selected profile") );
//     cProfile->setFixedHeight( pProfileInfo->minimumSizeHint().height() );
    connect( pProfileInfo, SIGNAL(clicked()), this, SLOT(profileInfo()) );
    topBox->addSpacing( 18 );

    QLabel *lFormat = new QLabel( i18n("Output format")+":", this );
    topBox->addWidget( lFormat );
    cFormat = new KComboBox( this );
    topBox->addWidget( cFormat );
//     connect( cFormat, SIGNAL(activated(int)), this, SLOT(formatChanged()) );
    connect( cFormat, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    topBox->addSpacing( 3 );
    pFormatInfo = new KPushButton( KIcon("dialog-information"), i18n("Info"), this );
    topBox->addWidget( pFormatInfo );
    pFormatInfo->setToolTip( i18n("Information about the selected file format") );
//     cFormat->setFixedHeight( pFormatInfo->minimumSizeHint().height() );
    connect( pFormatInfo, SIGNAL(clicked()), this, SLOT(formatInfo()) );
    topBox->addSpacing( 3 );
    QLabel *formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("More") + "</a>", this );
    topBox->addWidget( formatHelp );
    connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()) );
    topBox->addStretch( );

    QHBoxLayout *middleBox = new QHBoxLayout( );
    grid->addLayout( middleBox, 1, 0 );

    outputDirectory = new OutputDirectory( config, this );
    middleBox->addWidget( outputDirectory );
    connect( outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(outputDirectoryChanged()) );
    connect( outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(outputDirectoryChanged()) );

    QHBoxLayout *estimSizeBox = new QHBoxLayout();
    grid->addLayout( estimSizeBox, 2, 0 );
    estimSizeBox->addStretch();
    lEstimSize = new QLabel( QString(QChar(8776))+"? B / min." );
    lEstimSize->hide(); // hide for now because most plugins report inaccurate data
    estimSizeBox->addWidget( lEstimSize );

    QHBoxLayout *optionalBox = new QHBoxLayout();
    grid->addLayout( optionalBox, 3, 0 );
    QLabel *lOptional = new QLabel( i18n("Optional:") );
    optionalBox->addWidget( lOptional );
    optionalBox->addSpacing( 12 );
    cReplayGain = new QCheckBox( i18n("Calculate Replay Gain tags"), this );
    optionalBox->addWidget( cReplayGain );
    connect( cReplayGain, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    optionalBox->addSpacing( 12 );
//     cBpm = new QCheckBox( i18n("Calculate BPM tags"), this );
//     cBpm->hide();
//     optionalBox->addWidget( cBpm );
//     connect( cBpm, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    optionalBox->addStretch();

    QLabel *lInfo = new QLabel( text, this );
    grid->addWidget( lInfo, 4, 0, Qt::AlignVCenter | Qt::AlignCenter );
    grid->setRowStretch( 4, 1 );
}

OptionsSimple::~OptionsSimple()
{}

void OptionsSimple::init()
{
    updateProfiles();
}

void OptionsSimple::setReplayGainEnabled( bool enabled, const QString& toolTip )
{
    cReplayGain->setEnabled(enabled);
    cReplayGain->setToolTip(toolTip);
    if( !enabled )
    {
        QPalette notificationPalette = cReplayGain->palette();
//         notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(181,96,101) );
        notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(174,127,130) );
        cReplayGain->setPalette( notificationPalette );
    }
}

void OptionsSimple::setReplayGainChecked( bool enabled )
{
    cReplayGain->setChecked(enabled);
}

QString OptionsSimple::currentProfile()
{
    return cProfile->currentText();
}

QString OptionsSimple::currentFormat()
{
    return cFormat->currentText();
}

bool OptionsSimple::isReplayGainChecked()
{
    return cReplayGain->isChecked();
}

void OptionsSimple::updateProfiles()
{
    const QString lastProfile = cProfile->currentText();
    QStringList sProfile;
    cProfile->clear();

    sProfile += i18n("Very low");
    sProfile += i18n("Low");
    sProfile += i18n("Medium");
    sProfile += i18n("High");
    sProfile += i18n("Very high");
    sProfile += i18n("Lossless");
//     sProfile += i18n("Hybrid"); // currently unused
    sProfile += config->customProfiles();
    sProfile += i18n("User defined");
    cProfile->addItems( sProfile );

    if( cProfile->findText(lastProfile) != -1 )
    {
        cProfile->setCurrentIndex( cProfile->findText(lastProfile) );
    }
    else
    {
        profileChanged();
    }
}

void OptionsSimple::profileInfo()
{
    QString sProfileString = cProfile->currentText();

    if( sProfileString == i18n("Very low") )
    {
        KMessageBox::information( this,
            i18n("This produces sound files of a very low quality.\nThat can be useful if you have a mobile device where your storage space is limited. It is not recommended to save your music in this quality without a copy with higher quality."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("Low") )
    {
        KMessageBox::information( this,
            i18n("This produces sound files of a low quality.\nThat can be useful if you have a mobile device where your storage space is limited. It is not recommended to save your music in this quality without a copy with higher quality."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("Medium") )
    {
        KMessageBox::information( this,
            i18n("This produces sound files of a medium quality.\nIf your disc space is limited, you can use this to save your music."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("High") )
    {
        KMessageBox::information( this,
            i18n("This produces sound files of a high quality.\nIf you have enough disc space available, you can use this to save your music."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("Very high") )
    {
        KMessageBox::information( this,
            i18n("This produces sound files of a very high quality.\nYou should only use this, if you are a quality freak and have enough disc space available."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("Lossless") )
    {
        KMessageBox::information( this,
            i18n("This produces files, that have exact the same quality as the input files.\nThis files are very big and definitely only for quality freaks."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("Hybrid") )
    {
        KMessageBox::information( this,
            i18n("This produces two files. One lossy compressed playable file and one correction file.\nBoth files together result in a file that is equivalent to the input file."),
            i18n("Profile info for %1",sProfileString) );
    }
    else if( sProfileString == i18n("User defined") )
    {
        KMessageBox::information( this,
            i18n("You can define your own profile in the \"detailed\" tab."),
            i18n("Profile info for %1",sProfileString) );
    }
    else // the info button is hidden when showing user defined profiles
    {
        KMessageBox::information( this,
            i18n("This is a user defined profile."),
            i18n("Profile info for %1",sProfileString) );
    }
}

void OptionsSimple::profileRemove()
{
    int ret = KMessageBox::questionYesNo( this, i18n("Do you really want to remove the profile: %1").arg(cProfile->currentText()), i18n("Remove profile?") );
    if( ret == KMessageBox::Yes )
    {
        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == cProfile->currentText() )
            {
                QFile::remove( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) + config->data.profiles.at(i).fileName );
                config->data.profiles.removeAt(i);
                updateProfiles();
                emit customProfilesEdited();
                return;
            }
        }
    }
}

void OptionsSimple::formatInfo()
{
    QString format = cFormat->currentText();
    QString info = config->pluginLoader()->codecDescription(format);

    if( !info.isEmpty() )
    {
        KMessageBox::information( this, info, i18n("Format info for %1",format), QString(), KMessageBox::Notify | KMessageBox::AllowLink );
/*        QMessageBox *messageBox = new QMessageBox( this );
        messageBox->setIcon( QMessageBox::Information );
        messageBox->setWindowTitle( i18n("Format info for %1",format) );
        messageBox->setText( info.replace("\n","<br>") );
        messageBox->setTextFormat( Qt::RichText );
        messageBox->exec();*/
    }
    else
    {
        KMessageBox::information( this, i18n("Sorry, no format information available.") );
    }
}

void OptionsSimple::profileChanged()
{
    const QString profile = cProfile->currentText();
    const QString lastFormat = cFormat->currentText();
    cFormat->clear();

    pProfileRemove->hide();
    pProfileInfo->show();

    if( profile == i18n("Very low") || profile == i18n("Low") || profile == i18n("Medium") || profile == i18n("High") || profile == i18n("Very high") )
    {
        cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossy) );
    }
    else if( profile == i18n("Lossless") )
    {
        cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossless) );
    }
    else if( profile == i18n("Hybrid") )
    {
        cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Hybrid) );
    }
    else if( profile == i18n("User defined") )
    {
        cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    }
    else
    {
        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == profile )
            {
                cFormat->addItem( config->data.profiles.at(i).codecName );
                QDomElement root = config->data.profiles.at(i).data.documentElement();
                QDomElement outputOptions = root.elementsByTagName("outputOptions").at(0).toElement();
                outputDirectory->setMode( (OutputDirectory::Mode)outputOptions.attribute("mode").toInt() );
                outputDirectory->setDirectory( outputOptions.attribute("directory") );
                QDomElement features = root.elementsByTagName("features").at(0).toElement();
                cReplayGain->setChecked( features.attribute("replaygain").toInt() );
                pProfileRemove->show();
                pProfileInfo->hide();
            }
        }
    }

    if( cFormat->findText(lastFormat) != -1 )
    {
        cFormat->setCurrentIndex( cFormat->findText(lastFormat) );
    }

    somethingChanged();
}

// void OptionsSimple::formatChanged()
// {
//     QStringList errorList;
//     cReplayGain->setEnabled( config->pluginLoader()->canReplayGain(cFormat->currentText(),currentPlugin,&errorList) );
//     if( !cReplayGain->isEnabled() )
//     {
//         if( !errorList.isEmpty() )
//         {
//             errorList.prepend( i18n("Replay Gain is not supported for the %1 file format.\nPossible solutions are listed below.",cFormat->currentText()) );
//         }
//         else
//         {
//             errorList += i18n("Replay Gain is not supported for the %1 file format.\nPlease check your distribution's package manager in order to install an additional Replay Gain plugin.",cFormat->currentText());
//         }
//         cReplayGain->setToolTip( errorList.join("\n\n") );
//     }
//     else
//     {
//         cReplayGain->setToolTip( "" );
//     }
// }

void OptionsSimple::outputDirectoryChanged()
{
    if( config->customProfiles().indexOf(cProfile->currentText()) != -1 )
    {
        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == cProfile->currentText() )
            {
                QDomElement root = config->data.profiles.at(i).data.documentElement();
                QDomElement outputOptions = root.elementsByTagName("outputOptions").at(0).toElement();
                if( outputOptions.attribute("mode").toInt() != outputDirectory->mode() || outputOptions.attribute("directory") != outputDirectory->directory() )
                {
                    cProfile->setCurrentIndex( cProfile->findText(i18n("User defined")) );
                    profileChanged();
                }
                return;
            }
        }
    }
}

void OptionsSimple::somethingChanged()
{
    emit optionsChanged();
}

void OptionsSimple::currentDataRateChanged( int dataRate )
{
    if( dataRate > 0 )
    {
        const QString dataRateString = Global::prettyNumber(dataRate,"B");
        lEstimSize->setText( QString(QChar(8776))+" "+dataRateString+" / min." );
        lEstimSize->setToolTip( i18n("Using the current conversion options will create files with approximately %1 per minute.").arg(dataRateString) );
    }
    else
    {
        lEstimSize->setText( QString(QChar(8776))+" ? B / min." );
        lEstimSize->setToolTip( "" );
    }
}

void OptionsSimple::setCurrentProfile( const QString& profile )
{
    // TODO check profile (and don't change, if not available)
    cProfile->setCurrentIndex( cProfile->findText(profile) );
    profileChanged();
}

void OptionsSimple::setCurrentFormat( const QString& format )
{
    cFormat->setCurrentIndex( cFormat->findText(format) );
//     formatChanged();
}

void OptionsSimple::setCurrentOutputDirectory( const QString& directory )
{
    outputDirectory->setDirectory( directory );
    outputDirectoryChanged();
}

void OptionsSimple::setCurrentOutputDirectoryMode( int mode )
{
    outputDirectory->setMode( (OutputDirectory::Mode)mode );
    outputDirectoryChanged();
}

void OptionsSimple::showHelp()
{
    QList<CodecProblems::Problem> problemList;
    QMap<QString,QStringList> problems = config->pluginLoader()->encodeProblems();
    for( int i=0; i<problems.count(); i++ )
    {
        CodecProblems::Problem problem;
        problem.codecName = problems.keys().at(i);
        if( problem.codecName != "wav" )
        {
            problem.solutions = problems.value(problem.codecName);
            problemList += problem;
        }
    }
    CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Debug, problemList, this );
    problemsDialog->exec();
}
