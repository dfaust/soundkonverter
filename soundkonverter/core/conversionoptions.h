

#ifndef CONVERSIONOPTIONS_H
#define CONVERSIONOPTIONS_H

#include <KGenericFactory>

#include <QString>
#include <QDomElement>

/**
 * @short The options for the conversion process can be stored here
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class KDE_EXPORT ConversionOptions
{
public:
    ConversionOptions();
    virtual ~ConversionOptions();

    bool equalsBasics( ConversionOptions *_other );
    virtual bool equals( ConversionOptions *_other ); // checks if the other ConversionOptions is equal to this
    
    virtual QDomElement toXml( QDomDocument document );
    
    virtual bool fromXml( QDomElement conversionOptions );

    QString pluginName;             // an idetificator to see which plugin created the ConversionOptions
                                    // NOTE this must be unique for each plugin!

    // plugin must ensure that these values are set, so an alternative plugin can be used
    enum QualityMode { Quality = 0, Bitrate = 1, Lossless = 2 } qualityMode;
    double quality;                 // set both values in order to fail back if the other plugin does not support the quality mode
    int bitrate;                    // "
    enum BitrateMode { Vbr = 0, Abr = 1, Cbr = 2 } bitrateMode;
    int bitrateMin, bitrateMax;     // set to 0 to disable, only used when qualityMode==Bitrate and BitrateMode!=Cbr
    int samplingRate;               // in Hz, set to 0 to disable
    int channels;                   // number of channels, set to 0 to disable
    
    // plugin specific, may not be used by alternative plugins
    double compressionLevel;        // how hard should the encoder try

    // these values are set by OptionsDetailed::currentConversionOptions
    QString profile;                // the precalculated profile for the simple tab
    QString codecName;              // the output format (was outputFormat)
    int outputDirectoryMode;
    QString outputDirectory;
    bool replaygain;
    bool bpm;
};

#endif // CONVERSIONOPTIONS_H
