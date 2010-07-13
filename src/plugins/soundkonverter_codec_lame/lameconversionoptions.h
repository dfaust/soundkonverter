
#ifndef LAMECONVERSIONOPTIONS_H
#define LAMECONVERSIONOPTIONS_H

#include "../../core/conversionoptions.h"

class LameConversionOptions : public ConversionOptions
{
public:
    LameConversionOptions();
    ~LameConversionOptions();

    bool equals( ConversionOptions *_other );
    QDomElement toXml( QDomDocument document );
    bool fromXml( QDomElement conversionOptions );

    struct Data {
        enum Preset {
            Medium = 0,
            Standard = 1,
            Extreme = 2,
            Insane = 3,
            SpecifyBitrate = 4,
            UserDefined = 5
        } preset;
        int presetBitrate;
        bool presetBitrateCbr;
        bool presetFast;
    } data;
};

#endif // LAMECONVERSIONOPTIONS_H