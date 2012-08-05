
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
        int sampleRate; // 0 if disabled
        int sampleSize; // 0 if disabled
        short channels; // 0 if disabled
        QList<EffectData> effects;
    } data;
};

#endif // SOXFILTEROPTIONS_H
