
#include "filterplugin.h"
#include "codecwidget.h"


//
// class FilterPluginItem
//
////////////////////

FilterPluginItem::FilterPluginItem( QObject *parent )
    : CodecPluginItem( parent )
{}

FilterPluginItem::~FilterPluginItem()
{}


//
// class FilterPlugin
//
////////////////////

FilterPlugin::FilterPlugin( QObject *parent )
    : CodecPlugin( parent )
{
    lastUsedFilterOptions = 0;
}

FilterPlugin::~FilterPlugin()
{}

QString FilterPlugin::type()
{
    return "filter";
}

FilterWidget *FilterPlugin::deleteFilterWidget( FilterWidget *filterWidget )
{
    if( !filterWidget )
        return 0;

    if( lastUsedFilterOptions )
        delete lastUsedFilterOptions;

    lastUsedFilterOptions = filterWidget->currentFilterOptions();
    delete filterWidget;

    return 0;
}

FilterOptions *FilterPlugin::filterOptionsFromXml( QDomElement filterOptions )
{
    FilterOptions *options = new FilterOptions();
    options->fromXml( filterOptions );
    return options;
}

