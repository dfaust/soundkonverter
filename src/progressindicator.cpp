
#include "progressindicator.h"
#include "global.h"

#include <QApplication>
#include <QLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolTip>

#include <KLocale>


TrailingAverage::TrailingAverage()
{
    count = 10;
}

TrailingAverage::~TrailingAverage()
{}

void TrailingAverage::setCount( int _count )
{
    count = _count;

    while( deltaTime.count() > count )
        deltaTime.removeFirst();

    while( deltaValue.count() > count )
        deltaValue.removeFirst();
}

void TrailingAverage::addData( float _deltaTime, float _deltaValue )
{
    while( deltaTime.count() > count )
        deltaTime.removeFirst();

    while( deltaValue.count() > count )
        deltaValue.removeFirst();

    deltaTime.append( _deltaTime );
    deltaValue.append( _deltaValue );
}

float TrailingAverage::average()
{
    float _deltaTime = 0;
    foreach( const float time, deltaTime )
        _deltaTime += time;

    float _deltaValue = 0;
    foreach( const float value, deltaValue )
        _deltaValue += value;

    return _deltaValue / _deltaTime;
}


ProgressIndicator::ProgressIndicator( QWidget *parent, Feature features )
    : QWidget( parent ),
    lSpeed( 0 ),
    lTime( 0 )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    totalTime = processedTime = 0;

    QHBoxLayout *box = new QHBoxLayout( this );
    box->setContentsMargins( 0, 0, 0, 0 );

    pBar = new QProgressBar( this );
    box->addWidget( pBar, 0, Qt::AlignVCenter );
    pBar->setRange( 0, 1 );
    pBar->setValue( 0 );

    if( features != 0 )
    {
        box->addSpacing( 0.4*fontHeight );

        QGridLayout *statusChildGrid = new QGridLayout();
        statusChildGrid->setContentsMargins( 0, 0, 0, 0 );
        box->addLayout( statusChildGrid );
        box->setAlignment( statusChildGrid, Qt::AlignVCenter );

        if( features & FeatureSpeed )
        {
            QLabel *lSpeedText = new QLabel( i18n("Speed:"), this );
            statusChildGrid->addWidget( lSpeedText, 0, 0, Qt::AlignVCenter );

            QString actSpeed = "  0x";

            lSpeed = new QLabel( "<pre>" + actSpeed + "</pre>", this );
            statusChildGrid->addWidget( lSpeed, 0, 1, Qt::AlignVCenter | Qt::AlignRight );

            speedAverage.setCount( 10 );
        }

        if( features & FeatureTime )
        {
            QLabel *lTimeText = new QLabel( i18n("Remaining time:"), this );
            statusChildGrid->addWidget( lTimeText, 1, 0, Qt::AlignVCenter );

            lTime = new QLabel( "<pre> 0s</pre>", this );
            lTime->setFont( QFont( "Courier" ) );
            statusChildGrid->addWidget( lTime, 1, 1, Qt::AlignVCenter | Qt::AlignRight );

            timeAverage.setCount( 60 );
        }

        updateTime.setHMS( 24, 0, 0 );
    }
}

ProgressIndicator::~ProgressIndicator()
{}

void ProgressIndicator::timeChanged( float timeDelta )
{
    totalTime += timeDelta;

    if( totalTime > 0 )
        pBar->setRange( 0, (int)totalTime );
    else
        pBar->setRange( 0, 1 );
}

void ProgressIndicator::timeFinished( float timeDelta )
{
    processedTime += timeDelta;
}

void ProgressIndicator::finished( bool reset )
{
    if( reset )
    {
        totalTime -= processedTime;
        if( totalTime < 0 )
            totalTime = 0.0f;
    }

    processedTime = 0.0f;

    pBar->setRange( 0, totalTime > 0 ? (int)totalTime : 1 );
    if( reset )
        pBar->setValue( totalTime > 0 ? 0 : 1 );
    else
        pBar->setValue( pBar->maximum() );

    updateTime.setHMS( 24, 0, 0 );

    if( lTime )
    {
        lTime->setText( "<pre> 0s</pre>" );
    }

    if( lSpeed )
    {
        QString actSpeed = "  0x";

        lSpeed->setText( "<pre>" + actSpeed + "</pre>" );
    }

    emit progressChanged( i18n("Finished") );
}

void ProgressIndicator::update( float timeProgress )
{
    const float currentProcessedTime = processedTime + timeProgress;

    pBar->setValue( (int)currentProcessedTime );

    const int iPercent = ( pBar->maximum() > 0 ) ? pBar->value() * 100 / pBar->maximum() : 0;

    if( lTime || lSpeed )
    {
        if( !updateTime.isValid() )
            updateTime.start();

        if( updateTime.elapsed() >= 1000 )
        {
            const float deltaTime = updateTime.restart() / 1000;

            if( lTime )
            {
                timeAverage.addData( deltaTime, currentProcessedTime - lastProcessedTime );
                const float remainingProcessTime = totalTime - currentProcessedTime;
                const float remainingTime = remainingProcessTime / timeAverage.average() + 1;

                lTime->setText( "<pre>" + Global::prettyNumber(remainingTime,"s") + "</pre>" );
            }

            if( lSpeed )
            {
                speedAverage.addData( deltaTime, currentProcessedTime - lastProcessedTime );
                const float speed = speedAverage.average();

                if( speed >= 0.0f && speed < 100000.0f )
                {
                    QString speedString = QString::number(qRound(speed)) + "x";

                    if( speed < 10 )
                        speedString = " " + speedString;

                    if( speed < 100 )
                        speedString = " " + speedString;

                    lSpeed->setText( "<pre>" + speedString + "</pre>" );
                }
            }

            lastProcessedTime = currentProcessedTime;
        }
    }

    emit progressChanged( QString::number(iPercent) + "%" );
}

