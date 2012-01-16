
#include "progressindicator.h"
#include "global.h"

#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolTip>

#include <KLocale>
#include <KGlobal>


ProgressIndicator::ProgressIndicator( QWidget* parent )
    : QWidget( parent )
{
    totalTime = processedTime = 0;

    QHBoxLayout *box = new QHBoxLayout( this );
    box->setContentsMargins( 0, 0, 0, 0 );

    pBar = new QProgressBar( this );
    box->addWidget( pBar );
    pBar->setRange( 0, 1 );
    pBar->setValue( 0 );
    box->addSpacing( 4 );

    QGridLayout *statusChildGrid = new QGridLayout();
    statusChildGrid->setContentsMargins( 0, 0, 0, 0 );
    box->addLayout( statusChildGrid );

    QLabel *lSpeedText = new QLabel( i18n("Speed")+":", this );
    statusChildGrid->addWidget( lSpeedText, 0, 0, Qt::AlignVCenter );

    lSpeed = new QLabel( "<pre> 0.0x</pre>", this );
    statusChildGrid->addWidget( lSpeed, 0, 1, Qt::AlignVCenter | Qt::AlignRight );
    speedTime.setHMS( 24, 0, 0 );

    QLabel *lTimeText = new QLabel( i18n("Remaining time")+":", this );
    statusChildGrid->addWidget( lTimeText, 1, 0, Qt::AlignVCenter );

    lTime = new QLabel( "<pre> 0s</pre>", this );
    lTime->setFont( QFont( "Courier" ) );
    statusChildGrid->addWidget( lTime, 1, 1, Qt::AlignVCenter | Qt::AlignRight );
    elapsedTime.setHMS( 24, 0, 0 );
}

ProgressIndicator::~ProgressIndicator()
{}

void ProgressIndicator::timeChanged( float time )
{
    totalTime += time;

    if( totalTime > 0 )
        pBar->setRange( 0, (int)totalTime );
    else
        pBar->setRange( 0, 1 );
}

void ProgressIndicator::timeFinished( float time )
{
    processedTime += time;
    pBar->setValue( (int)processedTime );
}

void ProgressIndicator::finished( float time )
{
    totalTime = time;
    processedTime = 0;
    pBar->setRange( 0, (totalTime>0) ? (int)totalTime : 1 );
    pBar->setValue( (totalTime>0) ? 0 : 1 );
    elapsedTime.setHMS( 24, 0, 0 );
    lTime->setText( "<pre> 0s</pre>" );
    speedTime.setHMS( 24, 0, 0 );
    lSpeed->setText( "<pre> 0.0x</pre>" );
    emit progressChanged( i18n("Finished") );
}

void ProgressIndicator::update( float time )
{
    pBar->setValue( (int)(processedTime + time) );

    float fPercent;

    if( pBar->maximum() > 0 )
        fPercent = (float)pBar->value() * 100 / (float)pBar->maximum();
    else
        fPercent = 0.1f;

    if( !elapsedTime.isValid() )
        elapsedTime.start();

    if( !speedTime.isValid() )
        speedTime.start();

    if( speedTime.elapsed() > 1000 )
    {
        if( fPercent > 1.0f )
        {
            int tim = (int)( elapsedTime.elapsed()/1000/fPercent*(100-fPercent) + 1 );
            lTime->setText( "<pre>" + Global::prettyNumber(tim,"s") + "</pre>" );
        }

        int tim = speedTime.restart() / 1000;
        float speed = ( processedTime + time - speedProcessedTime ) / tim;
        speedProcessedTime = processedTime + time;
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

    emit progressChanged( QString::number((int)fPercent) + "%" );
}

