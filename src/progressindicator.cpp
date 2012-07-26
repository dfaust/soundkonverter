
#include "progressindicator.h"
#include "global.h"

#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolTip>

#include <KLocale>
#include <KGlobal>


ProgressIndicator::ProgressIndicator( QWidget *parent, Feature features )
    : QWidget( parent ),
    lSpeed( 0 ),
    lTime( 0 )
{
    totalTime = processedTime = 0;

    QHBoxLayout *box = new QHBoxLayout( this );
    box->setContentsMargins( 0, 0, 0, 0 );

    pBar = new QProgressBar( this );
    box->addWidget( pBar );
    pBar->setRange( 0, 1 );
    pBar->setValue( 0 );
    pBar->setFormat( "%v / %m" );

    if( features != 0 )
    {
        box->addSpacing( 4 );

        QGridLayout *statusChildGrid = new QGridLayout();
        statusChildGrid->setContentsMargins( 0, 0, 0, 0 );
        box->addLayout( statusChildGrid );

        if( features & FeatureSpeed )
        {
            QLabel *lSpeedText = new QLabel( i18n("Speed")+":", this );
            statusChildGrid->addWidget( lSpeedText, 0, 0, Qt::AlignVCenter );

            lSpeed = new QLabel( "<pre> 0.0x</pre>", this );
            statusChildGrid->addWidget( lSpeed, 0, 1, Qt::AlignVCenter | Qt::AlignRight );
            speedTime.setHMS( 24, 0, 0 );
        }

        if( features & FeatureTime )
        {
            QLabel *lTimeText = new QLabel( i18n("Remaining time")+":", this );
            statusChildGrid->addWidget( lTimeText, 1, 0, Qt::AlignVCenter );

            lTime = new QLabel( "<pre> 0s</pre>", this );
            lTime->setFont( QFont( "Courier" ) );
            statusChildGrid->addWidget( lTime, 1, 1, Qt::AlignVCenter | Qt::AlignRight );
            elapsedTime.setHMS( 24, 0, 0 );
        }
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
    pBar->setValue( (int)processedTime );
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

    if( lTime )
    {
        elapsedTime.setHMS( 24, 0, 0 );
        lTime->setText( "<pre> 0s</pre>" );
    }

    if( lSpeed )
    {
        speedTime.setHMS( 24, 0, 0 );
        lSpeed->setText( "<pre> 0.0x</pre>" );
    }

    emit progressChanged( i18n("Finished") );
}

void ProgressIndicator::update( float timeProgress )
{
    pBar->setValue( (int)(processedTime + timeProgress) );

    float fPercent;

    if( pBar->maximum() > 0 )
        fPercent = (float)pBar->value() * 100 / (float)pBar->maximum();
    else
        fPercent = 0.1f;

    if( lTime || lSpeed )
    {
        if( !elapsedTime.isValid() )
            elapsedTime.start();

        if( !speedTime.isValid() )
            speedTime.start();

        if( speedTime.elapsed() > 1000 )
        {
            if( fPercent > 1.0f && lTime )
            {
                const int time = (int)( elapsedTime.elapsed()/1000/fPercent*(100-fPercent) + 1 );
                lTime->setText( "<pre>" + Global::prettyNumber(time,"s") + "</pre>" );
            }

            if( lSpeed )
            {
                const int time = speedTime.restart() / 1000;
                const float speed = ( processedTime + timeProgress - speedProcessedTime ) / time;
                speedProcessedTime = processedTime + timeProgress;
                if( speed >= 0.0f && speed < 100000.0f )
                {
                    QString actSpeed;
                    actSpeed.sprintf( "%.1fx", speed );

                    if( speed < 10 )
                        actSpeed = " " + actSpeed;

                    if( KGlobal::locale()->decimalSymbol() != "." )
                        actSpeed.replace(".",KGlobal::locale()->decimalSymbol());

                    lSpeed->setText( "<pre>" + actSpeed + "</pre>" );
                }
            }
        }
    }

    emit progressChanged( QString::number((int)fPercent) + "%" );
}

