
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
    pPluginLoader = new PluginLoader( logger, this );
    pTagEngine = new TagEngine();
    pConversionOptionsManager = new ConversionOptionsManager( pPluginLoader );

    data.general.updateDelay = 100;
}

Config::~Config()
{
    save();
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
//     data.general.waitForAlbumGain = group.readEntry( "waitForAlbumGain", false );
    data.general.useVFATNames = group.readEntry( "useVFATNames", false );
    data.general.conflictHandling = (Config::Data::General::ConflictHandling)group.readEntry( "conflictHandling", 0 );
//     data.general.priority = group.readEntry( "priority", 10 );
    data.general.numFiles = group.readEntry( "numFiles", 0 );
    if( data.general.numFiles == 0 )
    {
        QList<Solid::Device> processors = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
        data.general.numFiles = ( processors.count() > 0 ) ? processors.count() : 1;
    }
    data.general.updateDelay = group.readEntry( "updateDelay", 100 );
//     data.general.executeUserScript = group.readEntry( "executeUserScript", false );
//     data.general.showToolBar = group.readEntry( "showToolBar", false );
//     data.general.outputFilePermissions = group.readEntry( "outputFilePermissions", 644 );
    data.general.createActionsMenu = group.readEntry( "createActionsMenu", true );
    data.general.removeFailedFiles = group.readEntry( "removeFailedFiles", true );
    data.general.replayGainGrouping = (Config::Data::General::ReplayGainGrouping)group.readEntry( "replayGainGrouping", 0 );
    data.general.preferredOggVorbisExtension = group.readEntry( "preferredOggVorbisExtension", "ogg" );

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
    data.advanced.maxSizeForSharedMemoryTempFiles = group.readEntry( "maxSizeForSharedMemoryTempFiles", data.advanced.sharedMemorySize / 2 );

    group = conf->group( "Backends" );
    data.backends.rippers = group.readEntry( "rippers", QStringList() );
    formats = group.readEntry( "formats", QStringList() );
    for( int i=0; i<formats.count(); i++ )
    {
        CodecData codecData;
        codecData.codecName = formats.at(i);
        codecData.encoders = group.readEntry( formats.at(i) + "_encoders", QStringList() );
        codecData.decoders = group.readEntry( formats.at(i) + "_decoders", QStringList() );
        codecData.replaygain = group.readEntry( formats.at(i) + "_replaygain", QStringList() );
        data.backends.codecs += codecData;
    }

    pPluginLoader->load();

    QList<CodecPlugin*> codecPlugins;
    QList<ReplayGainPlugin*> replaygainPlugins;
    QString pluginName;
    bool found;
    QStringList enabledPlugins;
    QStringList newPlugins;
    int codecIndex;

    formats = pPluginLoader->formatList( PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain), PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

    // build default backend priority list

    // ripper
    enabledPlugins.clear();
    newPlugins.clear();
    QList<RipperPlugin*> ripperPlugins = pPluginLoader->getAllRipperPlugins();
    for( int i=0; i<ripperPlugins.count(); i++ )
    {
        pluginName = ripperPlugins.at(i)->name();
        QList<ConversionPipeTrunk> codecTable = ripperPlugins.at(i)->codecTable();
        for( int j = 0; j < codecTable.count(); j++ )
        {
            if( codecTable.at(j).enabled )
            {
                enabledPlugins += pluginName;
                if( !data.backends.rippers.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(codecTable.at(j).rating).rightJustified(8,'0') + pluginName;
                    break;
                }
            }
        }
    }
    for( int j=0; j<data.backends.rippers.count(); j++ )
    {
        if( !enabledPlugins.contains(data.backends.rippers.at(j)) )
        {
            data.backends.rippers.removeAt(j);
            j--;
        }
    }
    newPlugins.sort();
    for( int i=0; i<newPlugins.count(); i++ )
    {
        data.backends.rippers += newPlugins.at(i).right(newPlugins.at(i).length()-8);
    }

    for( int i=0; i<formats.count(); i++ )
    {
        if( formats.at(i) == "wav" )
            continue;

        found = false;
        for( int k=0; k<data.backends.codecs.count(); k++ )
        {
            if( data.backends.codecs.at(k).codecName == formats.at(i) )
            {
                found = true;
                break;
            }
        }
        if( !found )
        {
            CodecData codecData;
            codecData.codecName = formats.at(i);
            data.backends.codecs += codecData;
        }

        codecIndex = -1;
        for( int j=0; j<data.backends.codecs.count(); j++ )
        {
            if( data.backends.codecs.at(j).codecName == formats.at(i) )
            {
                codecIndex = j;
                break;
            }
        }
        if( codecIndex == -1 )
            continue;

        // encoders
        enabledPlugins.clear();
        newPlugins.clear();
        for( int j=0; j<pPluginLoader->conversionPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->conversionPipeTrunks.at(j).codecTo == formats.at(i) && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).encoders.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        for( int j=0; j<data.backends.codecs.at(codecIndex).encoders.count(); j++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).encoders.at(j)) )
            {
                data.backends.codecs[codecIndex].encoders.removeAt(j);
                j--;
            }
        }
        newPlugins.sort();
        for( int j=newPlugins.count()-1; j>=0; j-- )
        {
            data.backends.codecs[codecIndex].encoders += newPlugins.at(j).right(newPlugins.at(j).length()-8);
        }

        // decoders
        enabledPlugins.clear();
        newPlugins.clear();
        for( int j=0; j<pPluginLoader->conversionPipeTrunks.count(); j++ )
        {
            if( pPluginLoader->conversionPipeTrunks.at(j).codecFrom == formats.at(i) && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).decoders.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        for( int j=0; j<data.backends.codecs.at(codecIndex).decoders.count(); j++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).decoders.at(j)) )
            {
                data.backends.codecs[codecIndex].decoders.removeAt(j);
                j--;
            }
        }
        newPlugins.sort();
        for( int j=newPlugins.count()-1; j>=0; j-- )
        {
            data.backends.codecs[codecIndex].decoders += newPlugins.at(j).right(newPlugins.at(j).length()-8);
        }

        // replaygain
        enabledPlugins.clear();
        if( pPluginLoader->hasCodecInternalReplayGain(formats.at(i)) )
        {
            enabledPlugins += i18n("Try internal");
        }
        newPlugins.clear();
        for( int j=0; j<pPluginLoader->replaygainPipes.count(); j++ )
        {
            if( pPluginLoader->replaygainPipes.at(j).codecName == formats.at(i) && pPluginLoader->replaygainPipes.at(j).enabled )
            {
                pluginName = pPluginLoader->replaygainPipes.at(j).plugin->name();
                enabledPlugins += pluginName;
                if( !data.backends.codecs.at(codecIndex).replaygain.contains(pluginName) && newPlugins.filter(QRegExp("[0-9]{8,8}"+pluginName)).count()==0 )
                {
                    newPlugins += QString::number(pPluginLoader->replaygainPipes.at(j).rating).rightJustified(8,'0') + pluginName;
                }
            }
        }
        for( int j=0; j<data.backends.codecs.at(codecIndex).replaygain.count(); j++ )
        {
            if( !enabledPlugins.contains(data.backends.codecs.at(codecIndex).replaygain.at(j)) )
            {
                data.backends.codecs[codecIndex].replaygain.removeAt(j);
                j--;
            }
        }
        if( pPluginLoader->hasCodecInternalReplayGain(formats.at(i)) && !data.backends.codecs.at(codecIndex).replaygain.contains(i18n("Try internal")) )
        {
            data.backends.codecs[codecIndex].replaygain += i18n("Try internal");
        }
        newPlugins.sort();
        for( int j=newPlugins.count()-1; j>=0; j-- )
        {
            data.backends.codecs[codecIndex].replaygain += newPlugins.at(j).right(newPlugins.at(j).length()-8);
        }
    }

    // load profiles
    QFile profilesFile;

    logger->log( 1000, "\nloading profiles ..." );

    QDir profilesDir( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) );
    profilesDir.setFilter( QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Readable );

    const QStringList profilesDirList = profilesDir.entryList();

    for( int i=0; i<profilesDirList.count(); i++ )
    {
        if( profilesDirList.at(i) == "." || profilesDirList.at(i) == ".." )
            continue;

        logger->log( 1000, "\tloading file: " + profilesDirList.at(i) );

        profilesFile.setFileName( KStandardDirs::locateLocal("data",QString("soundkonverter/profiles/")) + profilesDirList.at(i) );
        if( profilesFile.open( QIODevice::ReadOnly ) )
        {
            QDomDocument domTree("soundkonverter_profile");
            if( domTree.setContent( &profilesFile ) )
            {
                QDomElement root = domTree.documentElement();
                if( root.nodeName() == "soundkonverter" && root.attribute("type") == "profile" )
                {
                    ProfileData profile;
                    profile.fileName = profilesDirList.at(i);
                    profile.pluginName = root.attribute("pluginName");
                    profile.profileName = root.attribute("profileName");
                    profile.codecName = root.attribute("codecName");
                    profile.data = domTree;
                    data.profiles += profile;
                    logger->log( 1000, "\t\tname: " + profile.profileName + ", plugin: " + profile.pluginName );
                }
            }
            else
            {
                logger->log( 1000, "<pre>\t\t<span style=\"color:red\">failed to load profile: bad file format</span></pre>" );
            }
            profilesFile.close();
        }
        else
        {
            logger->log( 1000, "<pre>\t\t<span style=\"color:red\">can't open file for reading</span></pre>" );
        }
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
            sFormat = pPluginLoader->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid));
        }
        else
        {
            for( int i=0; i<data.profiles.count(); i++ )
            {
                if( data.profiles.at(i).profileName == profile )
                {
                    sFormat += data.profiles.at(i).codecName;
                }
            }
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

    logger->log( 1000, QString("loading of the configuration took %1 ms").arg(time.elapsed()) );
}

