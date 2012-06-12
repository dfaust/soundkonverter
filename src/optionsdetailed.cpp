
#include "optionsdetailed.h"
#include "config.h"
#include "core/codecplugin.h"
#include "core/codecwidget.h"
#include "outputdirectory.h"
#include "global.h"

#include <QLayout>
#include <QLabel>

#include <KLocale>
#include <QFrame>
#include <QChar>
#include <KIcon>
#include <KComboBox>
#include <KPushButton>
#include <KInputDialog>
#include <KMessageBox>
#include <QFile>
#include <KStandardDirs>
#include <QMenu>
#include <QToolButton>

//
// class OptionsDetailed
//
////////////////////

OptionsDetailed::OptionsDetailed( Config* _config, QWidget* parent )
    : QWidget( parent ),
    config( _config )
{
    grid = new QGridLayout( this );
//     grid->setContentsMargins( 6, 6, 6, 6 );
//     grid->setSpacing( 6 );

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lFormat = new QLabel( i18n("Output format")+":", this );
    topBox->addWidget( lFormat );
    cFormat = new KComboBox( this );
    topBox->addWidget( cFormat );
    cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    connect( cFormat, SIGNAL(activated(const QString&)), this, SLOT(formatChanged(const QString&)) );
//     connect( cFormat, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()) );

    topBox->addStretch();

    QLabel *lPlugin = new QLabel( i18n("Use Plugin")+":", this );
    topBox->addWidget( lPlugin );
    cPlugin = new KComboBox( this );
    topBox->addWidget( cPlugin );
    cPlugin->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    connect( cPlugin, SIGNAL(activated(const QString&)), this, SLOT(encoderChanged(const QString&)) );
    connect( cPlugin, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()) );

    // draw a horizontal line
    QFrame *lineFrame = new QFrame( this );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    grid->addWidget( lineFrame, 1, 0 );

    // prepare the plugin widget
    wPlugin = 0;
    grid->setRowStretch( 2, 1 );

    // draw a horizontal line
    lineFrame = new QFrame( this );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    grid->addWidget( lineFrame, 3, 0 );

    // the output directory
    QHBoxLayout *middleBox = new QHBoxLayout( );
    grid->addLayout( middleBox, 4, 0 );

    outputDirectory = new OutputDirectory( config, this );
    middleBox->addWidget( outputDirectory );

    QHBoxLayout *bottomBox = new QHBoxLayout();
    grid->addLayout( bottomBox, 5, 0 );

    cReplayGain = new QCheckBox( i18n("Calculate Replay Gain tags"), this );
    bottomBox->addWidget( cReplayGain );
    //connect( cReplayGain, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    bottomBox->addSpacing( 12 );
//     cBpm = new QCheckBox( i18n("Calculate BPM tags"), this );
//     cBpm->hide();
//     bottomBox->addWidget( cBpm );
    //connect( cBpm, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    bottomBox->addStretch();
    lEstimSize = new QLabel( QString(QChar(8776))+"? B / min." );
    lEstimSize->hide(); // hide for now because most plugins report inaccurate data
    bottomBox->addWidget( lEstimSize );
    pProfileSave = new KPushButton( KIcon("document-save"), "", this );
    bottomBox->addWidget( pProfileSave );
    pProfileSave->setFixedWidth( pProfileSave->height() );
    pProfileSave->setToolTip( i18n("Save current options as a profile") );
    connect( pProfileSave, SIGNAL(clicked()), this, SLOT(saveCustomProfile()) );
//     pProfileLoad = new KPushButton( KIcon("document-open"), "", this );
    pProfileLoad = new QToolButton( this );
    bottomBox->addWidget( pProfileLoad );
    pProfileLoad->setIcon( KIcon("document-open") );
    pProfileLoad->setPopupMode( QToolButton::InstantPopup );
    pProfileLoad->setFixedWidth( pProfileLoad->height() );
    pProfileLoad->setToolTip( i18n("Load saved profiles") );
//     connect( pProfileLoad, SIGNAL(clicked()), this, SLOT(saveProfile()) );

