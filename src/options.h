

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

class OptionsSimple;
class OptionsDetailed;
class Config;
class ConversionOptions;

class KTabWidget;


/**
 * @short The widget, where we can set our output options
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class Options : public QWidget
{
    Q_OBJECT
public:
    /** Constructor */
    Options( Config *_config, const QString& text, QWidget *parent=0 );

    /** Destructor */
    ~Options();

    /** Return the current options */
    ConversionOptions *currentConversionOptions();

    /** Set the current options */
    bool setCurrentConversionOptions( ConversionOptions* optionsptions );

public slots:
    /** Set the current profile */
    void setProfile( const QString& profile );

    /** Set the current format */
    void setFormat( const QString& format );

    /** Set the current output directory mode */
    void setOutputDirectoryMode( int mode );

    /** Set the current output directory */
    void setOutputDirectory( const QString& directory );

    /** Should be called if the conversion options have been accepted by the user */
    void accepted();

private:
    Config *config;

    KTabWidget *tab;

    OptionsSimple *optionsSimple;
    OptionsDetailed *optionsDetailed;


private slots:
    void tabChanged( const int pageIndex );
    void simpleOutputDirectoryModeChanged(const int mode);
    void simpleOutputDirectoryChanged(const QString&);
    void simpleOptionsChanged();
    void detailedOutputDirectoryModeChanged(const int mode);

};

#endif // OPTIONS_H
