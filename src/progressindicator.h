

#ifndef PROGRESSINDICATOR_H
#define PROGRESSINDICATOR_H

#include <QWidget>
#include <QDateTime>

class QProgressBar;
class QLabel;

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

//private slots:

private:
    QProgressBar *pBar;
    QLabel *lSpeed;
    QLabel *lTime;

    QTime elapsedTime;
    QTime speedTime;
    float speedProcessedTime;

    float totalTime;
    float processedTime;

signals:
    void progressChanged( const QString& progress );
};

#endif // PROGRESSINDICATOR_H
