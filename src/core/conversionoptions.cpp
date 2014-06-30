/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


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
{
    qualityMode = Quality;
    quality = 0;
    bitrate = 0;
    bitrateMode = Vbr;

    compressionLevel = 0;

    outputDirectoryMode = 0;

    replaygain = false;
}

ConversionOptions::~ConversionOptions()
{
    qDeleteAll( filterOptions );
}

bool ConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other )
        return false;

    if( !equalsBasics(_other) )
        return false;

    if( qualityMode !=_other->qualityMode )
        return false;
    if( quality !=_other->quality )
        return false;
    if( bitrate !=_other->bitrate )
        return false;
    if( bitrateMode !=_other->bitrateMode )
        return false;

    if( !equalsFilters(_other) )
        return false;

    return true;
}

bool ConversionOptions::equalsFilters( ConversionOptions *_other )
{
    if( !_other )
        return false;

    QStringList filters;
    foreach( FilterOptions *filter, filterOptions )
    {
        filters.append( filter->pluginName );
    }
    filters.sort();

    QStringList other_filters;
    foreach( FilterOptions *otherFilter, _other->filterOptions )
    {
        other_filters.append( otherFilter->pluginName );
    }
    other_filters.sort();

    if( filters == other_filters )
    {
        foreach( FilterOptions *filter, filterOptions )
        {
            foreach( FilterOptions *otherFilter, _other->filterOptions )
            {
                if( otherFilter->pluginName == filter->pluginName )
                {
                    if( !filter->equals(otherFilter) )
                        return false;
                    break;
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool ConversionOptions::equalsBasics( ConversionOptions *_other )
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

