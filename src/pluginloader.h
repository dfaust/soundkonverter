 //
// C++ Interface: pluginloader
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "core/codecplugin.h"
#include "core/filterplugin.h"
#include "core/replaygainplugin.h"
#include "core/ripperplugin.h"

#include <QStringList>
#include <KUrl>

class Logger;
class Config;
class QFile;


struct ConversionPipe
{
    QList<ConversionPipeTrunk> trunks;

    bool operator==( const ConversionPipe& other )
    {
        if( trunks.count() != other.trunks.count() )
            return false;

        for( int i=0; i<trunks.count(); i++ )
        {
            ConversionPipeTrunk a = trunks.at(i);
            ConversionPipeTrunk b = other.trunks.at(i);
            if( !(a == b) )
                return false;
        }

        return true;
    }
};


/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class PluginLoader
{
    friend class Config;

public:
    PluginLoader( Logger *_logger, Config *_config );
    ~PluginLoader();

//     enum Possibilities { EncAble, DecAble, EncOrDecAble, ReplayGain }; // TODO make or'ed list
//     enum CompressionType { Lossy, Lossless, Hybrid, LossyOrLossless, LossyOrLosslessOrHybrid }; // TODO make or'ed list
    enum Possibilities { Encode = 1, Decode = 2, ReplayGain = 4 };
    enum CompressionType { InferiorQuality = 1, Lossy = 2, Lossless = 4, Hybrid = 8 };

    void load();

    QStringList formatList( Possibilities, CompressionType );

    /** returns a list of encoder plugins for the given format (mime type or extension)
        the first value is the default encoder */
    QList<CodecPlugin*> encodersForCodec( const QString& codecName );
//     QList<CodecPlugin*> decodersForCodec( const QString& codecName );
//     QList<ReplayGainPlugin*> replaygainForCodec( const QString& codecName );

    /** returns the codec plugin with the specified name */
    BackendPlugin *backendPluginByName( const QString& name );

    /** returns a list of possible conversion pipes */
    QList<ConversionPipe> getConversionPipes( const QString& codecFrom, const QString& codecTo, QList<FilterOptions*> filterOptions = QList<FilterOptions*>(), const QString& preferredPlugin = "" ); // TODO change name ?
    /** returns a list of possible replay gain pipes for the codec */
    QList<ReplayGainPipe> getReplayGainPipes( const QString& codecName, const QString& preferredPlugin = "" );
    //** returns a list of possible rippers */
//     QList<RipperInfo> getRipperInfos( bool mustRipEntireCd = false, const QString& preferredPlugin = "" );

    QString getCodecFromM4aFile( QFile *file );
    QString getCodecFromFile( const KUrl& filename, QString *mimeType = 0, bool checkM4a = true );

    bool canDecode( const QString& codecName, QStringList *errorList = 0 );
    bool canReplayGain( const QString& codecName, CodecPlugin *plugin, QStringList *errorList = 0 );
    bool canRipEntireCd( QStringList *errorList = 0 );
    QMap<QString,QStringList> decodeProblems( bool detailed = false );
    QMap<QString,QStringList> encodeProblems( bool detailed = false );
    QMap<QString,QStringList> replaygainProblems( bool detailed = false );
    QString pluginDecodeProblems( const QString& pluginName, const QString& codecName );
    QString pluginEncodeProblems( const QString& pluginName, const QString& codecName );
    QString pluginReplayGainProblems( const QString& pluginName, const QString& codecName );
    bool isCodecLossless( const QString& codecName );
    bool isCodecInferiorQuality( const QString& codecName );
    bool isCodecHybrid( const QString& codecName );
    bool hasCodecInternalReplayGain( const QString& codecName );
    QStringList codecExtensions( const QString& codecName );
    QStringList codecMimeTypes( const QString& codecName );
    QString codecDescription( const QString& codecName );

    /** returns a list of all plugins e.g. for connecting their signals */
    QList<CodecPlugin*> getAllCodecPlugins() { return codecPlugins; }
    /** returns a list of all plugins e.g. for connecting their signals */
    QList<FilterPlugin*> getAllFilterPlugins() { return filterPlugins; }
    /** returns a list of all plugins e.g. for connecting their signals */
    QList<ReplayGainPlugin*> getAllReplayGainPlugins() { return replaygainPlugins; }
    /** returns a list of all plugins e.g. for connecting their signals */
    QList<RipperPlugin*> getAllRipperPlugins() { return ripperPlugins; }

private:
    Logger *logger;
    Config *config;

    // for determining the codec of an m4a file
    QStringList atomPath;

//     void addCodecItem( ConversionPipeTrunk trunk );
//     void addReplayGainItem( ReplayGainPipe pipe );
    void addFormatInfo( const QString& codecName, BackendPlugin *plugin );

    /** holds all known codec plugins */
    QList<CodecPlugin*> codecPlugins;
    /** holds all known filter plugins */
    QList<FilterPlugin*> filterPlugins;
    /** holds all known replay gain plugins */
    QList<ReplayGainPlugin*> replaygainPlugins;
    /** holds all known ripper plugins */
    QList<RipperPlugin*> ripperPlugins;

    /** holds all known conversion pipe trunks */
    QList<ConversionPipeTrunk> conversionPipeTrunks;
    /** holds all known filter items */
    QList<ConversionPipeTrunk> filterPipeTrunks;
    /** holds all known conversion pipe trunks and filter items */
    QList<ConversionPipeTrunk> conversionFilterPipeTrunks;
    /** holds all known replay gain items */
    QList<ReplayGainPipe> replaygainPipes;

    /** holds all known format infos */
    QList<BackendPlugin::FormatInfo> formatInfos;
};

#endif
