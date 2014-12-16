
#ifndef SOUNDKONVERTERAPP_H
#define SOUNDKONVERTERAPP_H

#include <QApplication>

class SoundKonverter;

class soundKonverterApp : public QApplication
{
    Q_OBJECT

public:
    soundKonverterApp(int & argc, char ** argv);
    ~soundKonverterApp();

    /** This function is called, when a new instance of soundKonverter should be created */
    virtual int newInstance();

    SoundKonverter *mainWindow;
};

#endif // SOUNDKONVERTERAPP_H
