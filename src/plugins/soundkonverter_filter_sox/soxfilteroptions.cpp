
#include "soxfilterglobal.h"

#include "soxfilteroptions.h"
#include "../../core/conversionoptions.h"


SoxFilterOptions::SoxFilterOptions()
{
    pluginName = global_plugin_name;
}

SoxFilterOptions::~SoxFilterOptions()
{}

bool SoxFilterOptions::equals( FilterOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    if( !FilterOptions::equals( _other ) )
        return false;

    SoxFilterOptions *other = dynamic_cast<SoxFilterOptions*>(_other);
    if( !other )
        return false;

    if( data.sampleRate != other->data.sampleRate )
        return false;
    if( data.sampleSize != other->data.sampleSize )
        return false;
    if( data.channels != other->data.channels )
        return false;

    QStringList effects;
    foreach( EffectData effectData, data.effects )
    {
        effects.append( effectData.effectName );
    }
    effects.sort();

    QStringList other_effects;
    foreach( EffectData otherEffectData, other->data.effects )
    {
        other_effects.append( otherEffectData.effectName );
    }
    other_effects.sort();

    if( effects == other_effects )
    {
        foreach( const EffectData effectData, data.effects )
        {
            foreach( const EffectData otherEffectData, other->data.effects )
            {
                if( otherEffectData.effectName == effectData.effectName )
                {
                    if( otherEffectData.data != effectData.data )
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

QDomElement SoxFilterOptions::toXml( QDomDocument document, const QString elementName )
{
    QDomElement filterOptions = FilterOptions::toXml( document, elementName );
    filterOptions.setAttribute("sampleRate",data.sampleRate);
    filterOptions.setAttribute("sampleSize",data.sampleSize);
    filterOptions.setAttribute("channels",data.channels);

    int i = 0;
    foreach( const EffectData effectData, data.effects )
    {
        if( effectData.effectName == i18n("Disabled") )
            continue;

        QDomElement effectElement = document.createElement("effect"+QString::number(i++));
        effectElement.setAttribute("name",effectData.effectName);

        if( effectData.effectName == "norm" )
        {
            if( !effectData.data.isEmpty() )
                effectElement.setAttribute("normalizeVolume",effectData.data.at(0).toDouble());
        }
        else if( effectData.effectName == "bass" )
        {
            if( !effectData.data.isEmpty() )
                effectElement.setAttribute("bassGain",effectData.data.at(0).toDouble());
        }
        else if( effectData.effectName == "treble" )
        {
            if( !effectData.data.isEmpty() )
                effectElement.setAttribute("trebleGain",effectData.data.at(0).toDouble());
        }
        filterOptions.appendChild(effectElement);
    }

    return filterOptions;
}

bool SoxFilterOptions::fromXml( QDomElement filterOptions )
{
    FilterOptions::fromXml( filterOptions );
    data.sampleRate = filterOptions.attribute("sampleRate").toInt();
    data.sampleSize = filterOptions.attribute("sampleSize").toInt();
    data.channels = filterOptions.attribute("channels").toInt();

    for( QDomNode node = filterOptions.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        if( node.nodeName().startsWith("effect") )
        {
            QDomElement effectElement = node.toElement();
            SoxFilterOptions::EffectData effectData;
            effectData.effectName = effectElement.attribute("name");

            if( effectData.effectName == "norm" )
            {
                effectData.data.append( effectElement.attribute("normalizeVolume").toDouble() );
            }
            else if( effectData.effectName == "bass" )
            {
                effectData.data.append( effectElement.attribute("bassGain").toDouble() );
            }
            else if( effectData.effectName == "treble" )
            {
                effectData.data.append( effectElement.attribute("trebleGain").toDouble() );
            }
            data.effects.append( effectData );
        }
    }

    return true;
}