/*
    QList<CodecPlugin*> plugins = config->pluginLoader()->getAllCodecPlugins();
    QList<QWidget*> widgets;
    QWidget *newWidget;
    for( int i=0; i<plugins.count(); i++ )
    {
        newWidget = plugins.at(i)->newCodecWidget();
        if( newWidget )
        {
            grid->addWidget( newWidget, 2, 0 );
            newWidget->show();
        }
        widgets += newWidget;
    }

    adjustSize();
    size_hint = sizeHint();

    for( int i=0; i<plugins.count(); i++ )
    {
        grid->removeWidget( widgets.at(i) );
        plugins.at(i)->deleteCodecWidget( widgets.at(i) );
    }
*/
}

OptionsDetailed::~OptionsDetailed()
{}

void OptionsDetailed::init()
{
    updateProfiles();

    cFormat->setCurrentIndex( 0 );
    formatChanged( cFormat->currentText() );
}

// QSize OptionsDetailed::sizeHint()
// {
//     return size_hint;
// }

void OptionsDetailed::setReplayGainChecked( bool enabled )
{
    cReplayGain->setChecked(enabled);
}

bool OptionsDetailed::isReplayGainEnabled( QString *toolTip )
{
    if( toolTip )
        *toolTip = cReplayGain->toolTip();

    return cReplayGain->isEnabled();
}

bool OptionsDetailed::isReplayGainChecked()
{
    return cReplayGain->isChecked();
}

CodecPlugin *OptionsDetailed::getCurrentPlugin()
{
    return currentPlugin;
}

//
// class private slots
//

void OptionsDetailed::updateProfiles()
{
    QMenu *menu = new QMenu( this );
    const QStringList profiles = config->customProfiles();
    for( int i=0; i<profiles.count(); i++ )
    {
        menu->addAction( profiles.at(i), this, SLOT(loadCustomProfileButtonClicked()) );
    }

    pProfileLoad->setMenu( menu );
    pProfileLoad->setShown( profiles.count() > 0 );
}

void OptionsDetailed::formatChanged( const QString& format )
{
    const QString oldEncoder = cPlugin->currentText();

    cPlugin->clear();
    //if( format != "wav" ) // TODO make it nicer if wav is selected
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == format )
        {
            cPlugin->addItems( config->data.backends.codecs.at(i).encoders );
        }
    }
    cPlugin->setCurrentIndex( 0 );
//     cPlugin->setEnabled( format != "wav" );

    if( cPlugin->currentText() != oldEncoder )
    {
        encoderChanged( cPlugin->currentText() );
    }
    else if( wPlugin )
    {
        qobject_cast<CodecWidget*>(wPlugin)->setCurrentFormat( cFormat->currentText() );
    }

    somethingChanged();
}

void OptionsDetailed::encoderChanged( const QString& encoder )
{
    CodecPlugin *plugin = (CodecPlugin*)config->pluginLoader()->backendPluginByName( encoder );
    if( !plugin )
    {
//         TODO leads to crashes
//         KMessageBox::error( this, i18n("Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nOptionsDetailed::encoderChanged; PluginLoader::codecPluginByName returned 0 for encoder: '%1'").arg(encoder), i18n("Internal error") );
        return;
    }
    currentPlugin = plugin;
    if( wPlugin )
    {
        grid->removeWidget( wPlugin );
        disconnect( wPlugin, SIGNAL(somethingChanged()), 0, 0 );
        wPlugin = plugin->deleteCodecWidget( wPlugin );
    }
    wPlugin = plugin->newCodecWidget();
    if( wPlugin )
    {
        connect( wPlugin, SIGNAL(somethingChanged()), this, SLOT(somethingChanged()) );
        qobject_cast<CodecWidget*>(wPlugin)->setCurrentFormat( cFormat->currentText() );
        grid->addWidget( wPlugin, 2, 0 );
        wPlugin->show();
//         wPlugin->setVisible( cFormat->currentText() != "wav" );
    }

    QStringList errorList;
    cReplayGain->setEnabled( config->pluginLoader()->canReplayGain(cFormat->currentText(),currentPlugin,&errorList) );
    if( !cReplayGain->isEnabled() )
    {
        QPalette notificationPalette = cReplayGain->palette();
//         notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(181,96,101) );
        notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(174,127,130) );
        cReplayGain->setPalette( notificationPalette );

        if( !errorList.isEmpty() )
        {
            errorList.prepend( i18n("Replay Gain is not supported for the %1 file format.\nPossible solutions are listed below.",cFormat->currentText()) );
        }
        else
        {
            errorList += i18n("Replay Gain is not supported for the %1 file format.\nPlease check your distribution's package manager in order to install an additional Replay Gain plugin.",cFormat->currentText());
        }
        cReplayGain->setToolTip( errorList.join("\n\n") );
    }
    else
    {
        cReplayGain->setToolTip( "" );
    }
}

