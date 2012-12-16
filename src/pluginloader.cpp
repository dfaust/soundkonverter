//
// C++ Implementation: pluginloader
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pluginloader.h"
#include "logger.h"
#include "config.h"

#include <QSet>
#include <QFile>

#include <KServiceTypeTrader>
#include <KMimeType>


bool moreThanConversionPipe( const ConversionPipe& pipe1, const ConversionPipe& pipe2 )
{
    int rating1 = 0;
    int rating2 = 0;

    int minimumRating1 = 0;
    foreach( const ConversionPipeTrunk trunk, pipe1.trunks )
    {
        if( minimumRating1 == 0 || trunk.rating < minimumRating1 )
            minimumRating1 = trunk.rating;

        rating1 -= 10;
    }
    rating1 += minimumRating1;

    int minimumRating2 = 0;
    foreach( const ConversionPipeTrunk trunk, pipe2.trunks )
    {
        if( minimumRating2 == 0 || trunk.rating < minimumRating2 )
            minimumRating2 = trunk.rating;

        rating2 -= 10;
    }
    rating2 += minimumRating2;

    return rating1 > rating2;
}

bool moreThanReplayGainPipe( const ReplayGainPipe& pipe1, const ReplayGainPipe& pipe2 )
{
    return pipe1.rating > pipe2.rating;
}


PluginLoader::PluginLoader( Logger *_logger, Config *_config )
    : logger( _logger ),
    config( _config )
{
    atomPath += "moov";
    atomPath += "trak";
    atomPath += "mdia";
    atomPath += "minf";
    atomPath += "stbl";
    atomPath += "stsd";
}

PluginLoader::~PluginLoader()
{
    qDeleteAll( codecPlugins );
    qDeleteAll( filterPlugins );
    qDeleteAll( replaygainPlugins );
    qDeleteAll( ripperPlugins );
}

void PluginLoader::addFormatInfo( const QString& codecName, BackendPlugin *plugin )
{
    BackendPlugin::FormatInfo info = plugin->formatInfo( codecName );

    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            if( formatInfos.at(i).description.isEmpty() )
                formatInfos[i].description = info.description;

            for( int j=0; j < info.mimeTypes.count(); j++ )
            {
                if( !formatInfos.at(i).mimeTypes.contains(info.mimeTypes.at(j)) )
                    formatInfos[i].mimeTypes.append( info.mimeTypes.at(j) );
            }
            for( int j=0; j < info.extensions.count(); j++ )
            {
                if( !formatInfos.at(i).extensions.contains(info.extensions.at(j)) )
                    formatInfos[i].extensions.append( info.extensions.at(j) );
            }

            if( formatInfos.at(i).lossless != info.lossless )
                logger->log( 1000, "<span style=\"color:red\">Disturbing Error: Plugin " + plugin->name() + " says " + codecName + " was " + (info.lossless?"lossless":"lossy") + " but it is already registed as " + (!info.lossless?"lossless":"lossy") + "</span>" );

            return;
        }
    }

    formatInfos.append( info );
}

