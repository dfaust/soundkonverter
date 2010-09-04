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
#include "core/replaygainplugin.h"
#include "core/ripperplugin.h"

#include <QObject>
#include <QStringList>
#include <KUrl>

class Logger;
class Config;


struct ConversionPipe
{
    QList<ConversionPipeTrunk> trunks;
//     QString tempCodec;
};


/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class PluginLoader : public QObject // TODO parent
{
    friend class Config;
  
    Q_OBJECT
public:
    PluginLoader( Logger *_logger, Config *parent );
    ~PluginLoader();

//     enum Possibilities { EncAble, DecAble, EncOrDecAble, ReplayGain }; // TODO make or'ed list
//     enum CompressionType { Lossy, Lossless, Hybrid, LossyOrLossless, LossyOrLosslessOrHybrid }; // TODO make or'ed list
    enum Possibilities { Encode = 1, Decode = 2, ReplayGain = 4 };
    enum CompressionType { Lossy = 1, Lossless = 2, Hybrid = 4 };

    void load();

    QStringList formatList( Possibilities, CompressionType );
    
    /** returns a list of encoder plugins for the given format (mime type or extension)
        the first value is the default encoder */
    QList<CodecPlugin*> encodersForCodec( const QString& codecName );
    QList<CodecPlugin*> decodersForCodec( const QString& codecName );
    QList<ReplayGainPlugin*> replaygainForCodec( const QString& codecName );

    /** returns the codec plugin with the specified name */
    CodecPlugin *codecPluginByName( const QString& name ); // TODO make it BackendPlugin *backendPluginByName ?

    /** returns a list of possible conversion pipes (TODO sorted by user and automatic rating) */
    QList<ConversionPipe> getConversionPipes( const QString& codecFrom, const QString& codecTo, const QString& preferredPlugin = "" ); // TODO change name
    /** returns a list of possible replay gain pipes for the codec */
    QList<ReplayGainPipe> getReplayGainPipes( const QString& codecName, const QString& preferredPlugin = "" );
    //** returns a list of possible rippers */
//     QList<RipperInfo> getRipperInfos( bool mustRipEntireCd = false, const QString& preferredPlugin = "" );

    QString getCodecFromFile( const KUrl& filename );

    bool canDecode( const QString& codecName, QStringList *errorList = 0 );
    bool canReplayGain( const QString& codecName, CodecPlugin *plugin, QStringList *errorList = 0 );
    bool canRipEntireCd( QStringList *errorList = 0 );
    QMap<QString,QStringList> decodeProblems();
    QMap<QString,QStringList> encodeProblems();
    QMap<QString,QStringList> replaygainProblems();
    bool isCodecLossless( const QString& codecName );
    bool isCodecHybrid( const QString& codecName );
    bool hasCodecInternalReplayGain( const QString& codecName );
    QStringList codecExtensions( const QString& codecName );
    QStringList codecMimeTypes( const QString& codecName );
    QString codecDescription( const QString& codecName );

    /** returns a list of all plugins e.g. for connecting their signals */
    QList<CodecPlugin*> getAllCodecPlugins() { return codecPlugins; }
    /** returns a list of all plugins e.g. for connecting their signals */
    QList<ReplayGainPlugin*> getAllReplayGainPlugins() { return replaygainPlugins; }
    /** returns a list of all plugins e.g. for connecting their signals */
    QList<RipperPlugin*> getAllRipperPlugins() { return ripperPlugins; }

private:
    Logger *logger;
    Config *config;

//     void addCodecItem( ConversionPipeTrunk trunk );
//     void addReplayGainItem( ReplayGainPipe pipe );
    void addFormatInfo( const QString& codecName, BackendPlugin *plugin );
    
    /** holds all known codec plugins */
    QList<CodecPlugin*> codecPlugins;
    /** holds all known replay gain plugins */
    QList<ReplayGainPlugin*> replaygainPlugins;
    /** holds all known ripper plugins */
    QList<RipperPlugin*> ripperPlugins;

    /** holds all known conversion pipe trunks */
    QList<ConversionPipeTrunk> conversionPipeTrunks;
    /** holds all known replay gain items */
    QList<ReplayGainPipe> replaygainPipes;

    /** holds all known format infos */
    QList<BackendPlugin::FormatInfo> formatInfos;
};

#endif
