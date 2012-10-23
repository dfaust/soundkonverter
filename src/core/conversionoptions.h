

#ifndef CONVERSIONOPTIONS_H
#define CONVERSIONOPTIONS_H

#include <KGenericFactory>

#include <QString>
#include <QDomElement>


/**
 * @short The options for the filter process can be stored here
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class KDE_EXPORT FilterOptions
{
public:
    FilterOptions();
    virtual ~FilterOptions();

    bool equalsBasics( FilterOptions *_other );
    virtual bool equals( FilterOptions *_other ); // checks if the other FilterOptions is equal to this

    virtual QDomElement toXml( QDomDocument document, const QString elementName );

    virtual bool fromXml( QDomElement filterOptions );

    QString pluginName;             // an identificator to see which plugin created the FilterOptions
                                    // NOTE this must be unique for each plugin!

    QString cmdArguments;           // user defined command line arguments
};


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
    bool equalsFilters( ConversionOptions *_other );
    virtual bool equals( ConversionOptions *_other ); // checks if the other ConversionOptions is equal to this

    virtual QDomElement toXml( QDomDocument document );

    virtual bool fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

    QString pluginName;             // an identificator to see which plugin created the ConversionOptions
                                    // NOTE this must be unique for each plugin!

    // plugin must ensure that these values are set, so an alternative plugin can be used
    enum QualityMode { Quality = 0, Bitrate = 1, Lossless = 2 } qualityMode;
    double quality;                 // plugin specific quaily value
    int bitrate;                    // set in order to fall back if the other plugin does not support the quality mode [kbit/s]
    enum BitrateMode { Vbr = 0, Abr = 1, Cbr = 2 } bitrateMode;

    QString cmdArguments;           // user defined command line arguments

    // plugin specific, may not be used by alternative plugins
    double compressionLevel;        // how hard should the encoder try

    // these values are set by OptionsDetailed::currentConversionOptions
    QString profile;                // the precalculated profile for the simple tab
    QString codecName;              // the output format (was outputFormat)

    int outputDirectoryMode;
    QString outputDirectory;
    QString outputFilesystem;       // the filesystem of the output directory

    bool replaygain;

    QList<FilterOptions*> filterOptions;
};

#endif // CONVERSIONOPTIONS_H