void PluginLoader::load()
{
    QTime overallTime;
    overallTime.start();
    QTime createInstanceTime;

    int createInstanceTimeSum = 0;

    KService::List offers;

    logger->log( 1000, "\nloading plugins ..." );

    offers = KServiceTypeTrader::self()->query("soundKonverter/CodecPlugin");

    if( !offers.isEmpty() )
    {
        for( int i=0; i<offers.size(); i++ )
        {
            createInstanceTime.start();
            QVariantList allArgs;
            allArgs << offers.at(i)->storageId() << "";
            QString error;
            CodecPlugin *plugin = KService::createInstance<CodecPlugin>( offers.at(i), 0, allArgs, &error );
            if( plugin )
            {
                logger->log( 1000, "\tloading plugin: " + plugin->name() );
                createInstanceTimeSum += createInstanceTime.elapsed();
                codecPlugins.append( plugin );
                plugin->scanForBackends();
                QMap<QString,int> encodeCodecs;
                QMap<QString,int> decodeCodecs;
                QList<ConversionPipeTrunk> codecTable = plugin->codecTable();
                for( int j = 0; j < codecTable.count(); j++ )
                {
                    codecTable[j].plugin = plugin;
                    conversionPipeTrunks.append( codecTable.at(j) );
                    if( codecTable.at(j).codecTo != "wav" )
                        encodeCodecs[codecTable.at(j).codecTo] += codecTable.at(j).enabled;
                    if( codecTable.at(j).codecFrom != "wav" )
                        decodeCodecs[codecTable.at(j).codecFrom] += codecTable.at(j).enabled;
                    addFormatInfo( codecTable.at(j).codecFrom, plugin );
                    addFormatInfo( codecTable.at(j).codecTo, plugin );
                }
                if( encodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tencode:" );
                    for( int j=0; j<encodeCodecs.count(); j++ )
                    {
                        QString spaces;
                        spaces.fill( ' ', 12 - encodeCodecs.keys().at(j).length() );
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(encodeCodecs.keys().at(j)).arg(spaces).arg(encodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    }
                }
                if( decodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tdecode:" );
                    for( int j=0; j<decodeCodecs.count(); j++ )
                    {
                        QString spaces;
                        spaces.fill( ' ', 12 - decodeCodecs.keys().at(j).length() );
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(decodeCodecs.keys().at(j)).arg(spaces).arg(decodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    }
                }
                logger->log( 1000, "" );
            }
            else
            {
                logger->log( 1000, "<pre>\t<span style=\"color:red\">failed to load plugin: " + offers.at(i)->library() + "</span></pre>" );
            }
        }
    }

    offers = KServiceTypeTrader::self()->query("soundKonverter/FilterPlugin");

    if( !offers.isEmpty() )
    {
        for( int i=0; i<offers.size(); i++ )
        {
            createInstanceTime.start();
            QVariantList allArgs;
            allArgs << offers.at(i)->storageId() << "";
            QString error;
            FilterPlugin *plugin = KService::createInstance<FilterPlugin>( offers.at(i), 0, allArgs, &error );
            if( plugin )
            {
                logger->log( 1000, "\tloading plugin: " + plugin->name() );
                createInstanceTimeSum += createInstanceTime.elapsed();
                filterPlugins.append( plugin );
                plugin->scanForBackends();
                QMap<QString,int> encodeCodecs;
                QMap<QString,int> decodeCodecs;
                QList<ConversionPipeTrunk> codecTable = plugin->codecTable();
                for( int j = 0; j < codecTable.count(); j++ )
                {
                    codecTable[j].plugin = plugin;
                    filterPipeTrunks.append( codecTable.at(j) );
                    if( codecTable.at(j).codecTo != "wav" )
                        encodeCodecs[codecTable.at(j).codecTo] += codecTable.at(j).enabled;
                    if( codecTable.at(j).codecFrom != "wav" )
                        decodeCodecs[codecTable.at(j).codecFrom] += codecTable.at(j).enabled;
                    addFormatInfo( codecTable.at(j).codecFrom, plugin );
                    addFormatInfo( codecTable.at(j).codecTo, plugin );
                }
                if( encodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tencode:" );
                    for( int j=0; j<encodeCodecs.count(); j++ )
                    {
                        QString spaces;
                        spaces.fill( ' ', 12 - encodeCodecs.keys().at(j).length() );
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(encodeCodecs.keys().at(j)).arg(spaces).arg(encodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    }
                }
                if( decodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tdecode:" );
                    for( int j=0; j<decodeCodecs.count(); j++ )
                    {
                        QString spaces;
                        spaces.fill( ' ', 12 - decodeCodecs.keys().at(j).length() );
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(decodeCodecs.keys().at(j)).arg(spaces).arg(decodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    }
                }
                // TODO filters
                logger->log( 1000, "" );
            }
            else
            {
                logger->log( 1000, "<pre>\t<span style=\"color:red\">failed to load plugin: " + offers.at(i)->library() + "</span></pre>" );
            }
        }
    }

    offers = KServiceTypeTrader::self()->query("soundKonverter/ReplayGainPlugin");

    if( !offers.isEmpty() )
    {
        for( int i=0; i<offers.size(); i++ )
        {
            createInstanceTime.start();
            QVariantList allArgs;
            allArgs << offers.at(i)->storageId() << "";
            QString error;
            ReplayGainPlugin *plugin = KService::createInstance<ReplayGainPlugin>( offers.at(i), 0, allArgs, &error );
            if( plugin )
            {
                logger->log( 1000, "\tloading plugin: " + plugin->name() );
                createInstanceTimeSum += createInstanceTime.elapsed();
                replaygainPlugins.append( plugin );
                plugin->scanForBackends();
                QList<ReplayGainPipe> codecTable = plugin->codecTable();
                for( int j = 0; j < codecTable.count(); j++ )
                {
                    codecTable[j].plugin = plugin;
                    replaygainPipes.append( codecTable.at(j) );
                    QString spaces;
                    spaces.fill( ' ', 12 - codecTable.at(j).codecName.length() );
                    logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(codecTable.at(j).codecName).arg(spaces).arg(codecTable.at(j).enabled ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    addFormatInfo( codecTable.at(j).codecName, plugin );
                }
                logger->log( 1000, "" );
            }
            else
            {
                logger->log( 1000, "<pre>\t<span style=\"color:red\">failed to load plugin: " + offers.at(i)->library() + "</span></pre>" );
            }
        }
    }

    offers = KServiceTypeTrader::self()->query("soundKonverter/RipperPlugin");

    if( !offers.isEmpty() )
    {
        for( int i=0; i<offers.size(); i++ )
        {
            createInstanceTime.start();
            QVariantList allArgs;
            allArgs << offers.at(i)->storageId() << "";
            QString error;
            RipperPlugin *plugin = KService::createInstance<RipperPlugin>( offers.at(i), 0, allArgs, &error );
            if( plugin )
            {
                logger->log( 1000, "\tloading plugin: " + plugin->name() );
                createInstanceTimeSum += createInstanceTime.elapsed();
                ripperPlugins.append( plugin );
                plugin->scanForBackends();
                QList<ConversionPipeTrunk> codecTable = plugin->codecTable();
                for( int j = 0; j < codecTable.count(); j++ )
                {
                    codecTable[j].plugin = plugin;
                    conversionPipeTrunks.append( codecTable.at(j) );
                    QString spaces;
                    spaces.fill( ' ', 12 - codecTable.at(j).codecTo.length() );
                    logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3, %4)").arg(codecTable.at(j).codecTo).arg(spaces).arg(codecTable.at(j).enabled ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>").arg(codecTable.at(j).data.canRipEntireCd ? "<span style=\"color:green\">can rip to single file</span>" : "<span style=\"color:red\">can't rip to single file</span>") + "</pre>" );
                }
                logger->log( 1000, "" );
            }
            else
            {
                logger->log( 1000, "<pre>\t<span style=\"color:red\">failed to load plugin: " + offers.at(i)->library() + "</span></pre>" );
            }
        }
    }

    conversionFilterPipeTrunks = conversionPipeTrunks + filterPipeTrunks;

    logger->log( 1000, QString("... all plugins loaded (took %1 ms, creating instances: %2 ms)").arg(overallTime.elapsed()).arg(createInstanceTimeSum) + "\n" );
}

QStringList PluginLoader::formatList( Possibilities possibilities, CompressionType compressionType )
{
    QSet<QString> set;
    QStringList list;

    foreach( const ConversionPipeTrunk trunk, conversionFilterPipeTrunks )
    {
        if( !trunk.enabled )
            continue;

        if( possibilities & Encode )
        {
            const QString codec = trunk.codecTo;
            const bool isLossless = isCodecLossless(codec);
            const bool isInferiorQuality = isCodecInferiorQuality(codec);
//             const bool isHybrid = isCodecHybrid(codec);
            if( ( ( compressionType & Lossy && !isLossless ) || ( compressionType & Lossless && isLossless ) ) &&
                ( ( compressionType & InferiorQuality && isInferiorQuality ) || !isInferiorQuality ) )
                set += codec;
//             if( compressionType & Hybrid && isCodecHybrid(codec) && ( compressionType & InferiorQuality && isInferiorQuality || !isInferiorQuality ) )
//                 set += codec;
        }
        if( possibilities & Decode )
        {
            const QString codec = trunk.codecFrom;
            const bool isLossless = isCodecLossless(codec);
            const bool isInferiorQuality = isCodecInferiorQuality(codec);
//             const bool isHybrid = isCodecHybrid(codec);
            if( ( ( compressionType & Lossy && !isLossless ) || ( compressionType & Lossless && isLossless ) ) &&
                ( ( compressionType & InferiorQuality && isInferiorQuality ) || !isInferiorQuality ) )
                set += codec;
//             if( compressionType & Hybrid && isCodecHybrid(codec) && ( compressionType & InferiorQuality && isInferiorQuality || !isInferiorQuality ) )
//                 set += codec;
        }
    }

    if( possibilities & ReplayGain )
    {
        for( int i=0; i<replaygainPipes.count(); i++ )
        {
            if( !replaygainPipes.at(i).enabled )
                continue;

            set += replaygainPipes.at(i).codecName;
        }
    }

    list = set.toList();
    list.sort();

    QStringList importantCodecs;
    importantCodecs += "ogg vorbis";
    importantCodecs += "mp3";
    importantCodecs += "flac";
    importantCodecs += "m4a/aac";
    importantCodecs += "m4a/alac";
    importantCodecs += "wma";
    int listIterator = 0;
    for( int i=0; i<importantCodecs.count(); i++ )
    {
        if( list.contains(importantCodecs.at(i)) )
        {
            list.move( list.indexOf(importantCodecs.at(i)), listIterator++ );
        }
    }
    if( list.contains("wav") )
    {
        list.move( list.indexOf("wav"), list.count()-1 );
    }

    return list;
}

QList<CodecPlugin*> PluginLoader::encodersForCodec( const QString& codecName )
{
    QSet<CodecPlugin*> encoders;

    foreach( const ConversionPipeTrunk pipeTrunk, conversionFilterPipeTrunks )
    {
        if( pipeTrunk.codecTo == codecName && pipeTrunk.enabled )
        {
            encoders += qobject_cast<CodecPlugin*>(pipeTrunk.plugin);
        }
    }

    return encoders.toList();
}

// QList<CodecPlugin*> PluginLoader::decodersForCodec( const QString& codecName )
// {
//     QSet<CodecPlugin*> decoders;
//
//     for( int i=0; i<conversionPipeTrunks.count(); i++ )
//     {
//         if( conversionPipeTrunks.at(i).codecFrom == codecName && conversionPipeTrunks.at(i).enabled && conversionPipeTrunks.at(i).plugin->type() == "codec" )
//         {
//             decoders += qobject_cast<CodecPlugin*>(conversionPipeTrunks.at(i).plugin);
//         }
//     }
//
//     for( int i=0; i<filterPipeTrunks.count(); i++ )
//     {
//         if( filterPipeTrunks.at(i).codecFrom == codecName && filterPipeTrunks.at(i).enabled && filterPipeTrunks.at(i).plugin->type() == "filter" )
//         {
//             decoders += qobject_cast<CodecPlugin*>(filterPipeTrunks.at(i).plugin);
//         }
//     }
//
//     return decoders.toList();
// }

// QList<ReplayGainPlugin*> PluginLoader::replaygainForCodec( const QString& codecName )
// {
//     QSet<ReplayGainPlugin*> replaygain;
//
//     for( int i=0; i<replaygainPipes.count(); i++ )
//     {
//         if( replaygainPipes.at(i).codecName == codecName && replaygainPipes.at(i).enabled )
//         {
//             replaygain += replaygainPipes.at(i).plugin;
//         }
//     }
//
//     return replaygain.toList();
// }

BackendPlugin *PluginLoader::backendPluginByName( const QString& name )
{
    for( int i=0; i<codecPlugins.count(); i++ )
    {
        if( codecPlugins.at(i)->name() == name )
        {
            return codecPlugins.at(i);
        }
    }
    for( int i=0; i<filterPlugins.count(); i++ )
    {
        if( filterPlugins.at(i)->name() == name )
        {
            return filterPlugins.at(i);
        }
    }
    for( int i=0; i<replaygainPlugins.count(); i++ )
    {
        if( replaygainPlugins.at(i)->name() == name )
        {
            return replaygainPlugins.at(i);
        }
    }
    for( int i=0; i<ripperPlugins.count(); i++ )
    {
        if( ripperPlugins.at(i)->name() == name )
        {
            return ripperPlugins.at(i);
        }
    }

    return 0;
}

QList<ConversionPipe> PluginLoader::getConversionPipes( const QString& codecFrom, const QString& codecTo, QList<FilterOptions*> filterOptions, const QString& preferredPlugin )
{
    QList<ConversionPipe> list;

    QStringList decoders;
    QStringList encoders;
    // get the lists of decoders and encoders ordered by the user in the config dialog
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == codecFrom )
        {
            decoders = config->data.backends.codecs.at(i).decoders;
        }
        if( config->data.backends.codecs.at(i).codecName == codecTo && codecTo != "wav" )
        {
            encoders = config->data.backends.codecs.at(i).encoders;
        }
    }
    // prepend the preferred plugin
    if( !preferredPlugin.isEmpty() )
    {
        encoders.removeAll( preferredPlugin );
        encoders.prepend( preferredPlugin );
    }

    QList<FilterPlugin*> filterPlugins;
    foreach( FilterOptions *filter, filterOptions )
    {
        filterPlugins.append( qobject_cast<FilterPlugin*>(backendPluginByName(filter->pluginName)) );
    }

    // build all possible pipes
    for( int i=0; i<conversionFilterPipeTrunks.count(); i++ )
    {
        if( conversionFilterPipeTrunks.at(i).codecFrom == codecFrom && conversionFilterPipeTrunks.at(i).enabled )
        {
            if( codecFrom == "wav" && conversionFilterPipeTrunks.at(i).codecTo == codecTo )
            {
                ConversionPipe newPipe;

                foreach( FilterPlugin *plugin, filterPlugins )
                {
                    if( plugin == qobject_cast<FilterPlugin*>(conversionFilterPipeTrunks.at(i).plugin) )
                        continue;

                    foreach( ConversionPipeTrunk trunk, filterPipeTrunks )
                    {
                        if( trunk.plugin == plugin && trunk.codecFrom == "wav" && trunk.codecTo == "wav" && trunk.enabled )
                        {
                            newPipe.trunks += trunk;
                            break;
                        }
                    }
                }
                newPipe.trunks += conversionFilterPipeTrunks.at(i);

                if( encoders.indexOf(newPipe.trunks.last().plugin->name()) != -1 )
                {
                    // add rating depending on the position in the list ordered by the user, encoders do count much
                    const int rating = ( encoders.count() - encoders.indexOf(newPipe.trunks.last().plugin->name()) ) * 1000000;
                    for( int i=0; i<newPipe.trunks.count(); i++ )
                    {
                        newPipe.trunks[i].rating += rating;
                    }
                }

                list += newPipe;
            }
            else if( codecTo == "wav" && conversionFilterPipeTrunks.at(i).codecTo == codecTo )
            {
                ConversionPipe newPipe;

                newPipe.trunks += conversionFilterPipeTrunks.at(i);
                foreach( FilterPlugin *plugin, filterPlugins )
                {
                    if( plugin == qobject_cast<FilterPlugin*>(conversionFilterPipeTrunks.at(i).plugin) )
                        continue;

                    foreach( ConversionPipeTrunk trunk, filterPipeTrunks )
                    {
                        if( trunk.plugin == plugin && trunk.codecFrom == "wav" && trunk.codecTo == "wav" && trunk.enabled )
                        {
                            newPipe.trunks += trunk;
                            break;
                        }
                    }
                }

                if( decoders.indexOf(newPipe.trunks.first().plugin->name()) != -1 )
                {
                    // add rating depending on the position in the list ordered by the user, decoders don't count much
                    const int rating = ( decoders.count() - decoders.indexOf(newPipe.trunks.first().plugin->name()) ) * 1000;
                    for( int i=0; i<newPipe.trunks.count(); i++ )
                    {
                        newPipe.trunks[i].rating += rating;
                    }
                }

                list += newPipe;
            }
            else if( conversionFilterPipeTrunks.at(i).codecTo == codecTo )
            {
                if( filterPlugins.count() == 0 || ( filterPlugins.count() == 1 && filterPlugins.first() == conversionFilterPipeTrunks.at(i).plugin ) )
                {
                    ConversionPipe newPipe;

                    newPipe.trunks += conversionFilterPipeTrunks.at(i);
                    if( decoders.indexOf(newPipe.trunks.first().plugin->name()) != -1 )
                    {
                        // add rating depending on the position in the list ordered by the user, decoders don't count much
                        newPipe.trunks[0].rating += ( decoders.count() - decoders.indexOf(newPipe.trunks.first().plugin->name()) ) * 1000;
                    }
                    if( encoders.indexOf(newPipe.trunks.first().plugin->name()) != -1 )
                    {
                        // add rating depending on the position in the list ordered by the user, encoders do count much
                        newPipe.trunks[0].rating += ( encoders.count() - encoders.indexOf(newPipe.trunks.first().plugin->name()) ) * 1000000;
                    }

                    list += newPipe;
                }
            }
            else if( conversionFilterPipeTrunks.at(i).codecTo == "wav" )
            {
                if( conversionFilterPipeTrunks.at(i).codecFrom == conversionFilterPipeTrunks.at(i).codecTo )
                    continue;

                for( int j = 0; j < conversionFilterPipeTrunks.count(); j++ )
                {
                    if( i == j )
                        continue;

                    if( conversionFilterPipeTrunks.at(j).codecFrom == conversionFilterPipeTrunks.at(j).codecTo )
                        continue;

                    if( conversionFilterPipeTrunks.at(j).codecFrom == conversionFilterPipeTrunks.at(i).codecTo && conversionFilterPipeTrunks.at(j).codecTo == codecTo && conversionFilterPipeTrunks.at(j).enabled )
                    {
                        ConversionPipe newPipe;

                        newPipe.trunks += conversionFilterPipeTrunks.at(i);
                        foreach( FilterPlugin *plugin, filterPlugins )
                        {
                            if( plugin == qobject_cast<FilterPlugin*>(conversionFilterPipeTrunks.at(i).plugin) )
                                continue;

                            if( plugin == qobject_cast<FilterPlugin*>(conversionFilterPipeTrunks.at(j).plugin) )
                                continue;

                            foreach( ConversionPipeTrunk trunk, filterPipeTrunks )
                            {
                                if( trunk.plugin == plugin && trunk.codecFrom == "wav" && trunk.codecTo == "wav" && trunk.enabled )
                                {
                                    newPipe.trunks += trunk;
                                    break;
                                }
                            }
                        }
                        newPipe.trunks += conversionFilterPipeTrunks.at(j);

                        if( decoders.indexOf(newPipe.trunks.first().plugin->name()) != -1 )
                        {
                            // add rating depending on the position in the list ordered by the user, decoders don't count much
                            const int rating = ( decoders.count() - decoders.indexOf(newPipe.trunks.first().plugin->name()) ) * 1000;
                            for( int i=0; i<newPipe.trunks.count(); i++ )
                            {
                                newPipe.trunks[i].rating += rating;
                            }
                        }
                        if( encoders.indexOf(newPipe.trunks.last().plugin->name()) != -1 )
                        {
                            // add rating depending on the position in the list ordered by the user, encoders do count much
                            const int rating = ( encoders.count() - encoders.indexOf(newPipe.trunks.last().plugin->name()) ) * 1000000;
                            for( int i=0; i<newPipe.trunks.count(); i++ )
                            {
                                newPipe.trunks[i].rating += rating;
                            }
                        }

                        list += newPipe;
                    }
                }
            }
        }
    }

    qSort( list.begin(), list.end(), moreThanConversionPipe );

    return list;
}

