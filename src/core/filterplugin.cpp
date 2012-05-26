
#include "filterplugin.h"
#include "codecwidget.h"


//
// class FilterPluginItem
//
////////////////////

FilterPluginItem::FilterPluginItem( QObject *parent )
    : BackendPluginItem( parent )
{}

FilterPluginItem::~FilterPluginItem()
{}


//
// class FilterPlugin
//
////////////////////

FilterPlugin::FilterPlugin( QObject *parent )
    : BackendPlugin( parent )
{
    lastUsedFilterOptions = 0;
}

FilterPlugin::~FilterPlugin()
{}

QString FilterPlugin::type()
{
    return "filter";
}

QWidget *FilterPlugin::deleteFilterWidget( QWidget *filterWidget )
{
    if( !filterWidget )
        return 0;

    if( lastUsedFilterOptions )
        delete lastUsedFilterOptions;

    lastUsedFilterOptions = qobject_cast<FilterWidget*>(filterWidget)->currentFilterOptions();
    delete filterWidget;

    return 0;
}

FilterOptions *FilterPlugin::filterOptionsFromXml( QDomElement filterOptions )
{
    FilterOptions *options = new FilterOptions();
    options->fromXml( filterOptions );
    return options;
}

