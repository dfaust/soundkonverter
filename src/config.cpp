
#include "config.h"
#include "logger.h"
#include "global.h"

#include <KConfigGroup>
#include <QDir>
#include <QDomElement>
#include <QTime>
#include <solid/device.h>
#include <KStandardDirs>


Config::Config( Logger *_logger, QObject *parent )
    : QObject( parent ),
    logger( _logger )
{
    connect( this, SIGNAL(updateWriteLogFilesSetting(bool)), logger, SLOT(updateWriteSetting(bool)) );

    pPluginLoader = new PluginLoader( logger, this );
    pTagEngine = new TagEngine( this );
    pConversionOptionsManager = new ConversionOptionsManager( pPluginLoader );
}

Config::~Config()
{
    save();

    qDeleteAll( data.profiles.values() );
    data.profiles.clear();

    delete pPluginLoader;
    delete pTagEngine;
    delete pConversionOptionsManager;
}

void Config::load()
{
    QTime time;
    time.start();

    QStringList formats;

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "General" );
    data.app.configVersion = group.readEntry( "configVersion", 0 );
    data.general.startTab = group.readEntry( "startTab", 0 );
    data.general.lastTab = group.readEntry( "lastTab", 0 );
    data.general.defaultProfile = group.readEntry( "defaultProfile", i18n("Last used") );
    data.general.lastProfile = group.readEntry( "lastProfile", i18n("High") );
    data.general.defaultFormat = group.readEntry( "defaultFormat", i18n("Last used") );
    data.general.lastFormat = group.readEntry( "lastFormat", "ogg vorbis" );
    data.general.lastOutputDirectoryMode = group.readEntry( "lastOutputDirectoryMode", 0 );
    data.general.specifyOutputDirectory = group.readEntry( "specifyOutputDirectory", QDir::homePath() + "/soundKonverter" );
    data.general.metaDataOutputDirectory = group.readEntry( "metaDataOutputDirectory", QDir::homePath() + "/soundKonverter/%b/%d - %n - %a - %t" );
    data.general.copyStructureOutputDirectory = group.readEntry( "copyStructureOutputDirectory", QDir::homePath() + "/soundKonverter" );
    data.general.lastMetaDataOutputDirectoryPaths = group.readEntry( "lastMetaDataOutputDirectoryPaths", QStringList() );
    data.general.lastNormalOutputDirectoryPaths = group.readEntry( "lastNormalOutputDirectoryPaths", QStringList() );
    data.general.waitForAlbumGain = group.readEntry( "waitForAlbumGain", true );
    data.general.useVFATNames = group.readEntry( "useVFATNames", false );
    data.general.writeLogFiles = group.readEntry( "writeLogFiles", false );
    data.general.conflictHandling = (Config::Data::General::ConflictHandling)group.readEntry( "conflictHandling", 0 );
//     data.general.priority = group.readEntry( "priority", 10 );
    data.general.numFiles = group.readEntry( "numFiles", 0 );
    data.general.numReplayGainFiles = group.readEntry( "numReplayGainFiles", 0 );
    if( data.general.numFiles == 0 || data.general.numReplayGainFiles == 0 )
    {
        QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
        const int num = processors.count() > 0 ? processors.count() : 1;
        if( data.general.numFiles == 0 )
            data.general.numFiles = num;
        if( data.general.numReplayGainFiles == 0 )
            data.general.numReplayGainFiles = num;
    }
