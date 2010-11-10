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
#include <KUrl>

class Config;
class Options;
class ConversionOptions;

class QFrame;
class QPropertyAnimation;
class KPushButton;

/**
	@author Daniel Faust <hessijames@gmail.com>
*/
class OptionsLayer : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )
    
public:
    /** Constructor */
    OptionsLayer( Config *config, QWidget *parent );

    /** Destructor */
    virtual ~OptionsLayer();

    void fadeIn();
    void fadeOut(); // should be private
    void addUrls( const KUrl::List& _urls );

private:
    QFrame *frame;
    Options *options;
    KPushButton *pOk;
    KPushButton *pCancel;
    
    QPropertyAnimation *aAnimation;
    qreal rOpacity;

    KUrl::List urls;
    QString command;
    
    qreal opacity();
    void setOpacity( qreal opacity );

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

private slots:
    void animationFinished();
    void abort();
    void ok();
    
signals:
    void done( const KUrl::List& urls, ConversionOptions *options, const QString& command );

};

#endif
