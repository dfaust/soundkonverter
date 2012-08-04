
#include "conversionoptions.h"

#include "filterplugin.h"


FilterOptions::FilterOptions()
{}

FilterOptions::~FilterOptions()
{}

bool FilterOptions::equals( FilterOptions *_other )
{
    if( !_other )
        return false;

    return ( equalsBasics(_other) );
}

bool FilterOptions::equalsBasics( FilterOptions *_other )
{
    if( !_other )
        return false;

    return ( pluginName ==_other->pluginName &&
             cmdArguments ==_other->cmdArguments );
}

QDomElement FilterOptions::toXml( QDomDocument document, const QString elementName )
{
    QDomElement filterOptions = document.createElement(elementName);

    filterOptions.setAttribute("pluginName",pluginName);
    filterOptions.setAttribute("cmdArguments",cmdArguments);

    return filterOptions;
}

bool FilterOptions::fromXml( QDomElement filterOptions )
{
    pluginName = filterOptions.attribute("pluginName");
    return true;
}


ConversionOptions::ConversionOptions()
{}

ConversionOptions::~ConversionOptions()
{
    qDeleteAll( filterOptions );
}

bool ConversionOptions::equals( ConversionOptions *_other ) // TODO filter options
{
    if( !_other )
        return false;

    return ( equalsBasics(_other) &&
             qualityMode ==_other->qualityMode &&
             quality ==_other->quality &&
             bitrate ==_other->bitrate &&
             bitrateMode ==_other->bitrateMode &&
             bitrateMin ==_other->bitrateMin &&
             bitrateMax ==_other->bitrateMax &&
             samplingRate ==_other->samplingRate &&
             channels ==_other->channels );
}

bool ConversionOptions::equalsBasics( ConversionOptions *_other ) // TODO filter options
{
    if( !_other )
        return false;

    return ( pluginName ==_other->pluginName &&
             profile ==_other->profile &&
             codecName ==_other->codecName &&
             outputDirectoryMode ==_other->outputDirectoryMode &&
             outputDirectory ==_other->outputDirectory &&
             replaygain ==_other->replaygain &&
             cmdArguments ==_other->cmdArguments );
}

QDomElement ConversionOptions::toXml( QDomDocument document )
{
    QDomElement conversionOptions = document.createElement("conversionOptions");
    conversionOptions.setAttribute("pluginName",pluginName);
    conversionOptions.setAttribute("profile",profile);
    conversionOptions.setAttribute("codecName",codecName);
    QDomElement encodingOptions = document.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode",qualityMode);
    encodingOptions.setAttribute("quality",quality);
    encodingOptions.setAttribute("bitrate",bitrate);
    encodingOptions.setAttribute("bitrateMode",bitrateMode);
    encodingOptions.setAttribute("bitrateMin",bitrateMin);
    encodingOptions.setAttribute("bitrateMax",bitrateMax);
    encodingOptions.setAttribute("samplingRate",samplingRate);
    encodingOptions.setAttribute("channels",channels);
    encodingOptions.setAttribute("compressionLevel",compressionLevel);
    encodingOptions.setAttribute("cmdArguments",cmdArguments);
    conversionOptions.appendChild(encodingOptions);
    QDomElement outputOptions = document.createElement("outputOptions");
    outputOptions.setAttribute("outputDirectoryMode",outputDirectoryMode);
    outputOptions.setAttribute("outputDirectory",outputDirectory);
    outputOptions.setAttribute("outputFilesystem",outputFilesystem);
    conversionOptions.appendChild(outputOptions);
    QDomElement features = document.createElement("features");
    features.setAttribute("replaygain",replaygain);
    conversionOptions.appendChild(features);

    int i = 0;
    foreach( FilterOptions *filter, filterOptions )
    {
        QDomElement filterOptionsElement = filter->toXml(document,"filterOptions"+QString::number(i++));
        conversionOptions.appendChild(filterOptionsElement);
    }

    return conversionOptions;
}

bool ConversionOptions::fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    pluginName = conversionOptions.attribute("pluginName");
    profile = conversionOptions.attribute("profile");
    codecName = conversionOptions.attribute("codecName");
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    qualityMode = (QualityMode)encodingOptions.attribute("qualityMode").toInt();
    quality = encodingOptions.attribute("quality").toDouble();
    bitrate = encodingOptions.attribute("bitrate").toInt();
    bitrateMode = (BitrateMode)encodingOptions.attribute("bitrateMode").toInt();
    bitrateMin = encodingOptions.attribute("bitrateMin").toInt();
    bitrateMax = encodingOptions.attribute("bitrateMax").toInt();
    samplingRate = encodingOptions.attribute("samplingRate").toInt();
    channels = encodingOptions.attribute("channels").toInt();
    compressionLevel = encodingOptions.attribute("compressionLevel").toDouble();
    cmdArguments = encodingOptions.attribute("cmdArguments");
    QDomElement outputOptions = conversionOptions.elementsByTagName("outputOptions").at(0).toElement();
    outputDirectoryMode = outputOptions.attribute("outputDirectoryMode").toInt();
    outputDirectory = outputOptions.attribute("outputDirectory");
    outputFilesystem = outputOptions.attribute("outputFilesystem");
    QDomElement features = conversionOptions.elementsByTagName("features").at(0).toElement();
    replaygain = features.attribute("replaygain").toInt();

    if( filterOptionsElements )
    {
        for( QDomNode node = conversionOptions.firstChild(); !node.isNull(); node = node.nextSibling() )
        {
            if( node.nodeName().startsWith("filterOptions") )
                filterOptionsElements->append( node.toElement() );
        }
    }

    return true;
}