//     data.general.executeUserScript = group.readEntry( "executeUserScript", false );
//     data.general.showToolBar = group.readEntry( "showToolBar", false );
//     data.general.outputFilePermissions = group.readEntry( "outputFilePermissions", 644 );
    data.general.actionMenuConvertMimeTypes = group.readEntry( "actionMenuConvertMimeTypes", QStringList() );
    data.general.actionMenuReplayGainMimeTypes = group.readEntry( "actionMenuReplayGainMimeTypes", QStringList() );
    data.general.replayGainGrouping = (Config::Data::General::ReplayGainGrouping)group.readEntry( "replayGainGrouping", 0 );
    data.general.preferredOggVorbisExtension = group.readEntry( "preferredOggVorbisExtension", "ogg" );
    data.general.preferredVorbisCommentCommentTag = group.readEntry( "preferredVorbisCommentCommentTag", "DESCRIPTION" );
    data.general.preferredVorbisCommentTrackTotalTag = group.readEntry( "preferredVorbisCommentTrackTotalTag", "TRACKTOTAL" );
    data.general.preferredVorbisCommentDiscTotalTag = group.readEntry( "preferredVorbisCommentDiscTotalTag", "DISCTOTAL" );

    // due to a bug lastNormalOutputDirectoryPaths could have more than 5 items
    while( data.general.lastNormalOutputDirectoryPaths.count() > 5 )
        data.general.lastNormalOutputDirectoryPaths.takeLast();

    group = conf->group( "Advanced" );
    data.advanced.useSharedMemoryForTempFiles = group.readEntry( "useSharedMemoryForTempFiles", false );
    data.advanced.sharedMemorySize = 0;
    if( QFile::exists("/dev/shm") )
    {
        system("df -B 1M /dev/shm | tail -1 > /dev/shm/soundkonverter_shm_size");
        QFile chkdf("/dev/shm/soundkonverter_shm_size");
        if( chkdf.open(QIODevice::ReadOnly|QIODevice::Text) )
        {
            QTextStream t( &chkdf );
            QString s = t.readLine();
            QRegExp rxlen( "^(?:\\S+)(?:\\s+)(?:\\s+)(\\d+)(?:\\s+)(\\d+)(?:\\s+)(\\d+)(?:\\s+)(\\d+)" );
            if( s.contains(rxlen) )
            {
                data.advanced.sharedMemorySize = rxlen.cap(1).toInt();
            }
            chkdf.close();
        }
        chkdf.remove();
    }
    data.advanced.maxSizeForSharedMemoryTempFiles = group.readEntry( "maxSizeForSharedMemoryTempFiles", data.advanced.sharedMemorySize / 4 );
    data.advanced.usePipes = group.readEntry( "usePipes", false );

    group = conf->group( "CoverArt" );
    data.coverArt.writeCovers = group.readEntry( "writeCovers", 1 );
    data.coverArt.writeCoverName = group.readEntry( "writeCoverName", 0 );
    data.coverArt.writeCoverDefaultName = group.readEntry( "writeCoverDefaultName", i18nc("cover file name","cover") );

    group = conf->group( "Backends" );
    data.backends.rippers = group.readEntry( "rippers", QStringList() );
    formats = group.readEntry( "formats", QStringList() );
    foreach( const QString format, formats )
    {
        CodecData codecData;
        codecData.codecName = format;
        codecData.encoders = group.readEntry( format + "_encoders", QStringList() );
        codecData.decoders = group.readEntry( format + "_decoders", QStringList() );
        codecData.replaygain = group.readEntry( format + "_replaygain", QStringList() );
        data.backends.codecs += codecData;
    }
    data.backends.filters = group.readEntry( "filters", QStringList() );
    data.backends.enabledFilters = group.readEntry( "enabledFilters", QStringList() );

    pPluginLoader->load();

    QString pluginName;
    bool found;
    QStringList enabledPlugins;
    QStringList newPlugins;
    int codecIndex;

    formats = pPluginLoader->formatList( PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain), PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

    // build default backend priority list

    // rippers
    enabledPlugins.clear();
    newPlugins.clear();
    // register existing enabled plugins as such and list new enabled plugins
    foreach( RipperPlugin *plugin, pPluginLoader->getAllRipperPlugins() )
    {
        pluginName = plugin->name();
        foreach( const ConversionPipeTrunk trunk, plugin->codecTable() )
        {
            if( trunk.enabled )
            {
                enabledPlugins += pluginName;
                if( !data.backends.rippers.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(trunk.rating).rightJustified(8,'0') + pluginName;
                    break;
                }
            }
        }
    }
    // remove plugins from the ripper list if they aren't enabled any more
    for( int i=0; i<data.backends.rippers.count(); i++ )
    {
        if( !enabledPlugins.contains(data.backends.rippers.at(i)) )
        {
            data.backends.rippers.removeAt(i);
            i--;
        }
    }
    // sort new enabled plugins and append them to the ripper list
    newPlugins.sort();
    for( int i=0; i<newPlugins.count(); i++ )
    {
        data.backends.rippers += newPlugins.at(i).right(newPlugins.at(i).length()-8);
    }

    foreach( const QString format, formats )
    {
        if( format == "wav" )
            continue;

        // get the index of the format in the data.backends.codecs list for direct access
        codecIndex = -1;
        for( int i=0; i<data.backends.codecs.count(); i++ )
        {
            if( data.backends.codecs.at(i).codecName == format )
            {
                codecIndex = i;
                break;
            }
        }
        // add format to the data.backends.codecs list if it isn't already in it
        if( codecIndex == -1 )
        {
            CodecData codecData;
            codecData.codecName = format;
            data.backends.codecs += codecData;
            codecIndex = data.backends.codecs.count() - 1;
        }

        // encoders
        enabledPlugins.clear();
        newPlugins.clear();
        // register existing enabled plugins as such and list new enabled plugins
        foreach( const ConversionPipeTrunk trunk, pPluginLoader->conversionFilterPipeTrunks )
        {
            if( trunk.codecTo == format && trunk.enabled )
            {
                pluginName = trunk.plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).encoders.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(trunk.rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        // remove plugins from the encoder list if they aren't enabled any more
        for( int i=0; i<data.backends.codecs.at(codecIndex).encoders.count(); i++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).encoders.at(i)) )
            {
                data.backends.codecs[codecIndex].encoders.removeAt(i);
                i--;
            }
        }
        // sort new enabled plugins and append them to the encoder list
        newPlugins.sort();
        for( int i=newPlugins.count()-1; i>=0; i-- ) // QStringList doesn't support sorting in descending order
        {
            data.backends.codecs[codecIndex].encoders += newPlugins.at(i).right(newPlugins.at(i).length()-8);
        }

        // decoders
        enabledPlugins.clear();
        newPlugins.clear();
        // register existing enabled plugins as such and list new enabled plugins
        foreach( const ConversionPipeTrunk trunk, pPluginLoader->conversionFilterPipeTrunks )
        {
            if( trunk.codecFrom == format && trunk.enabled )
            {
                pluginName = trunk.plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).decoders.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(trunk.rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        // remove plugins from the decoder list if they aren't enabled any more
        for( int i=0; i<data.backends.codecs.at(codecIndex).decoders.count(); i++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).decoders.at(i)) )
            {
                data.backends.codecs[codecIndex].decoders.removeAt(i);
                i--;
            }
        }
        // sort new enabled plugins and append them to the decoder list
        newPlugins.sort();
        for( int i=newPlugins.count()-1; i>=0; i-- ) // QStringList doesn't support sorting in descending order
        {
            data.backends.codecs[codecIndex].decoders += newPlugins.at(i).right(newPlugins.at(i).length()-8);
        }

        // replaygain
        enabledPlugins.clear();
        const bool internalReplayGainEnabled = pPluginLoader->hasCodecInternalReplayGain(format);
        if( internalReplayGainEnabled )
        {
            enabledPlugins += i18n("Try internal");
        }
        newPlugins.clear();
        // register existing enabled plugins as such and list new enabled plugins
        foreach( const ReplayGainPipe pipe, pPluginLoader->replaygainPipes )
        {
            if( pipe.codecName == format && pipe.enabled )
            {
                pluginName = pipe.plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).replaygain.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(pipe.rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        // remove plugins from the replay gain list if they aren't enabled any more
        for( int i=0; i<data.backends.codecs.at(codecIndex).replaygain.count(); i++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).replaygain.at(i)) )
            {
                data.backends.codecs[codecIndex].replaygain.removeAt(i);
                i--;
            }
        }
        // append internal replay gain if it is enabled
        if( internalReplayGainEnabled && !data.backends.codecs.at(codecIndex).replaygain.contains(i18n("Try internal")) )
        {
            data.backends.codecs[codecIndex].replaygain += i18n("Try internal");
        }
        // sort new enabled plugins and append them to the replay gain list
        newPlugins.sort();
        for( int i=newPlugins.count()-1; i>=0; i-- ) // QStringList doesn't support sorting in descending order
        {
            data.backends.codecs[codecIndex].replaygain += newPlugins.at(i).right(newPlugins.at(i).length()-8);
        }
    }

    // filters
    enabledPlugins.clear();
    newPlugins.clear();
    // register existing enabled plugins as such and list new enabled plugins
    foreach( FilterPlugin *plugin, pPluginLoader->getAllFilterPlugins() )
    {
        pluginName = plugin->name();
        foreach( const ConversionPipeTrunk trunk, plugin->codecTable() )
        {
            if( trunk.enabled && trunk.codecFrom == "wav" && trunk.codecTo == "wav" )
            {
                enabledPlugins += pluginName;
                if( !data.backends.filters.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(trunk.rating).rightJustified(8,'0') + pluginName;
                    break;
                }
            }
        }
    }
    // remove plugins from the filter list if they aren't enabled any more
    for( int i=0; i<data.backends.filters.count(); i++ )
    {
        if( !enabledPlugins.contains(data.backends.filters.at(i)) )
        {
            data.backends.filters.removeAt(i);
            i--;
        }
    }
    // sort new enabled plugins and append them to the filter list
    newPlugins.sort();
    for( int i=newPlugins.count()-1; i>=0; i-- ) // QStringList doesn't support sorting in descending order
    {
        data.backends.filters += newPlugins.at(i).right(newPlugins.at(i).length()-8);
    }
    // since filters can be completely disabled we have to keep track of data.backends.enabledFilters as well
    // remove plugins from the enabledFilters list if they aren't enabled any more
    for( int i=0; i<data.backends.enabledFilters.count(); i++ )
    {
        if( !data.backends.filters.contains(data.backends.enabledFilters.at(i)) )
        {
            data.backends.enabledFilters.removeAt(i);
            i--;
        }
    }
    // always enable the first filter
    if( data.app.configVersion < 1005 && data.backends.enabledFilters.isEmpty() && data.backends.filters.count() > 0 )
    {
        data.backends.enabledFilters.append( data.backends.filters.first() );
    }

    logger->log( 1000, "\nloading profiles ..." );
    QFile listFile( KStandardDirs::locateLocal("data","soundkonverter/profiles.xml") );
    if( listFile.open( QIODevice::ReadOnly ) )
    {
        QDomDocument list("soundkonverter_profilelist");
        if( list.setContent( &listFile ) )
        {
            QDomElement root = list.documentElement();
            if( root.nodeName() == "soundkonverter" && root.attribute("type") == "profilelist" )
            {
                QDomNodeList conversionOptionsElements = root.elementsByTagName("conversionOptions");
                for( int i=0; i<conversionOptionsElements.count(); i++ )
                {
                    ConversionOptions *conversionOptions = 0;
                    QList<QDomElement> filterOptionsElements;
                    const QString profileName = conversionOptionsElements.at(i).toElement().attribute("profileName");
                    const QString pluginName = conversionOptionsElements.at(i).toElement().attribute("pluginName");
                    CodecPlugin *plugin = (CodecPlugin*)pPluginLoader->backendPluginByName( pluginName );
                    if( plugin )
                    {
                        conversionOptions = plugin->conversionOptionsFromXml( conversionOptionsElements.at(i).toElement(), &filterOptionsElements );
                    }
                    else
                    {
                        logger->log( 1000, "\tname: " + profileName + ", plugin: " + pluginName );
                        logger->log( 1000, "\t\tcannot be loaded beacause the plugin cannot be found" );
                        continue;
                    }
                    if( conversionOptions )
                    {
                        foreach( QDomElement filterOptionsElement, filterOptionsElements )
                        {
                            FilterOptions *filterOptions = 0;
                            const QString filterPluginName = filterOptionsElement.attribute("pluginName");
                            FilterPlugin *filterPlugin = (FilterPlugin*)pPluginLoader->backendPluginByName( filterPluginName );
                            if( filterPlugin )
                            {
                                filterOptions = filterPlugin->filterOptionsFromXml( filterOptionsElement );
                            }
                            else
                            {
                                logger->log( 1000, "\tcannot load filter for profile: " + profileName );
                                continue;
                            }
                            conversionOptions->filterOptions.append( filterOptions );
                        }
                    }
                    if( conversionOptions )
                    {
                        data.profiles[profileName] = conversionOptions;
                        if( profileName != "soundkonverter_last_used" )
                            logger->log( 1000, "\tname: " + profileName + ", plugin: " + pluginName );
                    }
                }
            }
        }
        listFile.close();
    }
    logger->log( 1000, "... all profiles loaded\n" );

    QString profile;
    QStringList sFormat;
    QStringList sProfile;
    sProfile += i18n("Last used");
    sProfile += i18n("Very low");
    sProfile += i18n("Low");
    sProfile += i18n("Medium");
    sProfile += i18n("High");
    sProfile += i18n("Very high");
    sProfile += i18n("Lossless");
    sProfile += i18n("Hybrid");
    sProfile += customProfiles();
    if( sProfile.indexOf(data.general.defaultProfile) == -1 )
    {
        data.general.defaultProfile = i18n("High");
    }
    else
    {
        profile = data.general.defaultProfile;

        if( profile == i18n("Very low") || profile == i18n("Low") || profile == i18n("Medium") || profile == i18n("High") || profile == i18n("Very high") )
        {
            sFormat = pPluginLoader->formatList(PluginLoader::Encode,PluginLoader::Lossy);
        }
        else if( profile == i18n("Lossless") )
        {
            sFormat = pPluginLoader->formatList(PluginLoader::Encode,PluginLoader::Lossless);
        }
        else if( profile == i18n("Hybrid") )
        {
            sFormat = pPluginLoader->formatList(PluginLoader::Encode,PluginLoader::Hybrid);
        }
        else if( profile == i18n("User defined") )
        {
            sFormat = pPluginLoader->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid));
        }
        else
        {
            ConversionOptions *conversionOptions = data.profiles.value( profile );
            if( conversionOptions )
                sFormat += conversionOptions->codecName;
        }
        if( sFormat.indexOf(data.general.defaultFormat) == -1 )
        {
            data.general.defaultFormat = i18n("Last used");
        }
    }

    group = conf->group( "BackendOptimizationsIgnoreList" );
    const int backendOptimizationsIgnoreListCount = group.readEntry( "count", 0 );

    CodecOptimizations::Optimization optimization;
    for( int i=0; i<backendOptimizationsIgnoreListCount; i++ )
    {
        const QStringList backendOptimization = group.readEntry( QString("ignore_%1").arg(i), QStringList() );
        optimization.codecName = backendOptimization.at(0);
        const QString mode = backendOptimization.at(1);
        if( mode == "Encode" )
        {
            optimization.mode = CodecOptimizations::Optimization::Encode;
        }
        else if( mode == "Decode" )
        {
            optimization.mode = CodecOptimizations::Optimization::Decode;
        }
        else if( mode == "ReplayGain" )
        {
            optimization.mode = CodecOptimizations::Optimization::ReplayGain;
        }
        optimization.currentBackend = backendOptimization.at(2);
        optimization.betterBackend = backendOptimization.at(3);
        optimization.solution = CodecOptimizations::Optimization::Ignore;
        data.backendOptimizationsIgnoreList.optimizationList.append(optimization);
    }

    writeServiceMenu();

    logger->log( 1000, QString("loading of the configuration took %1 ms").arg(time.elapsed()) );
}

