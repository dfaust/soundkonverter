
#ifndef NORMALIZEFILTEROPTIONS_H
#define NORMALIZEFILTEROPTIONS_H

#include "../../core/conversionoptions.h"


class NormalizeFilterOptions : public FilterOptions
{
public:
    NormalizeFilterOptions();
    ~NormalizeFilterOptions();

    bool equals( FilterOptions *_other );
    QDomElement toXml( QDomDocument document, const QString& elementName ) const;
    bool fromXml( QDomElement filterOptions );

    FilterOptions* copy() const;

    struct Data {
        bool normalize;
    } data;
};

#endif // NORMALIZEFILTEROPTIONS_H
