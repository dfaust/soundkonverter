
#include "fluidsynthcodecglobal.h"

#include "fluidsynthcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>


FluidsynthCodecWidget::FluidsynthCodecWidget()
    : CodecWidget(),
    currentFormat( "midi" )
{}

FluidsynthCodecWidget::~FluidsynthCodecWidget()
{}

ConversionOptions *FluidsynthCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    return options;
}

bool FluidsynthCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    return true;
}

void FluidsynthCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString FluidsynthCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool FluidsynthCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int FluidsynthCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }

    return dataRate;
}