QList<ReplayGainPipe> PluginLoader::getReplayGainPipes( const QString& codecName, const QString& preferredPlugin )
{
    QList<ReplayGainPipe> list;

    QStringList backends;
    // get the lists of decoders and encoders ordered by the user in the config dialog
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == codecName )
        {
            backends = config->data.backends.codecs.at(i).replaygain;
        }
    }
    // prepend the preferred plugin
    backends.removeAll( preferredPlugin );
    backends.prepend( preferredPlugin );

    for( int i=0; i<replaygainPipes.count(); i++ )
    {
        if( replaygainPipes.at(i).codecName == codecName && replaygainPipes.at(i).enabled )
        {
            ReplayGainPipe newPipe = replaygainPipes.at(i);
            if( backends.indexOf(newPipe.plugin->name()) != -1 )
            {
                // add rating depending on the position in the list ordered by the user
                newPipe.rating += ( backends.count() - backends.indexOf(newPipe.plugin->name()) ) * 1000;
            }
            list += newPipe;
        }
    }

    qSort( list.begin(), list.end(), moreThanReplayGainPipe );

    return list;
}

QString PluginLoader::getCodecFromM4aFile( QFile *file )
{
    int atomPathDepth = 0;

    qint64 maxPos = file->size();

    while( !file->atEnd() )
    {
        const QByteArray length = file->read(4);
        const QByteArray name = file->read(4);

        if( atomPathDepth == 6 && name == "mp4a" )
        {
            // It could be something other than aac but lets assume it's aac for now
            return "m4a/aac";
        }
        else if( atomPathDepth == 6 && name == "alac" )
        {
            return "m4a/alac";
        }
        else if( length.size() == 4 )
        {
            qint64 int_length = ((static_cast<qint64>(length.at(0)) & 0xFF) << 24) +
                                ((static_cast<qint64>(length.at(1)) & 0xFF) << 16) +
                                ((static_cast<qint64>(length.at(2)) & 0xFF) << 8) +
                                ((static_cast<qint64>(length.at(3)) & 0xFF) );

            if( int_length == 0 ) // Meaning: continues until end of file.
                return "";

            if( int_length == 1 ) // Meaning: length is 64 bits
            {
                const QByteArray l = file->read(8);
                int_length    = ((static_cast<qint64>(l.at(0)) & 0xFF) << 56) +
                                ((static_cast<qint64>(l.at(1)) & 0xFF) << 48) +
                                ((static_cast<qint64>(l.at(2)) & 0xFF) << 40) +
                                ((static_cast<qint64>(l.at(3)) & 0xFF) << 32) +
                                ((static_cast<qint64>(l.at(4)) & 0xFF) << 24) +
                                ((static_cast<qint64>(l.at(5)) & 0xFF) << 16) +
                                ((static_cast<qint64>(l.at(6)) & 0xFF) << 8) +
                                ((static_cast<qint64>(l.at(7)) & 0xFF) );
            }

            if( name == atomPath.at(atomPathDepth) )
            {
                atomPathDepth++;
                maxPos = file->pos() - 8 + int_length;

                if( atomPathDepth == 6 )
                    file->seek( file->pos() + 8 ); // Skip 'stsd' header
            }
            else
            {
                if( file->pos() - 8 + int_length > maxPos )
                    return "";

                file->seek( file->pos() - 8 + int_length );
            }
        }
        else
        {
            return "";
        }
    }

    return "";
}