void Config::save()
{
    writeServiceMenu();

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;

    group = conf->group( "General" );
    group.writeEntry( "configVersion", SOUNDKONVERTER_VERSION_NUMBER );
    group.writeEntry( "startTab", data.general.startTab );
    group.writeEntry( "lastTab", data.general.lastTab );
    group.writeEntry( "defaultProfile", data.general.defaultProfile );
    group.writeEntry( "lastProfile", data.general.lastProfile );
    group.writeEntry( "defaultFormat", data.general.defaultFormat );
    group.writeEntry( "lastFormat", data.general.lastFormat );
    group.writeEntry( "lastOutputDirectoryMode", data.general.lastOutputDirectoryMode );
    group.writeEntry( "specifyOutputDirectory", data.general.specifyOutputDirectory );
    group.writeEntry( "metaDataOutputDirectory", data.general.metaDataOutputDirectory );
    group.writeEntry( "copyStructureOutputDirectory", data.general.copyStructureOutputDirectory );
    group.writeEntry( "lastMetaDataOutputDirectoryPaths", data.general.lastMetaDataOutputDirectoryPaths );
    group.writeEntry( "lastNormalOutputDirectoryPaths", data.general.lastNormalOutputDirectoryPaths );
    group.writeEntry( "waitForAlbumGain", data.general.waitForAlbumGain );
    group.writeEntry( "useVFATNames", data.general.useVFATNames );
    group.writeEntry( "writeLogFiles", data.general.writeLogFiles );
    group.writeEntry( "conflictHandling", (int)data.general.conflictHandling );
//     group.writeEntry( "priority", data.general.priority );
    group.writeEntry( "numFiles", data.general.numFiles );
    group.writeEntry( "numReplayGainFiles", data.general.numReplayGainFiles );
//     group.writeEntry( "executeUserScript", data.general.executeUserScript );
//     group.writeEntry( "showToolBar", data.general.showToolBar );
//     group.writeEntry( "outputFilePermissions", data.general.outputFilePermissions );
    group.writeEntry( "actionMenuConvertMimeTypes", data.general.actionMenuConvertMimeTypes );
    group.writeEntry( "actionMenuReplayGainMimeTypes", data.general.actionMenuReplayGainMimeTypes );
    group.writeEntry( "replayGainGrouping", (int)data.general.replayGainGrouping );
    group.writeEntry( "preferredOggVorbisExtension", data.general.preferredOggVorbisExtension );
    group.writeEntry( "preferredVorbisCommentCommentTag", data.general.preferredVorbisCommentCommentTag );
    group.writeEntry( "preferredVorbisCommentTrackTotalTag", data.general.preferredVorbisCommentTrackTotalTag );
    group.writeEntry( "preferredVorbisCommentDiscTotalTag", data.general.preferredVorbisCommentDiscTotalTag );

    group = conf->group( "Advanced" );
    group.writeEntry( "useSharedMemoryForTempFiles", data.advanced.useSharedMemoryForTempFiles );
    group.writeEntry( "maxSizeForSharedMemoryTempFiles", data.advanced.maxSizeForSharedMemoryTempFiles );
    group.writeEntry( "usePipes", data.advanced.usePipes );

    group = conf->group( "CoverArt" );
    group.writeEntry( "writeCovers", data.coverArt.writeCovers );
    group.writeEntry( "writeCoverName", data.coverArt.writeCoverName );
    group.writeEntry( "writeCoverDefaultName", data.coverArt.writeCoverDefaultName );

    group = conf->group( "Backends" );
    group.writeEntry( "rippers", data.backends.rippers );
    QStringList formats;
    foreach( const CodecData codec, data.backends.codecs )
    {
        const QString format = codec.codecName;
        group.writeEntry( format + "_encoders", codec.encoders );
        group.writeEntry( format + "_decoders", codec.decoders );
        group.writeEntry( format + "_replaygain", codec.replaygain );
        formats += format;
    }
    group.writeEntry( "formats", formats );
    group.writeEntry( "filters", data.backends.filters );
    group.writeEntry( "enabledFilters", data.backends.enabledFilters );

    group = conf->group( "BackendOptimizationsIgnoreList" );
    group.writeEntry( "count", data.backendOptimizationsIgnoreList.optimizationList.count() );

    for( int i=0; i<data.backendOptimizationsIgnoreList.optimizationList.count(); i++ )
    {
        QStringList backendOptimization;
        backendOptimization << data.backendOptimizationsIgnoreList.optimizationList.at(i).codecName;
        if( data.backendOptimizationsIgnoreList.optimizationList.at(i).mode == CodecOptimizations::Optimization::Encode )
        {
            backendOptimization << "Encode";
        }
        else if( data.backendOptimizationsIgnoreList.optimizationList.at(i).mode == CodecOptimizations::Optimization::Decode )
        {
            backendOptimization << "Decode";
        }
        else if( data.backendOptimizationsIgnoreList.optimizationList.at(i).mode == CodecOptimizations::Optimization::ReplayGain )
        {
            backendOptimization << "ReplayGain";
        }
        backendOptimization << data.backendOptimizationsIgnoreList.optimizationList.at(i).currentBackend;
        backendOptimization << data.backendOptimizationsIgnoreList.optimizationList.at(i).betterBackend;
        group.writeEntry( QString("ignore_%1").arg(i), backendOptimization );
    }

    emit updateWriteLogFilesSetting( data.general.writeLogFiles );
}