void Config::save()
{
    QString format;
    QStringList formats;

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
    group.writeEntry( "useVFATNames", data.general.useVFATNames );
    group.writeEntry( "conflictHandling", (int)data.general.conflictHandling );
//     group.writeEntry( "priority", data.general.priority );
    group.writeEntry( "numFiles", data.general.numFiles );
    group.writeEntry( "updateDelay", data.general.updateDelay );
//     group.writeEntry( "executeUserScript", data.general.executeUserScript );
//     group.writeEntry( "showToolBar", data.general.showToolBar );
//     group.writeEntry( "outputFilePermissions", data.general.outputFilePermissions );
    group.writeEntry( "createActionsMenu", data.general.createActionsMenu );
    group.writeEntry( "removeFailedFiles", data.general.removeFailedFiles );
    group.writeEntry( "replayGainGrouping", (int)data.general.replayGainGrouping );
    group.writeEntry( "preferredOggVorbisExtension", data.general.preferredOggVorbisExtension );

    group = conf->group( "Advanced" );
    group.writeEntry( "useSharedMemoryForTempFiles", data.advanced.useSharedMemoryForTempFiles );
    group.writeEntry( "maxSizeForSharedMemoryTempFiles", data.advanced.maxSizeForSharedMemoryTempFiles );

    group = conf->group( "Backends" );
    group.writeEntry( "rippers", data.backends.rippers );
    for( int i=0; i<data.backends.codecs.count(); i++ )
    {
        format = data.backends.codecs.at(i).codecName;
        group.writeEntry( format + "_encoders", data.backends.codecs.at(i).encoders );
        group.writeEntry( format + "_decoders", data.backends.codecs.at(i).decoders );
        group.writeEntry( format + "_replaygain", data.backends.codecs.at(i).replaygain );
        formats += format;
    }
    group.writeEntry( "formats", formats );

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

    writeServiceMenu();
}

