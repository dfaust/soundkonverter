
#include "progressindicator.h"
#include "global.h"

#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolTip>

#include <KLocale>
#include <KSystemTrayIcon>
//#include <kiconloader.h>
//#include <kmessagebox.h>
// #include <kdebug.h>


ProgressIndicator::ProgressIndicator( /*KSystemTrayIcon* _systemTrayIcon,*/ QWidget* parent )
    : QWidget( parent )
{
//     systemTrayIcon = _systemTrayIcon;

    totalTime = processedTime = 0;

    QHBoxLayout *box = new QHBoxLayout( this );
    box->setContentsMargins( 0, 0, 0, 0 );

    pBar = new QProgressBar( this );
    box->addWidget( pBar );
    pBar->setRange( 0, 1 );
    pBar->setValue( 0 );
//     pBar->setFormat( "%v / %m" );
    box->addSpacing( 4 );

    QGridLayout *statusChildGrid = new QGridLayout();
    statusChildGrid->setContentsMargins( 0, 0, 0, 0 );
    box->addLayout( statusChildGrid );

    QLabel *lSpeedText = new QLabel( i18n("Speed")+":", this );
    statusChildGrid->addWidget( lSpeedText, 0, 0, Qt::AlignVCenter );

    lSpeed = new QLabel( " 0.0x", this );
    lSpeed->setFont( QFont( "Courier" ) );
    statusChildGrid->addWidget( lSpeed, 0, 1, Qt::AlignVCenter | Qt::AlignRight );
    speedTime.setHMS( 24, 0, 0 );

    QLabel *lTimeText = new QLabel( i18n("Remaining time")+":", this );
    statusChildGrid->addWidget( lTimeText, 1, 0, Qt::AlignVCenter );

    lTime = new QLabel( " 0s", this );
    lTime->setFont( QFont( "Courier" ) );
    statusChildGrid->addWidget( lTime, 1, 1, Qt::AlignVCenter | Qt::AlignRight );
    elapsedTime.setHMS( 24, 0, 0 );

//     QToolTip::add( systemTrayIcon, i18n("Waiting") );
}

ProgressIndicator::~ProgressIndicator()
{}

void ProgressIndicator::timeChanged( float time )
{
    totalTime += time;
    if( totalTime > 0 ) pBar->setRange( 0, (int)totalTime );
    else pBar->setRange( 0, 1 );
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
    lTime->setText( " 0s" );
    speedTime.setHMS( 24, 0, 0 );
    lSpeed->setText( " 0.0x" );
//     QToolTip::add( systemTrayIcon, i18n("Finished") );
    emit progressChanged( i18n("Finished") );
}

void ProgressIndicator::update( float time )
{
//     pBar->setRange( 0, (int)totalTime );
    pBar->setValue( (int)(processedTime + time) );
    float fPercent;
    if( pBar->maximum() > 0 ) fPercent = (float)pBar->value() * 100 / (float)pBar->maximum();
    else fPercent = 0.1f;

    if( !elapsedTime.isValid() ) elapsedTime.start();
    if( !speedTime.isValid() ) speedTime.start();
    if( speedTime.elapsed() > 1000 )
    {
        if( fPercent > 1.0f )
        {
            int tim = (int)( elapsedTime.elapsed()/1000/fPercent*(100-fPercent) + 1 );
            lTime->setText( Global::prettyNumber(tim,"s") );
            /*if( tim >= 0 && tim < 1000000 )
            {
                int sec = tim % 60;
                int min = ( tim / 60 ) % 60;
                int hou = tim / 3600;
                QString timeLeft;
                if( hou > 0 )
                    timeLeft.sprintf( "%d:%02d:%02d", hou, min, sec );
                else
                    timeLeft.sprintf( "%02d:%02d", min, sec );
                lTime->setText( timeLeft );
            }*/
        }

        int tim = speedTime.restart() / 1000;
        float speed = ( processedTime + time - speedProcessedTime ) / tim;
        speedProcessedTime = processedTime + time;
        if( speed >= 0.0f && speed < 100000.0f )
        {
            QString actSpeed;
            actSpeed.sprintf( "%.1fx", speed );
            if( speed < 10 ) actSpeed = " " + actSpeed;
            lSpeed->setText( actSpeed );
        }
    }

//     emit progressChanged( Global::prettyNumber(fPercent,"%",2) );
    emit progressChanged( QString::number((int)fPercent) + "%" );
//     QToolTip::add( systemTrayIcon, percent );
}