QString PluginLoader::getCodecFromFile( const KUrl& filename, QString *mimeType, bool checkM4a )
{
    QString codec = "";
    short rating = 0;
    const QString mime = KMimeType::findByUrl(filename)->name();

    if( mimeType )
        *mimeType = mime;

    if( mime == "inode/directory" )
        return codec;

    const QString extension = filename.url().mid( filename.url().lastIndexOf(".") + 1 ).toLower();

    foreach( const BackendPlugin::FormatInfo info, formatInfos )
    {
        short newRating = info.priority;

        if( info.mimeTypes.contains(mime) )
            newRating += 100 - info.mimeTypes.indexOf(mime);

        if( info.extensions.contains(extension) )
            newRating += 100 - info.extensions.indexOf(extension);

        // special treatment for the mp4 family
        if( ( mime == "audio/mp4" || mime == "audio/x-m4a" ) && extension == "aac" && info.codecName == "m4a/aac" )
            newRating = 300;
        else if( ( mime == "audio/mp4" || mime == "audio/x-m4a" ) && extension == "alac" && info.codecName == "m4a/alac" )
            newRating = 300;

        if( newRating == info.priority )
            continue;

        if( newRating == 300 )
        {
            return info.codecName;
        }
        else if( newRating > rating )
        {
            rating = newRating;
            codec = info.codecName;
        }
    }

    // special treatment for the mp4 family
    if( checkM4a && ( mime == "audio/mp4" || mime == "audio/x-m4a" ) && filename.isLocalFile() )
    {
        QFile file( filename.toLocalFile() );
        if( file.open(QIODevice::ReadOnly) )
        {
            const QString newCodec = getCodecFromM4aFile( &file );

            file.close();

            if( !newCodec.isEmpty() )
                return newCodec;
        }
    }

    // get codec from a plugin - not used at the moment
    // QList<BackendPlugin*> allPlugins;
    // foreach( CodecPlugin *plugin, codecPlugins )
    //     allPlugins.append( plugin );
    // foreach( FilterPlugin *plugin, filterPlugins )
    //     allPlugins.append( plugin );
    // foreach( ReplayGainPlugin *plugin, replaygainPlugins )
    //     allPlugins.append( plugin );
    //
    // foreach( BackendPlugin *plugin, allPlugins )
    // {
    //     short newRating = 0;
    //     const QString newCodec = plugin->getCodecFromFile( filename, mime, &newRating );
    //     if( !newCodec.isEmpty() && newRating == 300 )
    //     {
    //         return newCodec;
    //     }
    //     else if( !newCodec.isEmpty() && newRating > rating )
    //     {
    //         rating = newRating;
    //         codec = newCodec;
    //     }
    // }

    return codec;
}

