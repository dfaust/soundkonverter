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

//
// C++ Interface: optionslayer
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OPTIONSLAYER_H
#define OPTIONSLAYER_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <KUrl>

class Config;
class Options;
class ConversionOptions;

class QFrame;
class KPushButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class OptionsLayer : public QWidget
{
    Q_OBJECT
public:
    /** Constructor */
    OptionsLayer( Config *config, QWidget *parent );

    /** Destructor */
    ~OptionsLayer();

    void fadeIn();
    void fadeOut(); // should be private
    void addUrls( const KUrl::List& _urls );

private:
    QFrame *frame;
    Options *options;
    KPushButton *pOk;
    KPushButton *pCancel;

    QTimer fadeTimer;
    float fadeAlpha;
    int fadeMode; // 1 = fade in, 2 = fade out

    KUrl::List urls;
    QString command;

    inline QBrush brushSetAlpha( QBrush brush, const int alpha )
    {
        QColor color = brush.color();
        color.setAlpha( alpha );
        brush.setColor( color );
        return brush;
    }

public slots:
    /** Set the current profile */
    void setProfile( const QString& profile );

    /** Set the current format */
    void setFormat( const QString& format );

    /** Set the current output directory */
    void setOutputDirectory( const QString& directory );

    /** Set the command to execute after the conversion is complete */
    void setCommand( const QString& _command );

    /** Set the current conversion options */
    void setCurrentConversionOptions( ConversionOptions *options );

private slots:
    void fadeAnim();
    void abort();
    void ok();

signals:
    void done( const KUrl::List& urls, ConversionOptions *options, const QString& command );
    void saveFileList();

};

#endif
