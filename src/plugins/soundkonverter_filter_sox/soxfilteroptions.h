
#ifndef SOXFILTEROPTIONS_H
#define SOXFILTEROPTIONS_H

#include "../../core/conversionoptions.h"


class SoxFilterOptions : public FilterOptions
{
public:
    SoxFilterOptions();
    ~SoxFilterOptions();

    bool equals( FilterOptions *_other );
    QDomElement toXml( QDomDocument document, const QString elementName );
    bool fromXml( QDomElement filterOptions );

    struct EffectData {
        QString effectName;
        QVariantList data;
    };

    struct Data {
        bool sampleRateEnabled;
        int sampleRate;
        bool sampleSizeEnabled;
        int sampleSize;
        bool channelsEnabled;
        short channels;
        QList<EffectData> effects;
    } data;
};

#endif // SOXFILTEROPTIONS_H
