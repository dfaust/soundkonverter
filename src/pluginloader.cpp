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

#include <KServiceTypeTrader>
#include <KMimeType>


bool moreThanConversionPipe( const ConversionPipe& pipe1, const ConversionPipe& pipe2 )
{
    int rating1 = 0;
    int rating2 = 0;

    if( pipe1.trunks.count() == 1 )
    {
        rating1 = pipe1.trunks.at(0).rating + 10;
    }
    else
    {
        rating1 = ( pipe1.trunks.at(0).rating < pipe1.trunks.at(1).rating ) ? pipe1.trunks.at(0).rating : pipe1.trunks.at(1).rating;
        if( pipe1.trunks.at(0).codecTo == "wav" )
            rating1 += 5;
    }

    if( pipe2.trunks.count() == 1 )
    {
        rating2 = pipe2.trunks.at(0).rating + 10;
    }
    else
    {
        rating2 = ( pipe2.trunks.at(0).rating < pipe2.trunks.at(1).rating ) ? pipe2.trunks.at(0).rating : pipe2.trunks.at(1).rating;
        if( pipe2.trunks.at(0).codecTo == "wav" )
            rating2 += 5;
    }

    return rating1 > rating2;
}

bool moreThanReplayGainPipe( const ReplayGainPipe& pipe1, const ReplayGainPipe& pipe2 )
{
    return pipe1.rating > pipe2.rating;
}

//
// class PluginLoader
//
////////////////////

PluginLoader::PluginLoader( Logger *_logger, Config *parent )
    : QObject( parent ),
    logger( _logger ),
    config( parent )
{
    codecPlugins.clear();
    conversionPipeTrunks.clear();
}

PluginLoader::~PluginLoader()
{}

//
// private
//

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

//
// public
//

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
                QMap<QString,bool> encodeCodecs;
                QMap<QString,bool> decodeCodecs;
                QList<ConversionPipeTrunk> codecTable = plugin->codecTable();
                for( int j = 0; j < codecTable.count(); j++ )
                {
                    codecTable[j].plugin = plugin;
                    conversionPipeTrunks.append( codecTable.at(j) );
//                     logger->log( 1000, "\t\tfrom " + codecTable.at(j).codecFrom + " to " + codecTable.at(j).codecTo + " (rating: " + QString::number(codecTable.at(j).rating) + ", enabled: " + QString::number(codecTable.at(j).enabled) + ", hasInternalReplayGain: " + QString::number(codecTable.at(j).data.hasInternalReplayGain) + ")" );
                    if( codecTable.at(j).codecTo != "wav" && ( !encodeCodecs.contains(codecTable.at(j).codecTo) || !encodeCodecs[codecTable.at(j).codecTo] ) )
                        encodeCodecs[codecTable.at(j).codecTo] = codecTable.at(j).enabled;
                    if( codecTable.at(j).codecFrom != "wav" && ( !decodeCodecs.contains(codecTable.at(j).codecFrom) || !decodeCodecs[codecTable.at(j).codecFrom] ) )
                        decodeCodecs[codecTable.at(j).codecFrom] = codecTable.at(j).enabled;
                    addFormatInfo( codecTable.at(j).codecFrom, plugin );
                    addFormatInfo( codecTable.at(j).codecTo, plugin );
                }
                if( encodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tencode:" );
                    for( int j=0; j<encodeCodecs.count(); j++ )
                    {
                        const QString tabs = encodeCodecs.keys().at(j).length() >= 6 ? "\t" : "\t\t";
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(encodeCodecs.keys().at(j)).arg(tabs).arg(encodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
                    }
                }
                if( decodeCodecs.count() > 0 )
                {
                    logger->log( 1000, "\t\tdecode:" );
                    for( int j=0; j<decodeCodecs.count(); j++ )
                    {
                        const QString tabs = decodeCodecs.keys().at(j).length() >= 6 ? "\t" : "\t\t";
                        logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(decodeCodecs.keys().at(j)).arg(tabs).arg(decodeCodecs.values().at(j) ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
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
//                     logger->log( 1000, "\t\t" + codecTable.at(j).codecName + " (rating: " + QString::number(codecTable.at(j).rating) + ", enabled: " + QString::number(codecTable.at(j).enabled) + ")" );
                    const QString tabs = codecTable.at(j).codecName.length() >= 6 ? "\t" : "\t\t";
                    logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3)").arg(codecTable.at(j).codecName).arg(tabs).arg(codecTable.at(j).enabled ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>") + "</pre>" );
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
//                     logger->log( 1000, "\t\tfrom " + codecTable.at(j).codecFrom + " to " + codecTable.at(j).codecTo + " (rating: " + QString::number(codecTable.at(j).rating) + ", enabled: " + QString::number(codecTable.at(j).enabled) + ", canRipEntireCd: " + QString::number(codecTable.at(j).data.canRipEntireCd) + ")" );
                    const QString tabs = codecTable.at(j).codecTo.length() >= 6 ? "\t" : "\t\t";
                    logger->log( 1000, "<pre>\t\t\t" + QString("%1%2(%3, %4)").arg(codecTable.at(j).codecTo).arg(tabs).arg(codecTable.at(j).enabled ? "<span style=\"color:green\">enabled</span>" : "<span style=\"color:red\">disabled</span>").arg(codecTable.at(j).data.canRipEntireCd ? "<span style=\"color:green\">can rip to single file</span>" : "<span style=\"color:red\">can't rip to single file</span>") + "</pre>" );
                }
                logger->log( 1000, "" );
            }
            else
            {
                logger->log( 1000, "<pre>\t<span style=\"color:red\">failed to load plugin: " + offers.at(i)->library() + "</span></pre>" );
            }
        }
    }

    logger->log( 1000, QString("... all plugins loaded (took %1 ms, creating instances: %2 ms)").arg(overallTime.elapsed()).arg(createInstanceTimeSum) + "\n" );
}

