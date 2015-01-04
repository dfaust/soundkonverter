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
#include <QUrl>

class Config;
class Options;
class ConversionOptions;

class QFrame;
class QPushButton;

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
    void addUrls( const QList<QUrl>& _urls );

private:
    QFrame *frame;
    Options *options;
    QPushButton *pOk;
    QPushButton *pCancel;

    QTimer fadeTimer;
    float fadeAlpha;
    int fadeMode; // 1 = fade in, 2 = fade out

    QList<QUrl> urls;
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
    void setCommand( const QString& command );

    /** Set the current conversion options */
    void setCurrentConversionOptions( ConversionOptions *options );

private slots:
    void fadeAnim();
    void abort();
    void ok();

signals:
    void done( const QList<QUrl>& urls, ConversionOptions *options, const QString& command );
    void saveFileList();

};

#endif
