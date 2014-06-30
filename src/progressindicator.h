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



#ifndef PROGRESSINDICATOR_H
#define PROGRESSINDICATOR_H

#include <QWidget>
#include <QDateTime>

class QProgressBar;
class QLabel;


class TrailingAverage
{
public:
    TrailingAverage();
    ~TrailingAverage();

    void setCount( int _count );
    void addData( float _deltaTime, float _deltaValue );
    float average();

private:
    int count;
    QList<float> deltaTime;
    QList<float> deltaValue;
};

/**
 * @short Displays the current progress
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class ProgressIndicator : public QWidget
{
    Q_OBJECT
public:
    enum Feature
    {
        FeatureNone  = 0,
        FeatureSpeed = 1,
        FeatureTime  = 2
    };

    ProgressIndicator( QWidget *parent = 0, Feature features = FeatureNone );
    ~ProgressIndicator();

public slots:
    void timeChanged( float timeDelta );
    void timeFinished( float timeDelta );
    void finished( bool reset );

    void update( float timeProgress );

private:
    QProgressBar *pBar;
    QLabel *lSpeed;
    QLabel *lTime;

    float totalTime;
    float processedTime;

    QTime updateTime;
    float lastProcessedTime;
    TrailingAverage timeAverage;
    TrailingAverage speedAverage;

signals:
    void progressChanged( const QString& progress );
};

#endif // PROGRESSINDICATOR_H
