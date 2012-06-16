

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

// #include "conversionoptions.h"

class OptionsSimple;
class OptionsDetailed;
class Config;
class ConversionOptions;

class QStringList;
class KTabWidget;
class KPushButton;


/**
 * All we need to know about a profile
 */
// struct ProfileData
// {
//     QString name;
//     ConversionOptions options;
// };


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
    virtual ~Options();

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

    /** Toggle between normal and advanced options in the detailed tab */
    KPushButton *pAdvancedOptionsToggle;

    //** Notify the user about new plugins */
//     KPushButton* pPluginsNotify;
    /** Notify the user about new / lost backends */
    KPushButton *pBackendsNotify;

    KTabWidget *tab;

//     OutputDirectory* outputDirectory;

    OptionsSimple *optionsSimple;
    OptionsDetailed *optionsDetailed;

//     Config *config;

    // /** Copy all options from the detailed tab to the simple tab / find the matching profile */
    //void updateSimpleTab();


private slots:
    void tabChanged( const int pageIndex );
//     void somethingChanged();
//     void configChanged();
//     void showConfigDialogBackends();
    void simpleOutputDirectoryModeChanged(const int mode);
    void simpleOutputDirectoryChanged(const QString&);
    void simpleOptionsChanged();
    void detailedOutputDirectoryModeChanged(const int mode);
    void detailedOutputDirectoryChanged(const QString&);
//     void detailedOptionsChanged();

signals:
    void optionsChanged();
    void showConfigPluginsPage();
    void showConfigEnvironmentPage();
};

#endif // OPTIONS_H