void Config::writeServiceMenu()
{
    QString content;
    QStringList mimeTypes;

    content = "";
    content += "[Desktop Entry]\n";
    content += "Type=Service\n";
    content += "Encoding=UTF-8\n";

    const QStringList convertFormats = pPluginLoader->formatList( PluginLoader::Decode, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

    mimeTypes.clear();
    foreach( const QString format, convertFormats )
    {
        mimeTypes += pPluginLoader->codecMimeTypes( format );
    }

    content += "ServiceTypes=KonqPopupMenu/Plugin," + mimeTypes.join(",") + "\n";

    content += "Icon=soundkonverter\n";
    content += "Actions=ConvertWithSoundkonverter;\n\n";

    content += "[Desktop Action ConvertWithSoundkonverter]\n";
    content += "Name="+i18n("Convert with soundKonverter")+"\n";
    content += "Icon=soundkonverter\n";
    content += "Exec=soundkonverter %F\n";

    const QString convertActionFileName = KStandardDirs::locateLocal( "services", "ServiceMenus/convert_with_soundkonverter.desktop" );
    if( ( data.general.actionMenuConvertMimeTypes != mimeTypes || !QFile::exists(convertActionFileName) ) && mimeTypes.count() > 0 )
    {
        QFile convertActionFile( convertActionFileName );
        if( convertActionFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        {
            QTextStream convertActionStream( &convertActionFile );
            convertActionStream << content;
            convertActionFile.close();
        }
        data.general.actionMenuConvertMimeTypes = mimeTypes;
    }

    content = "";
    content += "[Desktop Entry]\n";
    content += "Type=Service\n";
    content += "Encoding=UTF-8\n";

    const QStringList replaygainFormats = pPluginLoader->formatList( PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

    mimeTypes.clear();
    foreach( const QString format, replaygainFormats )
    {
        mimeTypes += pPluginLoader->codecMimeTypes( format );
    }

    content += "ServiceTypes=KonqPopupMenu/Plugin," + mimeTypes.join(",") + "\n";

    content += "Icon=soundkonverter_replaygain\n";
    content += "Actions=AddReplayGainWithSoundkonverter;\n\n";

    content += "[Desktop Action AddReplayGainWithSoundkonverter]\n";
    content += "Name="+i18n("Add Replay Gain with soundKonverter")+"\n";
    content += "Icon=soundkonverter-replaygain\n";
    content += "Exec=soundkonverter --replaygain %F\n";

    const QString replaygainActionFileName = KStandardDirs::locateLocal( "services", "ServiceMenus/add_replaygain_with_soundkonverter.desktop" );
    if( ( data.general.actionMenuReplayGainMimeTypes != mimeTypes || !QFile::exists(replaygainActionFileName) ) && mimeTypes.count() > 0 )
    {
        QFile replaygainActionFile( replaygainActionFileName );
        if( replaygainActionFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        {
            QTextStream replaygainActionStream( &replaygainActionFile );
            replaygainActionStream << content;
            replaygainActionFile.close();
        }
        data.general.actionMenuReplayGainMimeTypes = mimeTypes;
    }
}

QStringList Config::customProfiles()
{
    QStringList profiles;

    foreach( const QString profileName, data.profiles.keys() )
    {
        if( profileName == "soundkonverter_last_used" )
            continue;

        QList<CodecPlugin*> plugins = pPluginLoader->encodersForCodec( data.profiles.value(profileName)->codecName );
        foreach( CodecPlugin *plugin, plugins )
        {
            if( plugin->name() == data.profiles.value(profileName)->pluginName )
            {
                profiles.append( profileName );
                break;
            }
        }
    }

    return profiles;
}

QList<CodecOptimizations::Optimization> Config::getOptimizations( bool includeIgnored )
{
    QTime time;
    time.start();

    QList<CodecOptimizations::Optimization> optimizationList;
    CodecOptimizations::Optimization optimization;

    QStringList tempPluginList;
    QStringList optimizedPluginList;
    int currentBackendRating = 0;
    int betterBackendRating = 0;
    int codecIndex;
    bool ignore;

    const QStringList formats = pPluginLoader->formatList( PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain), PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    foreach( const QString format, formats )
    {
        if( format == "wav" )
            continue;

        codecIndex = -1;
        for( int j=0; j<data.backends.codecs.count(); j++ )
        {
            if( data.backends.codecs.at(j).codecName == format )
            {
                codecIndex = j;
                break;
            }
        }
        if( codecIndex == -1 )
            continue;

        // encoders
        tempPluginList.clear();
        for( int j=0; j<pPluginLoader->conversionPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->conversionPipeTrunks.at(j).codecTo == format && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                const QString pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        for( int j=0; j<pPluginLoader->filterPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->filterPipeTrunks.at(j).codecTo == format && pPluginLoader->filterPipeTrunks.at(j).enabled && pPluginLoader->filterPipeTrunks.at(j).plugin->type() == "filter" )
            {
                const QString pluginName = pPluginLoader->filterPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->filterPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        tempPluginList.sort();
        optimizedPluginList.clear();
        for( int j=tempPluginList.count()-1; j>=0; j-- )
        {
            const QString pluginName = tempPluginList.at(j).right(tempPluginList.at(j).length()-8);
            const int pluginRating = tempPluginList.at(j).left(8).toInt();
            optimizedPluginList += pluginName;
            if( data.backends.codecs.at(codecIndex).encoders.count() > 0 && pluginName == data.backends.codecs.at(codecIndex).encoders.first() )
            {
                currentBackendRating = pluginRating;
            }
            if( j == tempPluginList.count()-1 )
            {
                betterBackendRating = pluginRating;
            }
        }
        if( optimizedPluginList.count() != 0 && data.backends.codecs.at(codecIndex).encoders.count() != 0 )
        {
            ignore = false;
            for( int j=0; j<data.backendOptimizationsIgnoreList.optimizationList.count(); j++ )
            {
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == format &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).mode == CodecOptimizations::Optimization::Encode &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).currentBackend == data.backends.codecs.at(codecIndex).encoders.first() &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).betterBackend == optimizedPluginList.first()
                )
                {
                    ignore = true;
                }
            }

            // is there a better plugin available and has the better plugin really a higher rating or was it just sorted alphabetically at the top
            if( optimizedPluginList.first() != data.backends.codecs.at(codecIndex).encoders.first() && betterBackendRating > currentBackendRating )
            {
                if( ignore && includeIgnored )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::Encode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).encoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::Encode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).encoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Undecided;
                    optimizationList.append(optimization);
                }
            }
        }

        // decoders
        tempPluginList.clear();
        for( int j=0; j<pPluginLoader->conversionPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->conversionPipeTrunks.at(j).codecFrom == format && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                const QString pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        for( int j=0; j<pPluginLoader->filterPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->filterPipeTrunks.at(j).codecFrom == format && pPluginLoader->filterPipeTrunks.at(j).enabled && pPluginLoader->filterPipeTrunks.at(j).plugin->type() == "filter" )
            {
                const QString pluginName = pPluginLoader->filterPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->filterPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        tempPluginList.sort();
        optimizedPluginList.clear();
        for( int j=tempPluginList.count()-1; j>=0; j-- )
        {
            const QString pluginName = tempPluginList.at(j).right(tempPluginList.at(j).length()-8);
            const int pluginRating = tempPluginList.at(j).left(8).toInt();
            optimizedPluginList += pluginName;
            if( data.backends.codecs.at(codecIndex).decoders.count() > 0 && pluginName == data.backends.codecs.at(codecIndex).decoders.first() )
            {
                currentBackendRating = pluginRating;
            }
            if( j == tempPluginList.count()-1 )
            {
                betterBackendRating = pluginRating;
            }
        }
        if( optimizedPluginList.count() != 0 && data.backends.codecs.at(codecIndex).decoders.count() != 0 )
        {
            ignore = false;
            for( int j=0; j<data.backendOptimizationsIgnoreList.optimizationList.count(); j++ )
            {
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == format &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).mode == CodecOptimizations::Optimization::Decode &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).currentBackend == data.backends.codecs.at(codecIndex).decoders.first() &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).betterBackend == optimizedPluginList.first()
                )
                {
                    ignore = true;
                }
            }

            // is there a better plugin available and has the better plugin really a higher rating or was it just sorted alphabetically at the top
            if( optimizedPluginList.first() != data.backends.codecs.at(codecIndex).decoders.first() && betterBackendRating > currentBackendRating )
            {
                if( ignore && includeIgnored )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::Decode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).decoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::Decode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).decoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Undecided;
                    optimizationList.append(optimization);
                }
            }
        }

        // replaygain
        tempPluginList.clear();
        for( int j=0; j<pPluginLoader->replaygainPipes.count(); j++ )
        {
            if( pPluginLoader->replaygainPipes.at(j).codecName == format && pPluginLoader->replaygainPipes.at(j).enabled )
            {
                const QString pluginName = pPluginLoader->replaygainPipes.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->replaygainPipes.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        tempPluginList.sort();
        optimizedPluginList.clear();
        for( int j=tempPluginList.count()-1; j>=0; j-- )
        {
            const QString pluginName = tempPluginList.at(j).right(tempPluginList.at(j).length()-8);
            const int pluginRating = tempPluginList.at(j).left(8).toInt();
            optimizedPluginList += pluginName;
            if( data.backends.codecs.at(codecIndex).replaygain.count() > 0 && pluginName == data.backends.codecs.at(codecIndex).replaygain.first() )
            {
                currentBackendRating = pluginRating;
            }
            if( j == tempPluginList.count()-1 )
            {
                betterBackendRating = pluginRating;
            }
        }
        if( optimizedPluginList.count() != 0 && data.backends.codecs.at(codecIndex).replaygain.count() != 0 )
        {
            ignore = false;
            for( int j=0; j<data.backendOptimizationsIgnoreList.optimizationList.count(); j++ )
            {
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == format &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).mode == CodecOptimizations::Optimization::ReplayGain &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).currentBackend == data.backends.codecs.at(codecIndex).replaygain.first() &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).betterBackend == optimizedPluginList.first()
                )
                {
                    ignore = true;
                    break;
                }
            }

            // is there a better plugin available and has the better plugin really a higher rating or was it just sorted alphabetically at the top
            if( optimizedPluginList.first() != data.backends.codecs.at(codecIndex).replaygain.first() && betterBackendRating > currentBackendRating )
            {
                if( ignore && includeIgnored )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::ReplayGain;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).replaygain.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = format;
                    optimization.mode = CodecOptimizations::Optimization::ReplayGain;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).replaygain.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Undecided;
                    optimizationList.append(optimization);
                }
            }
        }
    }

    logger->log( 1000, QString("generation of the optimization list took %1 ms").arg(time.elapsed()) );

    return optimizationList;
}

