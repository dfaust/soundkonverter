
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
        QDomElement effect = document.createElement("effect"+QString::number(i++));
        effect.setAttribute("name",effectData.effectName);

        if( effectData.effectName == "norm" )
        {
            if( !effectData.data.isEmpty() )
                effect.setAttribute("normalizeVolume",effectData.data.at(0).toDouble());
        }
        else if( effectData.effectName == "bass" )
        {
            if( !effectData.data.isEmpty() )
                effect.setAttribute("bassGain",effectData.data.at(0).toDouble());
        }
        else if( effectData.effectName == "treble" )
        {
            if( !effectData.data.isEmpty() )
                effect.setAttribute("trebleGain",effectData.data.at(0).toDouble());
        }
        filterOptions.appendChild(effect);
    }

    return filterOptions;
}

bool SoxFilterOptions::fromXml( QDomElement filterOptions )
{
    FilterOptions::fromXml( filterOptions );
    data.sampleRate = filterOptions.attribute("sampleRate").toInt();
    data.sampleSize = filterOptions.attribute("sampleSize").toInt();
    data.channels = filterOptions.attribute("channels").toInt();

    return true;
}
