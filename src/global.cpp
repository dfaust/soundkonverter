//
// C++ Implementation: global
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "global.h"

#include <KLocale>
#include <KDebug>

Global::Global()
{}

Global::~Global()
{}


QString Global::prettyNumber( double num, QString unit, short digits )
{
    QString prettyString;
    if( unit == "%" && digits == 3 )
    {
        if( num < 10 ) prettyString.sprintf("%.2f %%",num);
        else if( num < 100 ) prettyString.sprintf("%.1f %%",num);
        else prettyString.sprintf("%.0f %%",num);
    }
    else if( unit == "%" && digits == 2 )
    {
        if( num < 10 ) prettyString.sprintf("%.1f %%",num);
        else prettyString.sprintf("%.0f %%",num);
    }
    else if( unit == "B" )
    {
        short pow = 0;
        while( num > 1024 )
        {
            num /= 1024;
            pow++;
        }
        if( pow == 0 ) unit = "B";
        else if( pow == 1 ) unit = "KiB";
        else if( pow == 2 ) unit = "MiB";
        else if( pow == 3 ) unit = "GiB";
        else if( pow == 4 ) unit = "TiB";
        else if( pow == 5 ) unit = "PiB";
        else if( pow == 6 ) unit = "EiB";
        else if( pow == 7 ) unit = "ZiB";
        else if( pow == 8 ) unit = "YiB";
        else unit = "Nan";
/*        if( num < 10 ) prettyString.sprintf("%.2f %s",num,unit);
        else if( num < 100 ) prettyString.sprintf("%.1f %s",num,unit);
        else prettyString.sprintf("%.0f %s",num,unit);*/
//         prettyString.sprintf("%.2f %s",9.76562,"KiB");
//         prettyString.sprintf("%f %s",num,unit);
//         prettyString = QString("%1 %2").arg(num).arg(unit);
//         prettyString = QString::number(num) + " " + unit;

        //if( num < 100 || num > 999 ) prettyString = QString("%1").arg(num).left(4);
        //else prettyString = QString("%1").arg(num).left(3);
        
        if( num < 100 || num > 999 ) prettyString = QString::number(num).left(4);
        else prettyString = QString::number(num).left(3);
        prettyString = prettyString + " " + unit;
    }
    else if( unit == "ms" )
    {
        short days = (int)(num/86400000.0f);
        short hours = (int)(num/3600000.0f)%24;
        short minutes = (int)(num/60000.0f)%60;
        short seconds = (int)(num/1000.0f)%60;
        short milliseconds = (int)num%1000;
/*        if( days >= 10 ) prettyString.sprintf("%i %s",days,i18n("d"));
        else if( days >= 1 ) prettyString.sprintf("%i %s, %02i %s",days,hours,i18n("d"),i18n("h"));
        else if( hours >= 1 ) prettyString.sprintf("%i %s, %02i %s",hours,minutes,i18n("h"),i18n("m"));
        else if( minutes >= 1 ) prettyString.sprintf("%i %s, %02i %s",minutes,seconds,i18n("m"),i18n("s"));
        else if( seconds >= 1 ) prettyString.sprintf("%i %s, %02i %s",seconds,milliseconds,i18n("s"),i18n("ms"));
        else if( milliseconds >= 1 ) prettyString.sprintf("%i %s",milliseconds,i18n("ms"));*/
        if( days >= 10 ) prettyString = QString("%1%2").arg(days).arg(i18nc("days","d"));
        else if( days >= 1 ) prettyString = QString("%1%2 %3%4").arg(days).arg(i18nc("days","d")).arg(hours).arg(i18nc("hours","h"));
        else if( hours >= 1 ) prettyString = QString("%1%2 %3%4").arg(hours).arg(i18nc("hours","h")).arg(minutes).arg(i18nc("minutes","m"));
        else if( minutes >= 1 && seconds < 10 ) prettyString = QString("%1%2 0%3%4").arg(minutes).arg(i18nc("minutes","m")).arg(seconds).arg(i18nc("seconds","s"));
        else if( minutes >= 1 ) prettyString = QString("%1%2 %3%4").arg(minutes).arg(i18nc("minutes","m")).arg(seconds).arg(i18nc("seconds","s"));
        else if( seconds >= 1 && milliseconds < 10 ) prettyString = QString("%1%2 00%3%4").arg(seconds).arg(i18nc("seconds","s")).arg(milliseconds).arg(i18nc("milliseconds","ms"));
        else if( seconds >= 1 && milliseconds < 100 ) prettyString = QString("%1%2 0%3%4").arg(seconds).arg(i18nc("seconds","s")).arg(milliseconds).arg(i18nc("milliseconds","ms"));
        else if( seconds >= 1 ) prettyString = QString("%1%2 %3%4").arg(seconds).arg(i18nc("seconds","s")).arg(milliseconds).arg(i18nc("milliseconds","ms"));
        else if( milliseconds >= 1 ) prettyString = QString("%1%2").arg(milliseconds).arg(i18nc("milliseconds","ms"));
    }
    else if( unit == "s" )
    {
        short days = (int)(num/86400);
        short hours = (int)(num/3600)%24;
        short minutes = (int)(num/60)%60;
        short seconds = (int)(num)%60;
        if( days >= 10 ) prettyString = QString("%1%2").arg(days).arg(i18nc("days","d"));
        else if( days >= 1 ) prettyString = QString("%1%2 %3%4").arg(days).arg(i18nc("days","d")).arg(hours).arg(i18nc("hours","h"));
        else if( hours >= 1 ) prettyString = QString("%1%2 %3%4").arg(hours).arg(i18nc("hours","h")).arg(minutes).arg(i18nc("minutes","m"));
        else if( minutes >= 1 && seconds < 10 ) prettyString = QString("%1%2 0%3%4").arg(minutes).arg(i18nc("minutes","m")).arg(seconds).arg(i18nc("seconds","s"));
        else if( minutes >= 1 ) prettyString = QString("%1%2 %3%4").arg(minutes).arg(i18nc("minutes","m")).arg(seconds).arg(i18nc("seconds","s"));
        else if( seconds >= 1 ) prettyString = QString("%1%2").arg(seconds).arg(i18nc("seconds","s"));
    }
    return prettyString;
}