void Config::writeServiceMenu()
{
    if( data.general.createActionsMenu )
    {
        QString content;
        QStringList mimeTypes;

        content = "";
        content += "[Desktop Entry]\n";
        content += "Type=Service\n";
        content += "Encoding=UTF-8\n";

        const QStringList convertFormats = pPluginLoader->formatList( PluginLoader::Decode, PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

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

        if( mimeTypes.count() > 0 )
        {
            QFile convertActionFile( KStandardDirs::locateLocal( "services", "ServiceMenus/convert_with_soundkonverter.desktop" ) );
            if( convertActionFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
            {
                QTextStream convertActionStream( &convertActionFile );
                convertActionStream << content;
                convertActionFile.close();
            }
        }

        content = "";
        content += "[Desktop Entry]\n";
        content += "Type=Service\n";
        content += "Encoding=UTF-8\n";

        const QStringList replaygainFormats = pPluginLoader->formatList( PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );

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

        if( mimeTypes.count() > 0 )
        {
            QFile replaygainActionFile( KStandardDirs::locateLocal( "services", "ServiceMenus/add_replaygain_with_soundkonverter.desktop" ) );
            if( replaygainActionFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
            {
                QTextStream replaygainActionStream( &replaygainActionFile );
                replaygainActionStream << content;
                replaygainActionFile.close();
            }
        }
    }
    else
    {
        QFile::remove( KStandardDirs::locateLocal( "services", "ServiceMenus/convert_with_soundkonverter.desktop" ) );
        QFile::remove( KStandardDirs::locateLocal( "services", "ServiceMenus/add_replaygain_with_soundkonverter.desktop" ) );
    }
}

QStringList Config::customProfiles()
{
    QStringList profiles;

    for( int i=0; i<data.profiles.count(); i++ )
    {
        QList<CodecPlugin*> plugins = pPluginLoader->encodersForCodec( data.profiles.at(i).codecName );

        for( int j=0; j<plugins.count(); j++ )
        {
            if( plugins.at(j) && data.profiles.at(i).pluginName == plugins.at(j)->name() )
            {
                profiles += data.profiles.at(i).profileName;
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

    QStringList formats = pPluginLoader->formatList( PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain), PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    for( int i=0; i<formats.count(); i++ )
    {
        if( formats.at(i) == "wav" )
            continue;

        codecIndex = -1;
        for( int j=0; j<data.backends.codecs.count(); j++ )
        {
            if( data.backends.codecs.at(j).codecName == formats.at(i) )
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
            if( pPluginLoader->conversionPipeTrunks.at(j).codecTo == formats.at(i) && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                const QString pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
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
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == formats.at(i) &&
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
                    optimization.codecName = formats.at(i);
                    optimization.mode = CodecOptimizations::Optimization::Encode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).encoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = formats.at(i);
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
            if( pPluginLoader->conversionPipeTrunks.at(j).codecFrom == formats.at(i) && pPluginLoader->conversionPipeTrunks.at(j).enabled && pPluginLoader->conversionPipeTrunks.at(j).plugin->type() == "codec" )
            {
                const QString pluginName = pPluginLoader->conversionPipeTrunks.at(j).plugin->name();
                if( tempPluginList.filter(QRegExp("[0-9]{8,8}"+pluginName)).count() == 0 )
                {
                    tempPluginList += QString::number(pPluginLoader->conversionPipeTrunks.at(j).rating).rightJustified(8,'0') + pluginName;
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
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == formats.at(i) &&
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
                    optimization.codecName = formats.at(i);
                    optimization.mode = CodecOptimizations::Optimization::Decode;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).decoders.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = formats.at(i);
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
            if( pPluginLoader->replaygainPipes.at(j).codecName == formats.at(i) && pPluginLoader->replaygainPipes.at(j).enabled )
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
                if( data.backendOptimizationsIgnoreList.optimizationList.at(j).codecName == formats.at(i) &&
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
                    optimization.codecName = formats.at(i);
                    optimization.mode = CodecOptimizations::Optimization::ReplayGain;
                    optimization.currentBackend = data.backends.codecs.at(codecIndex).replaygain.first();
                    optimization.betterBackend = optimizedPluginList.first();
                    optimization.solution = CodecOptimizations::Optimization::Ignore;
                    optimizationList.append(optimization);
                }
                else if( !ignore )
                {
                    optimization.codecName = formats.at(i);
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