bool PluginLoader::canDecode( const QString& codecName, QStringList *errorList )
{
    if( codecName.isEmpty() )
        return false;

    for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
    {
        if( conversionFilterPipeTrunks.at(i).codecFrom == codecName && conversionFilterPipeTrunks.at(i).enabled )
        {
            return true;
        }
    }

    if( errorList )
    {
        for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
        {
            if( conversionFilterPipeTrunks.at(i).codecFrom == codecName )
            {
                if( !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && !errorList->contains(conversionFilterPipeTrunks.at(i).problemInfo) )
                {
                      errorList->append( conversionFilterPipeTrunks.at(i).problemInfo );
                }
            }
        }
    }

    return false;
}

bool PluginLoader::canReplayGain( const QString& codecName, CodecPlugin *plugin, QStringList *errorList )
{
    if( codecName.isEmpty() )
        return false;

    for( int i=0; i<replaygainPipes.count(); i++ )
    {
        if( replaygainPipes.at(i).codecName == codecName && replaygainPipes.at(i).enabled )
        {
            return true;
        }
    }
    if( plugin )
    {
        for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
        {
            if( conversionFilterPipeTrunks.at(i).plugin == plugin && conversionFilterPipeTrunks.at(i).codecTo == codecName && conversionFilterPipeTrunks.at(i).enabled && conversionFilterPipeTrunks.at(i).data.hasInternalReplayGain )
            {
                return true;
            }
        }
    }

    if( errorList )
    {
        // internal replaygain are not inlcuded in the error list
        for( int i=0; i<replaygainPipes.size(); i++ )
        {
            if( replaygainPipes.at(i).codecName == codecName )
            {
                if( !replaygainPipes.at(i).problemInfo.isEmpty() && !errorList->contains(replaygainPipes.at(i).problemInfo) )
                {
                      errorList->append( replaygainPipes.at(i).problemInfo );
                }
            }
        }
    }

    return false;
}

