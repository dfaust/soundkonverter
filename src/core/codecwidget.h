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



#ifndef CODECWIDGET_H
#define CODECWIDGET_H

#include <QWidget>

#include <KGenericFactory>
#include <QDomDocument>

class FilterOptions;
class ConversionOptions;


/**
 * @short The plugin specific widget for detailed options
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class KDE_EXPORT FilterWidget : public QWidget
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
    virtual int currentDataRate() = 0;

signals:
    void optionsChanged();
};

#endif // CODECWIDGET_H
