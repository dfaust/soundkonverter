
#include "options.h"
#include "optionssimple.h"
#include "optionsdetailed.h"
#include "outputdirectory.h"
#include "pluginloader.h"
#include "config.h"

#include <QLayout>

#include <KLocale>
#include <KTabWidget>


Options::Options( Config *_config, const QString& text, QWidget *parent )
    : QWidget( parent ),
    config( _config )
{
    QGridLayout *gridLayout = new QGridLayout( this );
    gridLayout->setContentsMargins( 0, 0, 0, 0 );

    tab = new KTabWidget( this );
    gridLayout->addWidget( tab, 0, 0 );
    connect( tab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );

    optionsSimple = new OptionsSimple( config, text, this );
    connect( optionsSimple, SIGNAL(optionsChanged()), this, SLOT(simpleOptionsChanged()) );
    connect( optionsSimple->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(simpleOutputDirectoryModeChanged(int)) );
    connect( optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(simpleOutputDirectoryChanged(const QString&)) );

    optionsDetailed = new OptionsDetailed( config, this );
    connect( optionsDetailed->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(detailedOutputDirectoryModeChanged(int)) );
    connect( optionsDetailed, SIGNAL(currentDataRateChanged(int)), optionsSimple, SLOT(currentDataRateChanged(int)) );

    connect( optionsDetailed, SIGNAL(customProfilesEdited()), optionsSimple, SLOT(updateProfiles()) );
    connect( optionsSimple, SIGNAL(customProfilesEdited()), optionsDetailed, SLOT(updateProfiles()) );

    optionsSimple->init();
    optionsDetailed->init();

    QString format;
    const QStringList formats = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid));
    if( config->data.general.defaultFormat == i18n("Last used") || config->data.general.defaultFormat == "Last used" )
    {
        format = config->data.general.lastFormat;
    }
    else
    {
        format = config->data.general.defaultFormat;
    }
    if( !formats.contains(format) )
        format.clear();

    if( format.isEmpty() && formats.count() > 0 )
    {
        if( formats.contains(config->data.general.lastFormat) )
            format = config->data.general.lastFormat;
        else
            format = formats.at(0);
    }
    optionsDetailed->setCurrentFormat( format );

    QString profile;
    if( config->data.general.defaultProfile == i18n("Last used") || config->data.general.defaultProfile == "Last used" )
    {
        profile = "soundkonverter_last_used";
    }
    else
    {
        profile = config->data.general.defaultProfile;
    }
    if( profile.isEmpty() )
        profile = i18n("High");

    optionsDetailed->setCurrentProfile( profile );

    const int startTab = ( config->data.general.startTab == 0 ) ? config->data.general.lastTab : config->data.general.startTab - 1;

    tab->addTab( optionsSimple, i18n("Simple") );
    tab->addTab( optionsDetailed, i18n("Detailed") );

    tab->setCurrentIndex( startTab );
}

Options::~Options()
{}

ConversionOptions *Options::currentConversionOptions()
{
    return optionsDetailed->currentConversionOptions();
}

bool Options::setCurrentConversionOptions( ConversionOptions *options )
{
    const bool success = optionsDetailed->setCurrentConversionOptions( options );
    tabChanged( 0 ); // update optionsSimple
    return success;
}

void Options::simpleOutputDirectoryModeChanged( const int mode )
{
    if( optionsDetailed && optionsDetailed->outputDirectory )
        optionsDetailed->outputDirectory->setMode( (OutputDirectory::Mode)mode );

    config->data.general.lastOutputDirectoryMode = mode;
}

void Options::simpleOutputDirectoryChanged( const QString& directory )
{
    if( optionsDetailed && optionsDetailed->outputDirectory )
        optionsDetailed->outputDirectory->setDirectory( directory );
}

void Options::simpleOptionsChanged()
{
    optionsDetailed->setCurrentFormat( optionsSimple->currentFormat() );
    optionsDetailed->setCurrentProfile( optionsSimple->currentProfile() );
    optionsDetailed->resetFilterOptions();
    optionsDetailed->setReplayGainChecked( optionsSimple->isReplayGainChecked() );
    QString toolTip;
    const bool replaygainEnabled = optionsDetailed->isReplayGainEnabled( &toolTip );
    optionsSimple->setReplayGainEnabled( replaygainEnabled, toolTip );
}

void Options::detailedOutputDirectoryModeChanged( const int mode )
{
    config->data.general.lastOutputDirectoryMode = mode;
}

