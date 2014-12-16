
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

#define SOUNDKONVERTER_VERSION_NUMBER 1005
#define SOUNDKONVERTER_VERSION_STRING "2.1.90"

class Global
{
public:
    Global();
    ~Global();

    static QString prettyNumber(double num, QString unit, short digits=3);
};

#endif