bool PluginLoader::canRipEntireCd( QStringList *errorList )
{
    for( int i=0; i<conversionFilterPipeTrunks.count(); i++ )
    {
        if( conversionFilterPipeTrunks.at(i).plugin->type() == "ripper" && conversionFilterPipeTrunks.at(i).data.canRipEntireCd && conversionFilterPipeTrunks.at(i).enabled )
        {
            return true;
        }
    }

    if( errorList )
    {
        for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
        {
            if( conversionFilterPipeTrunks.at(i).plugin->type() == "ripper" && conversionFilterPipeTrunks.at(i).data.canRipEntireCd )
            {
                if( !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && !errorList->contains(conversionFilterPipeTrunks.at(i).problemInfo) )
                {
                      errorList->append( conversionFilterPipeTrunks.at(i).problemInfo );
                }
            }
        }
    }

    return false;
}

QMap<QString,QStringList> PluginLoader::decodeProblems( bool detailed )
{
    QMap<QString,QStringList> problems;
    QStringList errorList;
    QStringList enabledCodecs;

    if( !detailed )
    {
        for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
        {
            if( conversionFilterPipeTrunks.at(i).enabled )
            {
                enabledCodecs += conversionFilterPipeTrunks.at(i).codecFrom;
            }
        }
    }

    for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
    {
        if( !conversionFilterPipeTrunks.at(i).enabled && !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && !problems.value(conversionFilterPipeTrunks.at(i).codecFrom).contains(conversionFilterPipeTrunks.at(i).problemInfo) && !enabledCodecs.contains(conversionFilterPipeTrunks.at(i).codecFrom) )
        {
            problems[conversionFilterPipeTrunks.at(i).codecFrom] += conversionFilterPipeTrunks.at(i).problemInfo;
        }
    }

    return problems;
}