void Options::tabChanged( const int pageIndex )
{
    if( pageIndex == 0 )
    {
        // NOTE prevent signals from firing back
        disconnect( optionsSimple, SIGNAL(optionsChanged()), 0, 0 );
        disconnect( optionsSimple->outputDirectory, SIGNAL(modeChanged(int)), 0, 0 );
        disconnect( optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), 0, 0 );

        optionsSimple->updateProfiles();
        optionsSimple->setCurrentProfile( optionsDetailed->currentProfile() );
        optionsSimple->setCurrentFormat( optionsDetailed->currentFormat() );
        QString toolTip;
        const bool replaygainEnabled = optionsDetailed->isReplayGainEnabled( &toolTip );
        const bool replaygainChecked = optionsDetailed->isReplayGainChecked();
        optionsSimple->setReplayGainEnabled( replaygainEnabled, toolTip );
        optionsSimple->setReplayGainChecked( replaygainChecked );
        optionsSimple->setCurrentPlugin( optionsDetailed->getCurrentPlugin() );

        optionsSimple->outputDirectory->setMode( optionsDetailed->outputDirectory->mode() );
        optionsSimple->outputDirectory->setDirectory( optionsDetailed->outputDirectory->directory() );

        connect( optionsSimple, SIGNAL(optionsChanged()), this, SLOT(simpleOptionsChanged()) );
        connect( optionsSimple->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(simpleOutputDirectoryModeChanged(int)) );
        connect( optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(simpleOutputDirectoryChanged(const QString&)) );
    }

    config->data.general.lastTab = tab->currentIndex();
}

void Options::setProfile( const QString& profile )
{
    optionsSimple->setCurrentProfile( profile );
    simpleOptionsChanged();
}

void Options::setFormat( const QString& format )
{
    optionsSimple->setCurrentFormat( format );
    simpleOptionsChanged();
}

void Options::setOutputDirectoryMode( int mode )
{
    QString directory;
    optionsSimple->setCurrentOutputDirectoryMode( (OutputDirectory::Mode)mode );
    if( mode == (int)OutputDirectory::Specify )
        directory = config->data.general.specifyOutputDirectory;
    else if( mode == (int)OutputDirectory::Source )
        directory = "";
    else if( mode == (int)OutputDirectory::MetaData )
        directory = config->data.general.metaDataOutputDirectory;
    else if( mode == (int)OutputDirectory::CopyStructure )
        directory = config->data.general.copyStructureOutputDirectory;
    optionsSimple->setCurrentOutputDirectory( directory );
    simpleOutputDirectoryModeChanged( (OutputDirectory::Mode)mode );
    simpleOutputDirectoryChanged( directory );
}

void Options::setOutputDirectory( const QString& directory )
{
    optionsSimple->setCurrentOutputDirectoryMode( OutputDirectory::Specify );
    optionsSimple->setCurrentOutputDirectory( directory );
    simpleOutputDirectoryModeChanged( OutputDirectory::Specify );
    simpleOutputDirectoryChanged( directory );
}

void Options::accepted()
{
    const OutputDirectory::Mode mode = (OutputDirectory::Mode)config->data.general.lastOutputDirectoryMode;

    if( mode == OutputDirectory::MetaData )
    {
        const QString path = config->data.general.metaDataOutputDirectory;
        if( config->data.general.lastMetaDataOutputDirectoryPaths.contains(path) )
            config->data.general.lastMetaDataOutputDirectoryPaths.removeAll( path );
        else if( config->data.general.lastMetaDataOutputDirectoryPaths.size() >= 5 )
            config->data.general.lastMetaDataOutputDirectoryPaths.removeLast();
        config->data.general.lastMetaDataOutputDirectoryPaths.prepend( path );
    }
    else if( mode == OutputDirectory::Specify )
    {
        const QString path = config->data.general.specifyOutputDirectory;
        if( config->data.general.lastNormalOutputDirectoryPaths.contains(path) )
            config->data.general.lastNormalOutputDirectoryPaths.removeAll( path );
        else if( config->data.general.lastNormalOutputDirectoryPaths.size() >= 5 )
            config->data.general.lastNormalOutputDirectoryPaths.removeLast();
        config->data.general.lastNormalOutputDirectoryPaths.prepend( path );
    }
    else if( mode == OutputDirectory::CopyStructure )
    {
        const QString path = config->data.general.copyStructureOutputDirectory;
        if( config->data.general.lastNormalOutputDirectoryPaths.contains(path) )
            config->data.general.lastNormalOutputDirectoryPaths.removeAll( path );
        else if( config->data.general.lastNormalOutputDirectoryPaths.size() >= 5 )
            config->data.general.lastNormalOutputDirectoryPaths.removeLast();
        config->data.general.lastNormalOutputDirectoryPaths.prepend( path );
    }
}