void Config::doOptimizations( const QList<CodecOptimizations::Optimization>& optimizationList )
{
    int codecIndex;

    for( int i=0; i<optimizationList.count(); i++ )
    {
        if( optimizationList.at(i).solution == CodecOptimizations::Optimization::Fix )
        {
            codecIndex = -1;
            for( int j=0; j<data.backends.codecs.count(); j++ )
            {
                if( data.backends.codecs.at(j).codecName == optimizationList.at(i).codecName )
                {
                    codecIndex = j;
                    break;
                }
            }
            if( codecIndex == -1 )
                continue;

            for( int j=0; j<data.backendOptimizationsIgnoreList.optimizationList.count(); j++ )
            {
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == optimizationList.at(i).codecName &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).mode == optimizationList.at(i).mode &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).currentBackend == optimizationList.at(i).currentBackend &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).betterBackend == optimizationList.at(i).betterBackend
                )
                {
                    data.backendOptimizationsIgnoreList.optimizationList.removeAt( j );
                    break;
                }
            }

            if( optimizationList.at(i).mode == CodecOptimizations::Optimization::Encode )
            {
                data.backends.codecs[codecIndex].encoders.removeAll( optimizationList.at(i).betterBackend );
                data.backends.codecs[codecIndex].encoders.prepend( optimizationList.at(i).betterBackend );
            }
            else if( optimizationList.at(i).mode == CodecOptimizations::Optimization::Decode )
            {
                data.backends.codecs[codecIndex].decoders.removeAll( optimizationList.at(i).betterBackend );
                data.backends.codecs[codecIndex].decoders.prepend( optimizationList.at(i).betterBackend );
            }
            else if( optimizationList.at(i).mode == CodecOptimizations::Optimization::ReplayGain )
            {
                data.backends.codecs[codecIndex].replaygain.removeAll( optimizationList.at(i).betterBackend );
                data.backends.codecs[codecIndex].replaygain.prepend( optimizationList.at(i).betterBackend );
            }
        }
        else if( optimizationList.at(i).solution == CodecOptimizations::Optimization::Ignore )
        {
            bool found = false;

            for( int j=0; j<data.backendOptimizationsIgnoreList.optimizationList.count(); j++ )
            {
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == optimizationList.at(i).codecName &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).mode == optimizationList.at(i).mode &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).currentBackend == optimizationList.at(i).currentBackend &&
                    data.backendOptimizationsIgnoreList.optimizationList.at(j).betterBackend == optimizationList.at(i).betterBackend
                )
                {
                    found = true;
                    break;
                }
            }

            if( !found )
                data.backendOptimizationsIgnoreList.optimizationList.append(optimizationList.at(i));
        }
    }
}

