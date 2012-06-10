
#ifndef SOXFILTEROPTIONS_H
#define SOXFILTEROPTIONS_H

#include "../../core/conversionoptions.h"


class SoxFilterOptions : public FilterOptions
{
public:
    SoxFilterOptions();
    ~SoxFilterOptions();

    bool equals( FilterOptions *_other );
    QDomElement toXml( QDomElement _filterOptions );
    bool fromXml( QDomElement filterOptions );

    struct EffectData {
        QString effectName;
        QVariantList data;
    };

    struct Data {
        int sampleRate;
        int sampleSize;
        short channels;
        bool normalize;
        double normalizeVolume;
        QList<EffectData> effects;
    } data;
};

#endif // SOXFILTEROPTIONS_H