QStringList PluginLoader::formatList( Possibilities possibilities, CompressionType compressionType )
{
    QSet<QString> set;
    QStringList list;

    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( !conversionPipeTrunks.at(i).enabled )
            continue;

        if( possibilities & Encode )
        {
            if( compressionType & Lossy && !isCodecLossless(conversionPipeTrunks.at(i).codecTo) ) set += conversionPipeTrunks.at(i).codecTo;
            if( compressionType & Lossless && isCodecLossless(conversionPipeTrunks.at(i).codecTo) ) set += conversionPipeTrunks.at(i).codecTo;
            if( compressionType & Hybrid && isCodecHybrid(conversionPipeTrunks.at(i).codecTo) ) set += conversionPipeTrunks.at(i).codecTo;
        }
        if( possibilities & Decode )
        {
            if( compressionType & Lossy && !isCodecLossless(conversionPipeTrunks.at(i).codecFrom) ) set += conversionPipeTrunks.at(i).codecFrom;
            if( compressionType & Lossless && isCodecLossless(conversionPipeTrunks.at(i).codecFrom) ) set += conversionPipeTrunks.at(i).codecFrom;
            if( compressionType & Hybrid && isCodecHybrid(conversionPipeTrunks.at(i).codecFrom) ) set += conversionPipeTrunks.at(i).codecFrom;
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
    importantCodecs += "m4a";
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

    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( conversionPipeTrunks.at(i).codecTo == codecName && conversionPipeTrunks.at(i).enabled && conversionPipeTrunks.at(i).plugin->type() == "codec" )
        {
            encoders += qobject_cast<CodecPlugin*>(conversionPipeTrunks.at(i).plugin);
        }
    }

    return encoders.toList();
}

QList<CodecPlugin*> PluginLoader::decodersForCodec( const QString& codecName )
{
    QSet<CodecPlugin*> decoders;

    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( conversionPipeTrunks.at(i).codecFrom == codecName && conversionPipeTrunks.at(i).enabled && conversionPipeTrunks.at(i).plugin->type() == "codec" )
        {
            decoders += qobject_cast<CodecPlugin*>(conversionPipeTrunks.at(i).plugin);
        }
    }

    return decoders.toList();
}

QList<ReplayGainPlugin*> PluginLoader::replaygainForCodec( const QString& codecName )
{
    QSet<ReplayGainPlugin*> replaygain;

    for( int i=0; i<replaygainPipes.count(); i++ )
    {
        if( replaygainPipes.at(i).codecName == codecName && replaygainPipes.at(i).enabled )
        {
            replaygain += replaygainPipes.at(i).plugin;
        }
    }

    return replaygain.toList();
}

