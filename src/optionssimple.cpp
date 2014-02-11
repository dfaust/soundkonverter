
#include "optionssimple.h"
#include "config.h"
#include "outputdirectory.h"
#include "global.h"
#include "codecproblems.h"

#include <QApplication>
#include <QLayout>
#include <QBoxLayout>
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
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QGridLayout *grid = new QGridLayout( this );
    grid->setMargin( fontHeight );
    grid->setSpacing( fontHeight );

    QLabel *lQuality = new QLabel( i18n("Quality:"), this );
    grid->addWidget( lQuality, 0, 0 );

    QHBoxLayout *topBoxQuality = new QHBoxLayout();
    grid->addLayout( topBoxQuality, 0, 1 );
    cProfile = new KComboBox( this );
    topBoxQuality->addWidget( cProfile );
    connect( cProfile, SIGNAL(activated(int)), this, SLOT(profileChanged()) );
    topBoxQuality->addSpacing( 0.25*fontHeight );
    pProfileRemove = new KPushButton( KIcon("edit-delete"), i18n("Remove"), this );
    topBoxQuality->addWidget( pProfileRemove );
    pProfileRemove->setToolTip( i18n("Remove the selected profile") );
    pProfileRemove->hide();
    connect( pProfileRemove, SIGNAL(clicked()), this, SLOT(profileRemove()) );
    pProfileInfo = new KPushButton( KIcon("dialog-information"), i18n("Info"), this );
    topBoxQuality->addWidget( pProfileInfo );
    pProfileInfo->setToolTip( i18n("Information about the selected profile") );
//     cProfile->setFixedHeight( pProfileInfo->minimumSizeHint().height() );
    connect( pProfileInfo, SIGNAL(clicked()), this, SLOT(profileInfo()) );
    topBoxQuality->addStretch( );

    topBoxQuality->addSpacing( fontHeight );

    QLabel *lFormat = new QLabel( i18n("Format:"), this );
    grid->addWidget( lFormat, 0, 2 );

    QHBoxLayout *topBoxFormat = new QHBoxLayout();
    grid->addLayout( topBoxFormat, 0, 3 );
    cFormat = new KComboBox( this );
    topBoxFormat->addWidget( cFormat );
//     connect( cFormat, SIGNAL(activated(int)), this, SLOT(formatChanged()) );
    connect( cFormat, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    topBoxFormat->addSpacing( 0.25*fontHeight );
    pFormatInfo = new KPushButton( KIcon("dialog-information"), i18n("Info"), this );
    topBoxFormat->addWidget( pFormatInfo );
    pFormatInfo->setToolTip( i18n("Information about the selected file format") );
//     cFormat->setFixedHeight( pFormatInfo->minimumSizeHint().height() );
    connect( pFormatInfo, SIGNAL(clicked()), this, SLOT(formatInfo()) );
    topBoxFormat->addSpacing( 0.25*fontHeight );
    QLabel *formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("More formats...") + "</a>", this );
    topBoxFormat->addWidget( formatHelp );
    connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()) );
    topBoxFormat->addStretch( );

    QLabel *lOutput = new QLabel( i18n("Destination:"), this );
    grid->addWidget( lOutput, 1, 0 );

    QHBoxLayout *middleBox = new QHBoxLayout();
    grid->addLayout( middleBox, 1, 1, 1, 3 );
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

    QLabel *lOptional = new QLabel( i18n("Optional:") );
    grid->addWidget( lOptional, 3, 0 );

    QHBoxLayout *optionalBox = new QHBoxLayout();
    grid->addLayout( optionalBox, 3, 1 );
    cReplayGain = new QCheckBox( i18n("Calculate Replay Gain tags"), this );
    optionalBox->addWidget( cReplayGain );
    connect( cReplayGain, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    optionalBox->addStretch();

    QLabel *lInfo = new QLabel( text, this );
    grid->addWidget( lInfo, 4, 0, 1, 4, Qt::AlignVCenter | Qt::AlignCenter );
    grid->setRowStretch( 4, 1 );

    grid->setColumnStretch( 1, 1 );
    grid->setColumnStretch( 3, 1 );
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
    const QString sProfileString = cProfile->currentText();

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

    KMessageBox::information( this, info, i18n("Profile info for %1",sProfileString) );
}

void OptionsSimple::profileRemove()
{
    const QString profileName = cProfile->currentText();

    const int ret = KMessageBox::questionYesNo( this, i18n("Do you really want to remove the profile: %1").arg(profileName), i18n("Remove profile?") );
    if( ret == KMessageBox::Yes )
    {
        QDomDocument list("soundkonverter_profilelist");

        QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/profiles.xml") );
        if( listFile.open( QIODevice::ReadOnly ) )
        {
            if( list.setContent( &listFile ) )
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

        if( listFile.open( QIODevice::WriteOnly ) )
        {
            updateProfiles();
            emit customProfilesEdited();

            QTextStream stream(&listFile);
            stream << list.toString();
            listFile.close();
        }
    }
}

void OptionsSimple::formatInfo()
{
    const QString format = cFormat->currentText();
    const QString info = config->pluginLoader()->codecDescription(format);

    if( !info.isEmpty() )
    {
        KMessageBox::information( this, info, i18n("Format info for %1",format), QString(), KMessageBox::Notify | KMessageBox::AllowLink );
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
        cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    }
    else
    {
        foreach( const QString profileName, config->data.profiles.keys() )
        {
            if( profileName == profile )
            {
                ConversionOptions *conversionOptions = config->data.profiles.value( profileName );
                if( conversionOptions )
                {
                    cFormat->addItem( conversionOptions->codecName );
                    outputDirectory->setMode( (OutputDirectory::Mode)conversionOptions->outputDirectoryMode );
                    outputDirectory->setDirectory( conversionOptions->outputDirectory );
                    cReplayGain->setChecked( conversionOptions->replaygain );
                    pProfileRemove->show();
                    pProfileInfo->hide();
                }
                break;
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
    const QString profileName = cProfile->currentText();
    ConversionOptions *conversionOptions = config->data.profiles.value( profileName );
    if( conversionOptions )
    {
        if( conversionOptions->outputDirectoryMode != outputDirectory->mode() || conversionOptions->outputDirectory != outputDirectory->directory() )
        {
            cProfile->setCurrentIndex( cProfile->findText(i18n("User defined")) );
            profileChanged();
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
