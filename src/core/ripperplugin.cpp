
#include "ripperplugin.h"


RipperPluginItem::RipperPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{
    data.fromSector = 0;
    data.toSector = 0;

    data.fileCount = 0;
    data.processedFiles = 0;
    data.lastFileProgress = 0;
}

RipperPluginItem::~RipperPluginItem()
{}


//
// class RipperPlugin
//
////////////////////

RipperPlugin::RipperPlugin( QObject *parent )
    : BackendPlugin( parent )
{}

RipperPlugin::~RipperPlugin()
{}

QString RipperPlugin::type()
{
    return "ripper";
}
