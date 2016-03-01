
#include "musepackcodecglobal.h"

#include "musepackconversionoptions.h"

MusePackConversionOptions::MusePackConversionOptions()
    : ConversionOptions()
{
    pluginName = global_plugin_name;

    data.preset = Data::Standard;
}

MusePackConversionOptions::~MusePackConversionOptions()
{}

bool MusePackConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    MusePackConversionOptions *other = dynamic_cast<MusePackConversionOptions*>(_other);

    if( data.preset==other->data.preset && data.preset==Data::UserDefined )
    {
        return ConversionOptions::equals(_other);
    }
    else if( data.preset==other->data.preset )
    {
        return ( equalsBasics(_other) && equalsFilters(_other) );
    }
    else
    {
        return false;
    }
}

QDomElement MusePackConversionOptions::toXml( QDomDocument document ) const
{
    QDomElement conversionOptions = ConversionOptions::toXml( document );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = document.createElement("data");
    data.setAttribute("preset",MusePackConversionOptions::data.preset);
    encodingOptions.appendChild(data);

    return conversionOptions;
}

bool MusePackConversionOptions::fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions::fromXml( conversionOptions, filterOptionsElements );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    MusePackConversionOptions::data.preset = (Data::Preset)data.attribute("preset").toInt();
    return true;
}

ConversionOptions* MusePackConversionOptions::copy() const
{
    MusePackConversionOptions* c = new MusePackConversionOptions();
    c->pluginName = pluginName;
    c->qualityMode = qualityMode;
    c->quality = quality;
    c->bitrate = bitrate;
    c->bitrateMode = bitrateMode;
    c->cmdArguments = cmdArguments;
    c->compressionLevel = compressionLevel;
    c->profile = profile;
    c->codecName = codecName;
    c->outputDirectoryMode = outputDirectoryMode;
    c->outputDirectory = outputDirectory;
    c->outputFilesystem = outputFilesystem;
    c->replaygain = replaygain;

    foreach( const FilterOptions* f, filterOptions )
    {
        c->filterOptions.append(f->copy());
    }

    c->data.preset = data.preset;

    return static_cast<ConversionOptions*>(c);
}
