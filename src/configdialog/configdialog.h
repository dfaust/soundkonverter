
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <KPageDialog>

class Config;
class ConfigGeneralPage;
class ConfigAdvancedPage;
class ConfigCoverArtPage;
class ConfigBackendsPage;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class ConfigDialog : public KPageDialog
{
    Q_OBJECT
public:
    ConfigDialog( Config *_config, QWidget *parent = 0/*, Page startPage = GeneralPage*/ );
    ~ConfigDialog();

private:
    Config *config;

    KPageWidgetItem *generalPage;
    ConfigGeneralPage *configGeneralPage;
    KPageWidgetItem *advancedPage;
    ConfigAdvancedPage *configAdvancedPage;
    KPageWidgetItem *coverArtPage;
    ConfigCoverArtPage *configCoverArtPage;
    KPageWidgetItem *backendsPage;
    ConfigBackendsPage *configBackendsPage;

    bool lastUseVFATNames;
    int lastConflictHandling;

    bool generalPageChanged;
    bool advancedlPageChanged;
    bool coverArtPageChanged;
    bool backendsPageChanged;

private slots:
    void configChanged( bool state );
    void okClicked();
    void applyClicked();
    void defaultClicked();

signals:
    void updateFileList();
};

#endif
