
#include "conversionoptions.h"


ConversionOptions::ConversionOptions()
{}

ConversionOptions::~ConversionOptions()
{}

bool ConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other ) return false;
    return ( equalsBasics(_other) && qualityMode==_other->qualityMode && quality==_other->quality && bitrate==_other->bitrate && bitrateMode==_other->bitrateMode &&
             bitrateMin==_other->bitrateMin && bitrateMax==_other->bitrateMax && samplingRate==_other->samplingRate && channels==_other->channels );
}

bool ConversionOptions::equalsBasics( ConversionOptions *_other )
{
    if( !_other ) return false;
    return ( pluginName==_other->pluginName && profile==_other->profile && codecName==_other->codecName &&
             outputDirectoryMode==_other->outputDirectoryMode && outputDirectory==_other->outputDirectory &&
             replaygain==_other->replaygain && cmdArguments==_other->cmdArguments ); // && bpm==_other->bpm
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
    conversionOptions.appendChild(outputOptions);
    QDomElement features = document.createElement("features");
    features.setAttribute("replaygain",replaygain);
    conversionOptions.appendChild(features);
    
    return conversionOptions;
}

bool ConversionOptions::fromXml( QDomElement conversionOptions )
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
    QDomElement features = conversionOptions.elementsByTagName("features").at(0).toElement();
    replaygain = features.attribute("replaygain").toInt();
    return true;
}
















