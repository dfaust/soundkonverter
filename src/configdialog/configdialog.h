/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


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
