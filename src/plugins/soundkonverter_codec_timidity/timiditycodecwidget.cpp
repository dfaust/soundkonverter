
#include "timiditycodecglobal.h"

#include "timiditycodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>


TimidityCodecWidget::TimidityCodecWidget()
    : CodecWidget(),
    currentFormat( "midi" )
{}

TimidityCodecWidget::~TimidityCodecWidget()
{}

ConversionOptions *TimidityCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    return options;
}

bool TimidityCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    return true;
}

void TimidityCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString TimidityCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool TimidityCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int TimidityCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }

    return dataRate;
}

