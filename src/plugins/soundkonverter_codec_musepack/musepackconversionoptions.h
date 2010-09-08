
#ifndef MUSEPACKCONVERSIONOPTIONS_H
#define MUSEPACKCONVERSIONOPTIONS_H

#include "../../core/conversionoptions.h"

class MusePackConversionOptions : public ConversionOptions
{
public:
    MusePackConversionOptions();
    ~MusePackConversionOptions();

    bool equals( ConversionOptions *_other );
    QDomElement toXml( QDomDocument document );
    bool fromXml( QDomElement conversionOptions );

    struct Data {
        enum Preset {
            Telephone = 0,
            Thumb = 1,
            Radio = 2,
            Standard = 3,
            Extreme = 4,
            Insane = 5,
            Braindead = 6,
            UserDefined = 7
        } preset;
    } data;
};

#endif // MUSEPACKCONVERSIONOPTIONS_H