BackendPlugin *PluginLoader::backendPluginByName( const QString& name )
{
    for( int i=0; i<codecPlugins.count(); i++ )
    {
        if( codecPlugins.at(i)->name() == name )
        {
            return codecPlugins.at(i);
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

QList<ConversionPipe> PluginLoader::getConversionPipes( const QString& codecFrom, const QString& codecTo, const QString& preferredPlugin )
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

    // build all possible pipes
    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( conversionPipeTrunks.at(i).codecFrom == codecFrom && conversionPipeTrunks.at(i).codecTo == codecTo && conversionPipeTrunks.at(i).enabled )
        {
            ConversionPipe newPipe;
            newPipe.trunks += conversionPipeTrunks.at(i);
            if( decoders.indexOf(newPipe.trunks.at(0).plugin->name()) != -1 )
            {
                // add rating depending on the position in the list ordered by the user, decoders don't count much
                newPipe.trunks[0].rating += ( decoders.count() - decoders.indexOf(newPipe.trunks.at(0).plugin->name()) ) * 1000;
            }
            if( encoders.indexOf(newPipe.trunks.at(0).plugin->name()) != -1 )
            {
                // add rating depending on the position in the list ordered by the user, encoders do count much
                newPipe.trunks[0].rating += ( encoders.count() - encoders.indexOf(newPipe.trunks.at(0).plugin->name()) ) * 1000000;
            }
            list += newPipe;
        }
        else if( conversionPipeTrunks.at(i).codecFrom == codecFrom && conversionPipeTrunks.at(i).codecTo == "wav" /*isCodecLossless(conversionPipeTrunks.at(i).codecTo)*/ && conversionPipeTrunks.at(i).enabled )
        {
            for( int j = 0; j < conversionPipeTrunks.count(); j++ )
            {
                if( i == j )
                    continue;

                if( conversionPipeTrunks.at(j).codecFrom == conversionPipeTrunks.at(i).codecTo && conversionPipeTrunks.at(j).codecTo == codecTo && conversionPipeTrunks.at(j).enabled )
                {
                    ConversionPipe newPipe;
                    newPipe.trunks += conversionPipeTrunks.at(i);
                    newPipe.trunks += conversionPipeTrunks.at(j);
                    if( decoders.indexOf(newPipe.trunks.at(0).plugin->name()) != -1 )
                    {
                        // add rating depending on the position in the list ordered by the user, decoders don't count much
                        newPipe.trunks[0].rating += ( decoders.count() - decoders.indexOf(newPipe.trunks.at(0).plugin->name()) ) * 1000;
                        newPipe.trunks[1].rating += ( decoders.count() - decoders.indexOf(newPipe.trunks.at(0).plugin->name()) ) * 1000;
                    }
                    if( encoders.indexOf(newPipe.trunks.at(1).plugin->name()) != -1 )
                    {
                        // add rating depending on the position in the list ordered by the user, encoders do count much
                        newPipe.trunks[0].rating += ( encoders.count() - encoders.indexOf(newPipe.trunks.at(1).plugin->name()) ) * 1000000;
                        newPipe.trunks[1].rating += ( encoders.count() - encoders.indexOf(newPipe.trunks.at(1).plugin->name()) ) * 1000000;
                    }
                    list += newPipe;
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

QString PluginLoader::getCodecFromFile( const KUrl& filename )
{
    QString codec = "";
    const QString mimeType = KMimeType::findByUrl(filename)->name();

    for( int i=0; i<codecPlugins.count(); i++ )
    {
        codec = codecPlugins.at(i)->getCodecFromFile( filename, mimeType );
        if( codec != "" )
            return codec;
    }

    for( int i=0; i<replaygainPlugins.count(); i++ )
    {
        codec = codecPlugins.at(i)->getCodecFromFile( filename, mimeType );
        if( codec != "" )
            return codec;
    }

    return mimeType;
}

bool PluginLoader::canDecode( const QString& codecName, QStringList *errorList )
{
    for( int i=0; i<conversionPipeTrunks.size(); i++ )
    {
        if( conversionPipeTrunks.at(i).codecFrom == codecName && conversionPipeTrunks.at(i).enabled )
        {
            return true;
        }
    }

    if( errorList )
    {
        for( int i=0; i<conversionPipeTrunks.size(); i++ )
        {
            if( conversionPipeTrunks.at(i).codecFrom == codecName )
            {
                if( !conversionPipeTrunks.at(i).problemInfo.isEmpty() && !errorList->contains(conversionPipeTrunks.at(i).problemInfo) )
                {
                      errorList->append( conversionPipeTrunks.at(i).problemInfo );
                }
            }
        }
    }

    return false;
}

bool PluginLoader::canReplayGain( const QString& codecName, CodecPlugin *plugin, QStringList *errorList )
{
    for( int i=0; i<replaygainPipes.count(); i++ )
    {
        if( replaygainPipes.at(i).codecName == codecName && replaygainPipes.at(i).enabled )
        {
            return true;
        }
    }
    if( plugin )
    {
        for( int i=0; i<conversionPipeTrunks.size(); i++ )
        {
            if( conversionPipeTrunks.at(i).plugin == plugin && conversionPipeTrunks.at(i).codecTo == codecName && conversionPipeTrunks.at(i).enabled && conversionPipeTrunks.at(i).data.hasInternalReplayGain )
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
    for( int i=0; i<conversionPipeTrunks.count(); i++ )
    {
        if( conversionPipeTrunks.at(i).plugin->type() == "ripper" && conversionPipeTrunks.at(i).data.canRipEntireCd && conversionPipeTrunks.at(i).enabled )
        {
            return true;
        }
    }

    if( errorList )
    {
        for( int i=0; i<conversionPipeTrunks.size(); i++ )
        {
            if( conversionPipeTrunks.at(i).plugin->type() == "ripper" && conversionPipeTrunks.at(i).data.canRipEntireCd )
            {
                if( !conversionPipeTrunks.at(i).problemInfo.isEmpty() && !errorList->contains(conversionPipeTrunks.at(i).problemInfo) )
                {
                      errorList->append( conversionPipeTrunks.at(i).problemInfo );
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
        for( int i=0; i<conversionPipeTrunks.size(); i++ )
        {
            if( conversionPipeTrunks.at(i).enabled )
            {
                enabledCodecs += conversionPipeTrunks.at(i).codecFrom;
            }
        }
    }

    for( int i=0; i<conversionPipeTrunks.size(); i++ )
    {
        if( !conversionPipeTrunks.at(i).enabled && !conversionPipeTrunks.at(i).problemInfo.isEmpty() && !problems.value(conversionPipeTrunks.at(i).codecFrom).contains(conversionPipeTrunks.at(i).problemInfo) && !enabledCodecs.contains(conversionPipeTrunks.at(i).codecFrom) )
        {
            problems[conversionPipeTrunks.at(i).codecFrom] += conversionPipeTrunks.at(i).problemInfo;
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
        for( int i=0; i<conversionPipeTrunks.size(); i++ )
        {
            if( conversionPipeTrunks.at(i).enabled )
            {
                enabledCodecs += conversionPipeTrunks.at(i).codecTo;
            }
        }
    }

    for( int i=0; i<conversionPipeTrunks.size(); i++ )
    {
        if( !conversionPipeTrunks.at(i).enabled && !conversionPipeTrunks.at(i).problemInfo.isEmpty() && !problems.value(conversionPipeTrunks.at(i).codecTo).contains(conversionPipeTrunks.at(i).problemInfo) && !enabledCodecs.contains(conversionPipeTrunks.at(i).codecTo) )
        {
              problems[conversionPipeTrunks.at(i).codecTo] += conversionPipeTrunks.at(i).problemInfo;
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
    for( int i=0; i<conversionPipeTrunks.size(); i++ )
    {
        if( !conversionPipeTrunks.at(i).enabled && !conversionPipeTrunks.at(i).problemInfo.isEmpty() && conversionPipeTrunks.at(i).plugin->name() == pluginName && conversionPipeTrunks.at(i).codecFrom == codecName )
        {
              return conversionPipeTrunks.at(i).problemInfo;
        }
    }

    return QString();
}

QString PluginLoader::pluginEncodeProblems( const QString& pluginName, const QString& codecName )
{
    for( int i=0; i<conversionPipeTrunks.size(); i++ )
    {
        if( !conversionPipeTrunks.at(i).enabled && !conversionPipeTrunks.at(i).problemInfo.isEmpty() && conversionPipeTrunks.at(i).plugin->name() == pluginName && conversionPipeTrunks.at(i).codecTo == codecName )
        {
              return conversionPipeTrunks.at(i).problemInfo;
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

