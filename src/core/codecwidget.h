

#ifndef CODECWIDGET_H
#define CODECWIDGET_H

#include <QWidget>

#include <KGenericFactory>
#include <QDomDocument>

class ConversionOptions;

/**
 * @short The plugin specific widget for detailed options
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class KDE_EXPORT CodecWidget : public QWidget
{
    Q_OBJECT
public:
    CodecWidget();
    virtual ~CodecWidget();

    virtual ConversionOptions *currentConversionOptions() = 0;
    virtual bool setCurrentConversionOptions( ConversionOptions *_options ) = 0; // returns false if options are invalid (eg. different plugin)
    virtual void setCurrentFormat( const QString& format ) = 0;
    virtual QString currentProfile() = 0;
    virtual bool setCurrentProfile( const QString& profile ) = 0; // returns false if options are invalid (eg. plugin doesn't support hybrid)
    virtual QDomDocument customProfile() = 0;
    virtual bool setCustomProfile( const QString& profile, const QDomDocument& document ) = 0; // returns false if something goes wrong - whatever
    virtual int currentDataRate() = 0;

signals:
    void optionsChanged();
};

#endif // CODECWIDGET_H
