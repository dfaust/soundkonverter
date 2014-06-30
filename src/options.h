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
