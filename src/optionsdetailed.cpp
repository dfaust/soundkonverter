
#include "optionsdetailed.h"
#include "config.h"
#include "core/codecplugin.h"
#include "core/codecwidget.h"
#include "outputdirectory.h"
#include "global.h"

#include <QApplication>
#include <QLayout>
#include <QBoxLayout>
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
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    int gridRow = 0;
    grid = new QGridLayout( this );

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lFormat = new QLabel( i18n("Format:"), this );
    topBox->addWidget( lFormat );
    cFormat = new KComboBox( this );
    topBox->addWidget( cFormat );
    cFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    connect( cFormat, SIGNAL(activated(const QString&)), this, SLOT(formatChanged(const QString&)) );
//     connect( cFormat, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()) );

    topBox->addStretch();

    lPlugin = new QLabel( i18n("Use Plugin:"), this );
    topBox->addWidget( lPlugin );
    cPlugin = new KComboBox( this );
    topBox->addWidget( cPlugin );
    cPlugin->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    connect( cPlugin, SIGNAL(activated(const QString&)), this, SLOT(encoderChanged(const QString&)) );
    connect( cPlugin, SIGNAL(activated(const QString&)), this, SLOT(somethingChanged()) );
    pConfigurePlugin = new KPushButton( KIcon("configure"), "", this );
    pConfigurePlugin->setFixedSize( cPlugin->sizeHint().height(), cPlugin->sizeHint().height() );
    pConfigurePlugin->setFlat( true );
    topBox->addWidget( pConfigurePlugin );
    topBox->setStretchFactor( pConfigurePlugin, 1 );
    connect( pConfigurePlugin, SIGNAL(clicked()), this, SLOT(configurePlugin()) );

    // draw a horizontal line
    QFrame *lineFrame = new QFrame( this );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    lineFrame->setFixedHeight( fontHeight );
    grid->addWidget( lineFrame, 1, 0 );

    // prepare the plugin widget
    wPlugin = 0;
    grid->setRowStretch( 2, 1 );
    grid->setRowMinimumHeight( 2, 20 );
    gridRow = 3;

    // draw a horizontal line
    lineFrame = new QFrame( this );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    lineFrame->setFixedHeight( fontHeight );
    grid->addWidget( lineFrame, gridRow++, 0 );

    int filterCount = 0;
    foreach( QString pluginName, config->data.backends.enabledFilters )
    {
        FilterPlugin *plugin = qobject_cast<FilterPlugin*>(config->pluginLoader()->backendPluginByName(pluginName));
        if( !plugin )
            continue;

        FilterWidget *widget = plugin->newFilterWidget();
        if( !widget )
            continue;

        wFilter.insert( widget, plugin );
        connect( widget, SIGNAL(optionsChanged()), this, SLOT(somethingChanged()) );
        grid->addWidget( widget, gridRow++, 0 );
        widget->show();
        filterCount++;
    }
    if( filterCount > 0 )
    {
        // draw a horizontal line
        lineFrame = new QFrame( this );
        lineFrame->setFrameShape( QFrame::HLine );
        lineFrame->setFrameShadow( QFrame::Sunken );
        lineFrame->setFixedHeight( fontHeight );
        grid->addWidget( lineFrame, gridRow++, 0 );
    }

    // the output directory
    QHBoxLayout *middleBox = new QHBoxLayout( );
    grid->addLayout( middleBox, gridRow++, 0 );

    QLabel *lOutput = new QLabel( i18n("Destination:"), this );
    middleBox->addWidget( lOutput );
    outputDirectory = new OutputDirectory( config, this );
    middleBox->addWidget( outputDirectory );

    QHBoxLayout *bottomBox = new QHBoxLayout();
    grid->addLayout( bottomBox, gridRow++, 0 );

    cReplayGain = new QCheckBox( i18n("Calculate Replay Gain tags"), this );
    bottomBox->addWidget( cReplayGain );
    //connect( cReplayGain, SIGNAL(toggled(bool)), this, SLOT(somethingChanged()) );
    bottomBox->addStretch();
    lEstimSize = new QLabel( QString(QChar(8776))+"? B / min." );
    lEstimSize->hide(); // hide for now because most plugins report inaccurate data
    bottomBox->addWidget( lEstimSize );
    pProfileSave = new KPushButton( KIcon("document-save"), "", this );
    bottomBox->addWidget( pProfileSave );
    pProfileSave->setFixedWidth( pProfileSave->height() );
    pProfileSave->setToolTip( i18n("Save current options as a profile") );
    connect( pProfileSave, SIGNAL(clicked()), this, SLOT(saveCustomProfile()) );
    pProfileLoad = new QToolButton( this );
    bottomBox->addWidget( pProfileLoad );
    pProfileLoad->setIcon( KIcon("document-open") );
    pProfileLoad->setPopupMode( QToolButton::InstantPopup );
    pProfileLoad->setFixedWidth( pProfileLoad->height() );
    pProfileLoad->setToolTip( i18n("Load saved profiles") );
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

