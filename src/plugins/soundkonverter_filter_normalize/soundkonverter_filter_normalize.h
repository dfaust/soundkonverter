
#ifndef SOUNDKONVERTER_FILTER_NORMALIZE_H
#define SOUNDKONVERTER_FILTER_NORMALIZE_H

#include "../../core/filterplugin.h"

class FilterOptions;


class soundkonverter_filter_normalize : public FilterPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_filter_normalize( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_filter_normalize();

    QString name();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    QWidget *newFilterWidget();

    int filter( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions );
    QStringList filterCommand( const KUrl& inputFile, const KUrl& outputFile, FilterOptions *_filterOptions );
    float parseOutput( const QString& output );
};

K_EXPORT_SOUNDKONVERTER_FILTER( normalize, soundkonverter_filter_normalize )


#endif // SOUNDKONVERTER_FILTER_NORMALIZE_H


