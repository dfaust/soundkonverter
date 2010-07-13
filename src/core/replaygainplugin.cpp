
#include "replaygainplugin.h"


ReplayGainPluginItem::ReplayGainPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{}

ReplayGainPluginItem::~ReplayGainPluginItem()
{}


//
// class ReplayGainPlugin
//
////////////////////

ReplayGainPlugin::ReplayGainPlugin( QObject *parent )
    : BackendPlugin( parent )
{
    lastId = 0;
}

ReplayGainPlugin::~ReplayGainPlugin()
{}

QString ReplayGainPlugin::type()
{
    return "replaygain";
}