void OptionsDetailed::resetFilterOptions()
{
    for( int i=0; i<wFilter.size(); i++ )
    {
        FilterWidget *widget = wFilter.keys().at(i);
        if( widget )
        {
            widget->setCurrentFilterOptions( 0 );
        }
    }
}

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

    if( cPlugin->currentText() != oldEncoder )
    {
        encoderChanged( cPlugin->currentText() );
    }
    else if( wPlugin )
    {
        wPlugin->setCurrentFormat( cFormat->currentText() );
    }

    lPlugin->setShown( format != "wav" );
    cPlugin->setShown( format != "wav" );
    pConfigurePlugin->setShown( format != "wav" );
    if( wPlugin )
        wPlugin->setShown( format != "wav" );

    QStringList errorList;
    cReplayGain->setEnabled( config->pluginLoader()->canReplayGain(cFormat->currentText(),currentPlugin,&errorList) );
    if( !cReplayGain->isEnabled() )
    {
        QPalette notificationPalette = cReplayGain->palette();
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
        cReplayGain->setToolTip( i18n("Replay Gain tags can tell your music player how loud a track is\nso it can adjust the volume to play all tracks with equal loudness.") );
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
    if( wPlugin )
    {
        grid->removeWidget( wPlugin );
        disconnect( wPlugin, SIGNAL(optionsChanged()), 0, 0 );
        wPlugin = currentPlugin->deleteCodecWidget( wPlugin );
    }
    currentPlugin = plugin;
    wPlugin = plugin->newCodecWidget();
    if( wPlugin )
    {
        connect( wPlugin, SIGNAL(optionsChanged()), this, SLOT(somethingChanged()) );
        qobject_cast<CodecWidget*>(wPlugin)->setCurrentFormat( cFormat->currentText() );
        if( plugin->lastUsedConversionOptions )
        {
            wPlugin->setCurrentConversionOptions( plugin->lastUsedConversionOptions );
            delete plugin->lastUsedConversionOptions;
            plugin->lastUsedConversionOptions = 0;
        }
        grid->addWidget( wPlugin, 2, 0 );
    }

    pConfigurePlugin->setEnabled( plugin->isConfigSupported(BackendPlugin::Encoder,"") );

    if( pConfigurePlugin->isEnabled() )
        pConfigurePlugin->setToolTip( i18n("Configure %1 ...",encoder) );
    else
        pConfigurePlugin->setToolTip( "" );
}

