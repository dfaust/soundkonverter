//
// C++ Interface: config
//

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

#include "pluginloader.h"
#include "metadata/tagengine.h"
#include "conversionoptionsmanager.h"

#include <QDomDocument>

class Logger;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class Config : public QObject
{
    Q_OBJECT
public:
    struct CodecData {
        QString codecName;
        QStringList encoders;
        QStringList decoders;
        QStringList replaygain;
    };

    struct ProfileData {
        QString fileName;
        QString profileName;
        QString pluginName;
        QString codecName;
        QDomDocument data;
    };

    struct Data {
        struct General {
            int startTab;
            int lastTab;
            QString defaultProfile;
            QString lastProfile;
            QString defaultFormat;
            QString lastFormat;
            int lastOutputDirectoryMode;
            QString specifyOutputDirectory;
            QString metaDataOutputDirectory;
            QString copyStructureOutputDirectory;
//             int priority;
            bool waitForAlbumGain;
            bool useVFATNames;
            enum ConflictHandling {
                NewFileName = 0,
                Skip = 1,
                Overwrite = 2
            } conflictHandling;
            int numFiles;
            int updateDelay;
//             bool executeUserScript;
//             bool showToolBar;
//             int outputFilePermissions;
            bool createActionsMenu;
            bool removeFailedFiles;
        } general;
        struct Backends {
            QStringList rippers;
            QList<CodecData> codecs;
        } backends;
        QList<ProfileData> profiles;
        struct App {
            int configVersion;
        } app;
    } data;

    Config( Logger *_logger, QObject *parent );

    ~Config();

    void load();
    void save();
    
    /// returns a list of all working custom profiles
    QStringList customProfiles();

    PluginLoader *pluginLoader() { return pPluginLoader; }
    TagEngine *tagEngine() { return pTagEngine; }
    ConversionOptionsManager *conversionOptionsManager() { return pConversionOptionsManager; }

private:
    Logger *logger;

    PluginLoader *pPluginLoader;
    TagEngine *pTagEngine;
    ConversionOptionsManager *pConversionOptionsManager;
    
    void writeServiceMenu();
};

#endif
