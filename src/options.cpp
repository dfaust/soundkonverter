
#include "options.h"
#include "optionssimple.h"
#include "optionsdetailed.h"
#include "outputdirectory.h"
#include "pluginloader.h"
#include "config.h"

#include <QLayout>

#include <KLocalizedString>
#include <QTabWidget>

Options::Options(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

//     connect(ui.optionsSimple, SIGNAL(optionsChanged()),                                  this, SLOT(simpleOptionsChanged()));
//     connect(ui.optionsSimple->outputDirectory, SIGNAL(modeChanged(int)),                 this, SLOT(simpleOutputDirectoryModeChanged(int)));
//     connect(ui.optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(simpleOutputDirectoryChanged(const QString&)));

//     connect(ui.optionsDetailed->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(detailedOutputDirectoryModeChanged(int)));
//     connect(ui.optionsDetailed, SIGNAL(currentDataRateChanged(int)),       ui.optionsSimple, SLOT(currentDataRateChanged(int)));

//     connect(ui.optionsDetailed, SIGNAL(customProfilesEdited()), ui.optionsSimple, SLOT(updateProfiles()));
//     connect(ui.optionsSimple, SIGNAL(customProfilesEdited()),   ui.optionsDetailed, SLOT(updateProfiles()));
}

Options::~Options()
{
}

void Options::init(Config *config, const QString &text)
{
    this->config = config;

    ui.optionsSimple->init(config, text);
    ui.optionsDetailed->init(config);

//     QString format;
//     const QStringList formats = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid));
//     if( config->data.general.defaultFormat == i18n("Last used") || config->data.general.defaultFormat == "Last used" )
//     {
//         format = config->data.general.lastFormat;
//     }
//     else
//     {
//         format = config->data.general.defaultFormat;
//     }
//     if( !formats.contains(format) )
//         format.clear();
//
//         if( format.isEmpty() && formats.count() > 0 )
//         {
//             if( formats.contains(config->data.general.lastFormat) )
//                 format = config->data.general.lastFormat;
//                 else
//                     format = formats.at(0);
//         }
//         ui.optionsDetailed->setCurrentFormat( format );
//
//     QString profile;
//     if( config->data.general.defaultProfile == i18n("Last used") || config->data.general.defaultProfile == "Last used" )
//     {
//         profile = "soundkonverter_last_used";
//     }
//     else
//     {
//         profile = config->data.general.defaultProfile;
//     }
//     if( profile.isEmpty() )
//         profile = i18n("High");
//
//     ui.optionsDetailed->setCurrentProfile( profile );
//
//     const int startTab = ( config->data.general.startTab == 0 ) ? config->data.general.lastTab : config->data.general.startTab - 1;
//
// //     tab->addTab( ui.optionsSimple, i18n("Simple") );
// //     tab->addTab( ui.optionsDetailed, i18n("Detailed") );
//
//     ui.tabWidget->setCurrentIndex( startTab );
}

ConversionOptions *Options::currentConversionOptions()
{
    return ui.optionsDetailed->currentConversionOptions();
}

bool Options::setCurrentConversionOptions( ConversionOptions *options )
{
    const bool success = ui.optionsDetailed->setCurrentConversionOptions( options );
    tabChanged( 0 ); // update ui.optionsSimple
    return success;
}

void Options::simpleOutputDirectoryModeChanged( const int mode )
{
//     if( ui.optionsDetailed && ui.optionsDetailed->outputDirectory )
//         ui.optionsDetailed->outputDirectory->setMode( (OutputDirectory::Mode)mode );
//
//     config->data.general.lastOutputDirectoryMode = mode;
}

void Options::simpleOutputDirectoryChanged( const QString& directory )
{
//     if( ui.optionsDetailed && ui.optionsDetailed->outputDirectory )
//         ui.optionsDetailed->outputDirectory->setDirectory( directory );
}

void Options::simpleOptionsChanged()
{
//     ui.optionsDetailed->setCurrentFormat( ui.optionsSimple->currentFormat() );
//     ui.optionsDetailed->setCurrentProfile( ui.optionsSimple->currentProfile() );
//     ui.optionsDetailed->resetFilterOptions();
//     ui.optionsDetailed->setReplayGainChecked( ui.optionsSimple->isReplayGainChecked() );
//     QString toolTip;
//     const bool replaygainEnabled = ui.optionsDetailed->isReplayGainEnabled( &toolTip );
//     ui.optionsSimple->setReplayGainEnabled( replaygainEnabled, toolTip );
}

void Options::detailedOutputDirectoryModeChanged( const int mode )
{
//     config->data.general.lastOutputDirectoryMode = mode;
}