void OptionsDetailed::somethingChanged()
{
    int dataRate = 0;

    if( wPlugin )
        dataRate = wPlugin->currentDataRate();

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

void OptionsDetailed::configurePlugin()
{
    CodecPlugin *plugin = (CodecPlugin*)config->pluginLoader()->backendPluginByName( cPlugin->currentText() );

    if( plugin )
    {
        plugin->showConfigDialog( BackendPlugin::Encoder, "", this );
    }
}

ConversionOptions *OptionsDetailed::currentConversionOptions( bool saveLastUsed )
{
    ConversionOptions *options = 0;

    if( wPlugin && currentPlugin )
    {
        options = wPlugin->currentConversionOptions();
        if( options )
        {
            options->codecName = cFormat->currentText();
            if( options->codecName != "wav" )
                options->pluginName = currentPlugin->name();
            else
                options->pluginName = "";
            options->profile = wPlugin->currentProfile();
            options->outputDirectoryMode = outputDirectory->mode();
            options->outputDirectory = outputDirectory->directory();
            options->outputFilesystem = outputDirectory->filesystem();
            options->replaygain = cReplayGain->isEnabled() && cReplayGain->isChecked();

            for( int i=0; i<wFilter.size(); i++ )
            {
                FilterWidget *widget = wFilter.keys().at(i);
                FilterPlugin *plugin = wFilter.values().at(i);
                if( widget && plugin )
                {
                    FilterOptions *filterOptions = widget->currentFilterOptions();
                    if( filterOptions )
                    {
                        filterOptions->pluginName = plugin->name();
                        options->filterOptions.append( filterOptions );
                    }
                }
            }

            if( saveLastUsed )
            {
                config->data.general.lastProfile = currentProfile();
                saveCustomProfile( true );
                config->data.general.lastFormat = cFormat->currentText();
            }
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
    if( options->codecName != "wav" )
    {
        cPlugin->setCurrentIndex( cPlugin->findText(options->pluginName) );
        encoderChanged( cPlugin->currentText() );
    }
    outputDirectory->setMode( (OutputDirectory::Mode)options->outputDirectoryMode );
    outputDirectory->setDirectory( options->outputDirectory );
    cReplayGain->setChecked( options->replaygain );

    bool succeeded = true;

    if( options->codecName == "wav" )
        succeeded = true;
    else if( wPlugin )
        succeeded = wPlugin->setCurrentConversionOptions( options );
    else
        succeeded = false;

    QStringList usedFilter;
    foreach( FilterOptions *filterOptions, options->filterOptions )
    {
        bool filterSucceeded = false;
        for( int i=0; i<wFilter.size(); i++ )
        {
            FilterWidget *widget = wFilter.keys().at(i);
            FilterPlugin *plugin = wFilter.values().at(i);
            if( widget && plugin && filterOptions->pluginName == plugin->name() )
            {
                filterSucceeded = widget->setCurrentFilterOptions( filterOptions );
                usedFilter.append( filterOptions->pluginName );
                break;
            }
        }
        if( !filterSucceeded )
            succeeded = false;
    }
    // if a filter is disabled, its FilterOptions is 0 thus it won't be added to ConversionOptions, but we need to update the widget so it won't show false data
    for( int i=0; i<wFilter.size(); i++ )
    {
        FilterWidget *widget = wFilter.keys().at(i);
        FilterPlugin *plugin = wFilter.values().at(i);
        if( widget && plugin && !usedFilter.contains(plugin->name()) )
        {
            widget->setCurrentFilterOptions( 0 );
        }
    }

    return succeeded;
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

        QDomDocument list("soundkonverter_profilelist");
        QDomElement root;
        bool profileFound = false;

        QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/profiles.xml") );
        if( listFile.open( QIODevice::ReadOnly ) )
        {
            if( list.setContent( &listFile ) )
            {
                root = list.documentElement();
                if( root.nodeName() == "soundkonverter" && root.attribute("type") == "profilelist" )
                {
                    QDomNodeList conversionOptionsElements = root.elementsByTagName("conversionOptions");
                    for( int i=0; i<conversionOptionsElements.count(); i++ )
                    {
                        if( conversionOptionsElements.at(i).toElement().attribute("profileName") == profileName )
                        {
                            int ret;
                            if( lastUsed )
                                ret = KMessageBox::Yes;
                            else
                                ret = KMessageBox::questionYesNo( this, i18n("A profile with this name already exists.\n\nDo you want to overwrite the existing one?"), i18n("Profile already exists") );

                            if( ret == KMessageBox::Yes )
                            {
                                ConversionOptions *conversionOptions = currentConversionOptions( false );
                                delete config->data.profiles[profileName];
                                config->data.profiles[profileName] = conversionOptions;
                                root.removeChild(conversionOptionsElements.at(i));
                                QDomElement profileElement = conversionOptions->toXml(list);
                                profileElement.setAttribute("profileName",profileName);
                                root.appendChild(profileElement);
                                profileFound = true;
                                break;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }
            }
            listFile.close();
        }

        if( listFile.open( QIODevice::WriteOnly ) )
        {
            if( list.childNodes().isEmpty() )
            {
                root = list.createElement("soundkonverter");
                root.setAttribute("type","profilelist");
                list.appendChild(root);
            }

            if( !profileFound )
            {
                ConversionOptions *conversionOptions = currentConversionOptions( false );
                config->data.profiles[profileName] = conversionOptions;
                QDomElement profileElement = conversionOptions->toXml(list);
                profileElement.setAttribute("profileName",profileName);
                root.appendChild(profileElement);
            }

            updateProfiles();
            emit customProfilesEdited();

            QTextStream stream(&listFile);
            stream << list.toString();
            listFile.close();

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
    setCurrentProfile( profile );
}

QString OptionsDetailed::currentProfile()
{
    if( wPlugin )
        return wPlugin->currentProfile();
    else
        return "";
}

bool OptionsDetailed::setCurrentProfile( const QString& profile )
{
    if( config->data.profiles.keys().contains(profile) )
    {
        ConversionOptions *conversionOptions = config->data.profiles.value( profile );
        if( conversionOptions )
            return setCurrentConversionOptions( conversionOptions );
    }
    else if( wPlugin )
    {
        return wPlugin->setCurrentProfile( profile );
    }

    return false;
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