QMap<QString,QStringList> PluginLoader::encodeProblems( bool detailed )
{
    QMap<QString,QStringList> problems;
    QStringList errorList;
    QStringList enabledCodecs;

    if( !detailed )
    {
        for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
        {
            if( conversionFilterPipeTrunks.at(i).enabled )
            {
                enabledCodecs += conversionFilterPipeTrunks.at(i).codecTo;
            }
        }
    }

    for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
    {
        if( !conversionFilterPipeTrunks.at(i).enabled && !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && !problems.value(conversionFilterPipeTrunks.at(i).codecTo).contains(conversionFilterPipeTrunks.at(i).problemInfo) && !enabledCodecs.contains(conversionFilterPipeTrunks.at(i).codecTo) )
        {
              problems[conversionFilterPipeTrunks.at(i).codecTo] += conversionFilterPipeTrunks.at(i).problemInfo;
        }
    }

    return problems;
}

QMap<QString,QStringList> PluginLoader::replaygainProblems( bool detailed )
{
    QMap<QString,QStringList> problems;
    QStringList errorList;
    QStringList enabledCodecs;

    if( !detailed )
    {
        for( int i=0; i<replaygainPipes.size(); i++ )
        {
            if( replaygainPipes.at(i).enabled )
            {
                enabledCodecs += replaygainPipes.at(i).codecName;
            }
        }
    }

    for( int i=0; i<replaygainPipes.size(); i++ )
    {
        if( !replaygainPipes.at(i).enabled && !replaygainPipes.at(i).problemInfo.isEmpty() && !problems.value(replaygainPipes.at(i).codecName).contains(replaygainPipes.at(i).problemInfo) && !enabledCodecs.contains(replaygainPipes.at(i).codecName) )
        {
              problems[replaygainPipes.at(i).codecName] += replaygainPipes.at(i).problemInfo;
        }
    }

    return problems;
}