void OptionsDetailed::somethingChanged()
{
    int dataRate = 0;

    if( wPlugin )
        dataRate = qobject_cast<CodecWidget*>(wPlugin)->currentDataRate();

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

    emit currentDataRateChanged( dataRate );
}

ConversionOptions *OptionsDetailed::currentConversionOptions()
{
    ConversionOptions *options = 0;

    if( wPlugin && currentPlugin )
    {
        options = qobject_cast<CodecWidget*>(wPlugin)->currentConversionOptions();
        if( options )
        {
            options->codecName = cFormat->currentText();
            if( options->codecName != "wav" )
                options->pluginName = currentPlugin->name();
            else
                options->pluginName = "";
            options->profile = qobject_cast<CodecWidget*>(wPlugin)->currentProfile();
            options->outputDirectoryMode = outputDirectory->mode();
            options->outputDirectory = outputDirectory->directory();
            options->outputFilesystem = outputDirectory->filesystem();
            options->replaygain = cReplayGain->isChecked();
//             options->bpm = cBpm->isChecked();

            config->data.general.lastProfile = currentProfile();
            saveCustomProfile( true );
            config->data.general.lastFormat = cFormat->currentText();
        }
    }

    return options;
}

bool OptionsDetailed::setCurrentConversionOptions( ConversionOptions *options )
{
    if( !options )
        return false;

    cFormat->setCurrentIndex( cFormat->findText(options->codecName) );
    formatChanged( cFormat->currentText() );
    cPlugin->setCurrentIndex( cPlugin->findText(options->pluginName) );
    encoderChanged( cPlugin->currentText() );
    outputDirectory->setMode( (OutputDirectory::Mode)options->outputDirectoryMode );
    outputDirectory->setDirectory( options->outputDirectory );
    cReplayGain->setChecked( options->replaygain );
//     cBpm->setChecked( options->bpm );

    if( wPlugin )
        return qobject_cast<CodecWidget*>(wPlugin)->setCurrentConversionOptions( options );
    else
        return false;
}