void Options::tabChanged( const int pageIndex )
{
//     if( pageIndex == 0 )
//     {
//         // NOTE prevent signals from firing back
//         disconnect( ui.optionsSimple, SIGNAL(optionsChanged()), 0, 0 );
//         disconnect( ui.optionsSimple->outputDirectory, SIGNAL(modeChanged(int)), 0, 0 );
//         disconnect( ui.optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), 0, 0 );
//
//         ui.optionsSimple->updateProfiles();
//         ui.optionsSimple->setCurrentProfile( ui.optionsDetailed->currentProfile() );
//         ui.optionsSimple->setCurrentFormat( ui.optionsDetailed->currentFormat() );
//         QString toolTip;
//         const bool replaygainEnabled = ui.optionsDetailed->isReplayGainEnabled( &toolTip );
//         const bool replaygainChecked = ui.optionsDetailed->isReplayGainChecked();
//         ui.optionsSimple->setReplayGainEnabled( replaygainEnabled, toolTip );
//         ui.optionsSimple->setReplayGainChecked( replaygainChecked );
//         ui.optionsSimple->setCurrentPlugin( ui.optionsDetailed->getCurrentPlugin() );
//
//         ui.optionsSimple->outputDirectory->setMode( ui.optionsDetailed->outputDirectory->mode() );
//         ui.optionsSimple->outputDirectory->setDirectory( ui.optionsDetailed->outputDirectory->directory() );
//
//         connect( ui.optionsSimple, SIGNAL(optionsChanged()), this, SLOT(simpleOptionsChanged()) );
//         connect( ui.optionsSimple->outputDirectory, SIGNAL(modeChanged(int)), this, SLOT(simpleOutputDirectoryModeChanged(int)) );
//         connect( ui.optionsSimple->outputDirectory, SIGNAL(directoryChanged(const QString&)), this, SLOT(simpleOutputDirectoryChanged(const QString&)) );
//     }
//
//     config->data.general.lastTab = ui.tabWidget->currentIndex();
}

void Options::setProfile( const QString& profile )
{
//     ui.optionsSimple->setCurrentProfile( profile );
//     simpleOptionsChanged();
}

void Options::setFormat( const QString& format )
{
//     ui.optionsSimple->setCurrentFormat( format );
//     simpleOptionsChanged();
}

void Options::setOutputDirectoryMode( int mode )
{
//     QString directory;
//     ui.optionsSimple->setCurrentOutputDirectoryMode( (OutputDirectory::Mode)mode );
//     if( mode == (int)OutputDirectory::Specify )
//         directory = config->data.general.specifyOutputDirectory;
//     else if( mode == (int)OutputDirectory::Source )
//         directory = "";
//     else if( mode == (int)OutputDirectory::MetaData )
//         directory = config->data.general.metaDataOutputDirectory;
//     else if( mode == (int)OutputDirectory::CopyStructure )
//         directory = config->data.general.copyStructureOutputDirectory;
//     ui.optionsSimple->setCurrentOutputDirectory( directory );
//     simpleOutputDirectoryModeChanged( (OutputDirectory::Mode)mode );
//     simpleOutputDirectoryChanged( directory );
}

void Options::setOutputDirectory( const QString& directory )
{
//     ui.optionsSimple->setCurrentOutputDirectoryMode( OutputDirectory::Specify );
//     ui.optionsSimple->setCurrentOutputDirectory( directory );
//     simpleOutputDirectoryModeChanged( OutputDirectory::Specify );
//     simpleOutputDirectoryChanged( directory );
}

void Options::accepted()
{
//     const OutputDirectory::Mode mode = (OutputDirectory::Mode)config->data.general.lastOutputDirectoryMode;
//
//     if( mode == OutputDirectory::MetaData )
//     {
//         const QString path = config->data.general.metaDataOutputDirectory;
//         if( config->data.general.lastMetaDataOutputDirectoryPaths.contains(path) )
//             config->data.general.lastMetaDataOutputDirectoryPaths.removeAll( path );
//         else if( config->data.general.lastMetaDataOutputDirectoryPaths.size() >= 5 )
//             config->data.general.lastMetaDataOutputDirectoryPaths.removeLast();
//         config->data.general.lastMetaDataOutputDirectoryPaths.prepend( path );
//     }
//     else if( mode == OutputDirectory::Specify )
//     {
//         const QString path = config->data.general.specifyOutputDirectory;
//         if( config->data.general.lastNormalOutputDirectoryPaths.contains(path) )
//             config->data.general.lastNormalOutputDirectoryPaths.removeAll( path );
//         else if( config->data.general.lastNormalOutputDirectoryPaths.size() >= 5 )
//             config->data.general.lastNormalOutputDirectoryPaths.removeLast();
//         config->data.general.lastNormalOutputDirectoryPaths.prepend( path );
//     }
//     else if( mode == OutputDirectory::CopyStructure )
//     {
//         const QString path = config->data.general.copyStructureOutputDirectory;
//         if( config->data.general.lastNormalOutputDirectoryPaths.contains(path) )
//             config->data.general.lastNormalOutputDirectoryPaths.removeAll( path );
//         else if( config->data.general.lastNormalOutputDirectoryPaths.size() >= 5 )
//             config->data.general.lastNormalOutputDirectoryPaths.removeLast();
//         config->data.general.lastNormalOutputDirectoryPaths.prepend( path );
//     }
}
