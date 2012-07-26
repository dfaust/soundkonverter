
#include "replaygainplugin.h"


ReplayGainPluginItem::ReplayGainPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{
    data.fileCount = 0;
    data.processedFiles = 0;
    data.lastFileProgress = 0;
}

ReplayGainPluginItem::~ReplayGainPluginItem()
{}


ReplayGainPlugin::ReplayGainPlugin( QObject *parent )
    : BackendPlugin( parent )
{}

ReplayGainPlugin::~ReplayGainPlugin()
{}

QString ReplayGainPlugin::type()
{
    return "replaygain";
}