bool OptionsDetailed::saveCustomProfile( bool lastUsed )
{
    if( wPlugin && currentPlugin )
    {
        QString profileName;
        if( lastUsed )
        {
            profileName = "soundkonverter_last_used";
        }
        else
        {
            bool ok;
            profileName = KInputDialog::getText( i18n("New profile"), i18n("Enter a name for the new profile:"), "", &ok );
            if( !ok )
                return false;
        }

        if( profileName.isEmpty() )
        {
            KMessageBox::information( this, i18n("You cannot save a profile without a name."), i18n("Profile name is empty") );
            return false;
        }

        QStringList profiles;
        profiles += i18n("Very low");
        profiles += i18n("Low");
        profiles += i18n("Medium");
        profiles += i18n("High");
        profiles += i18n("Very high");
        profiles += i18n("Lossless");
        profiles += i18n("Hybrid");
        profiles += i18n("Last used");
        profiles += "Last used";
        profiles += i18n("User defined");
        if( !lastUsed )
            profiles += "soundkonverter_last_used";

        if( profiles.contains(profileName) )
        {
            KMessageBox::information( this, i18n("You cannot overwrite the built-in profiles."), i18n("Profile already exists") );
            return false;
        }

        QDomDocument data = qobject_cast<CodecWidget*>(wPlugin)->customProfile();
        QDomElement root = data.documentElement();
        root.setAttribute("pluginName",currentPlugin->name());
        root.setAttribute("profileName",profileName);
        QDomElement outputOptions = data.createElement("outputOptions");
        outputOptions.setAttribute("mode",outputDirectory->mode());
        outputOptions.setAttribute("directory",outputDirectory->directory());
        root.appendChild(outputOptions);
        QDomElement features = data.createElement("features");
        features.setAttribute("replaygain",cReplayGain->isChecked()&&cReplayGain->isEnabled());
        root.appendChild(features);

        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == profileName )
            {
                int ret;
                if( lastUsed )
                {
                    ret = KMessageBox::Yes;
                }
                else
                {
                    ret = KMessageBox::questionYesNo( this, i18n("A profile with this name already exists.\n\nDo you want to overwrite the existing one?"), i18n("Profile already exists") );
                }
                if( ret == KMessageBox::Yes )
                {
                    config->data.profiles[i].pluginName = currentPlugin->name();
                    config->data.profiles[i].codecName = cFormat->currentText();
                    config->data.profiles[i].data = data;
                    QFile profileFile( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) + config->data.profiles.at(i).fileName );
                    if( profileFile.open( QIODevice::WriteOnly ) )
                    {
                        QTextStream stream(&profileFile);
                        stream << data;
                        profileFile.close();
                        updateProfiles();
                        emit customProfilesEdited();
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }

        Config::ProfileData profile;
        int i=1;
        do {
            profile.fileName = QString::number(i);
            i++;
        } while( QFile::exists( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) + profile.fileName) );
        profile.pluginName = currentPlugin->name();
        profile.profileName = profileName;
        profile.codecName = cFormat->currentText();
        profile.data = data;
        config->data.profiles += profile;
        QFile profileFile( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) + profile.fileName );
        if( profileFile.open( QIODevice::WriteOnly ) )
        {
            QTextStream stream(&profileFile);
            stream << data;
            profileFile.close();
            updateProfiles();
            emit customProfilesEdited();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void OptionsDetailed::loadCustomProfileButtonClicked()
{
    const QString profile = qobject_cast<QAction*>(QObject::sender())->text().replace("&","");
    loadCustomProfile( profile );
}

bool OptionsDetailed::loadCustomProfile( const QString& profile )
{
    if( wPlugin && currentPlugin )
    {
        for( int i=0; i<config->data.profiles.count(); i++ )
        {
            if( config->data.profiles.at(i).profileName == profile && config->data.profiles.at(i).pluginName == currentPlugin->name() )
            {
                if( cFormat->currentText() != config->data.profiles.at(i).codecName )
                {
                    cFormat->setCurrentIndex( cFormat->findText(config->data.profiles.at(i).codecName) );
                    formatChanged( cFormat->currentText() );
                }
                if( cPlugin->currentText() != config->data.profiles.at(i).pluginName )
                {
                    cPlugin->setCurrentIndex( cPlugin->findText(config->data.profiles.at(i).pluginName) );
                    encoderChanged( cPlugin->currentText() );
                }
                QDomElement root = config->data.profiles.at(i).data.documentElement();
                QDomElement outputOptions = root.elementsByTagName("outputOptions").at(0).toElement();
                if( profile != "soundkonverter_last_used" )
                {
                    outputDirectory->setMode( (OutputDirectory::Mode)outputOptions.attribute("mode").toInt() );
                    outputDirectory->setDirectory( outputOptions.attribute("directory") );
                }
                QDomElement features = root.elementsByTagName("features").at(0).toElement();
                cReplayGain->setChecked( features.attribute("replaygain").toInt() );
                return qobject_cast<CodecWidget*>(wPlugin)->setCustomProfile( profile, config->data.profiles.at(i).data );
            }
        }
    }

    return false;
}

QString OptionsDetailed::currentProfile()
{
    if( wPlugin )
        return qobject_cast<CodecWidget*>(wPlugin)->currentProfile();
    else
        return "";
}

void OptionsDetailed::setCurrentProfile( const QString& profile )
{
    if( !wPlugin )
        return;

    if( !qobject_cast<CodecWidget*>(wPlugin)->setCurrentProfile(profile) )
    {
        // TODO find a plugin that supports the profile (eg. hybrid)
    }
}

QString OptionsDetailed::currentFormat()
{
    return cFormat->currentText();
}

void OptionsDetailed::setCurrentFormat( const QString& format )
{
    if( !format.isEmpty() && format != cFormat->currentText() )
    {
        cFormat->setCurrentIndex( cFormat->findText(format) );
        formatChanged( cFormat->currentText() );
    }
}


