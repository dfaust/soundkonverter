
#include "config.h"
#include "logger.h"

#include <KConfigGroup>
#include <QDir>
#include <QDomElement>
#include <solid/device.h>
#include <kstandarddirs.h>


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
    data.general.useVFATNames = group.readEntry( "useVFATNames", true );
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
        
        for( int j=0; j<data.backends.codecs.count(); j++ )
        {
            if( data.backends.codecs.at(j).codecName == formats.at(i) )
            {
                codecIndex = j;
                break;
            }
        }

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

    QStringList profilesDirList = profilesDir.entryList();

    for( int i=0; i<profilesDirList.count(); i++ )
    {
        if( profilesDirList.at(i) == "." || profilesDirList.at(i) == ".." ) continue;

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
                logger->log( 1000, "\tfailed to load plugin: bad file format" );
            }
            profilesFile.close();
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

}

void Config::save()
{
    QString format;
    QStringList formats;
  
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group;
    
    group = conf->group( "General" );
    group.writeEntry( "configVersion", 1000 );
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
    group.writeEntry( "useVFATNames", data.general.useVFATNames );
    group.writeEntry( "conflictHandling", (int)data.general.conflictHandling );
//     group.writeEntry( "priority", data.general.priority );
    group.writeEntry( "numFiles", data.general.numFiles );
    group.writeEntry( "updateDelay", data.general.updateDelay );
//     group.writeEntry( "executeUserScript", data.general.executeUserScript );
//     group.writeEntry( "showToolBar", data.general.showToolBar );
//     group.writeEntry( "outputFilePermissions", data.general.outputFilePermissions );

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

