

#ifndef CODECWIDGET_H
#define CODECWIDGET_H

#include <QWidget>

#include <QDomDocument>

class FilterOptions;
class ConversionOptions;

/** The plugin specific widget for detailed options */
class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    FilterWidget();
    virtual ~FilterWidget();

    virtual FilterOptions *currentFilterOptions() = 0;
    virtual bool setCurrentFilterOptions( FilterOptions *_options ) = 0; // returns false if options are invalid (eg. different plugin)

signals:
    void optionsChanged();
};


/** The plugin specific widget for detailed options */
class CodecWidget : public QWidget
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
    virtual int currentDataRate() = 0;

signals:
    void optionsChanged();
};

Q_DECLARE_INTERFACE(CodecWidget, "org.soundkonverter.codecwidget")

#endif // CODECWIDGET_H