QString PluginLoader::pluginDecodeProblems( const QString& pluginName, const QString& codecName )
{
    for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
    {
        if( !conversionFilterPipeTrunks.at(i).enabled && !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && conversionFilterPipeTrunks.at(i).plugin->name() == pluginName && conversionFilterPipeTrunks.at(i).codecFrom == codecName )
        {
              return conversionFilterPipeTrunks.at(i).problemInfo;
        }
    }

    return QString();
}

QString PluginLoader::pluginEncodeProblems( const QString& pluginName, const QString& codecName )
{
    for( int i=0; i<conversionFilterPipeTrunks.size(); i++ )
    {
        if( !conversionFilterPipeTrunks.at(i).enabled && !conversionFilterPipeTrunks.at(i).problemInfo.isEmpty() && conversionFilterPipeTrunks.at(i).plugin->name() == pluginName && conversionFilterPipeTrunks.at(i).codecTo == codecName )
        {
              return conversionFilterPipeTrunks.at(i).problemInfo;
        }
    }

    return QString();
}

QString PluginLoader::pluginReplayGainProblems( const QString& pluginName, const QString& codecName )
{
    for( int i=0; i<replaygainPipes.size(); i++ )
    {
        if( !replaygainPipes.at(i).enabled && !replaygainPipes.at(i).problemInfo.isEmpty() && replaygainPipes.at(i).plugin->name() == pluginName && replaygainPipes.at(i).codecName == codecName )
        {
              return replaygainPipes.at(i).problemInfo;
        }
    }

    return QString();
}

bool PluginLoader::isCodecLossless( const QString& codecName )
{
    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            return formatInfos.at(i).lossless;
        }
    }
    return false;
}

bool PluginLoader::isCodecInferiorQuality( const QString& codecName )
{
    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            return formatInfos.at(i).inferiorQuality;
        }
    }
    return false;
}

bool PluginLoader::isCodecHybrid( const QString& codecName )
{
    Q_UNUSED(codecName)

    return false;
}

bool PluginLoader::hasCodecInternalReplayGain( const QString& codecName )
{
    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( conversionPipeTrunks.at(i).codecTo == codecName && conversionPipeTrunks.at(i).plugin->type() == "codec" && conversionPipeTrunks.at(i).data.hasInternalReplayGain )
        {
            return true;
        }
    }
    return false;
}

QStringList PluginLoader::codecExtensions( const QString& codecName )
{
    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            QStringList extensions = formatInfos.at(i).extensions;

            if( codecName == "ogg vorbis" && config->data.general.preferredOggVorbisExtension == "oga" )
            {
                extensions.removeAll( "oga" );
                extensions.prepend( "oga" );
            }

            return extensions;
        }
    }
    return QStringList();
}

QStringList PluginLoader::codecMimeTypes( const QString& codecName )
{
    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            return formatInfos.at(i).mimeTypes;
        }
    }
    return QStringList();
}

QString PluginLoader::codecDescription( const QString& codecName )
{
    for( int i=0; i<formatInfos.count(); i++ )
    {
        if( formatInfos.at(i).codecName == codecName )
        {
            return formatInfos.at(i).description;
        }
    }
    return "";
}

