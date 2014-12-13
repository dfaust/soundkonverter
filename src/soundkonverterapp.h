

#ifndef SOUNDKONVERTERAPP_H
#define SOUNDKONVERTERAPP_H


#include <QApplication>

class soundKonverter;

/**
 * @short The soundKonverter application. It controlles ensures that there can only run one instance of soundKonverter.
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class soundKonverterApp : public QApplication
{
    Q_OBJECT
public:
    /** Constructor */
    soundKonverterApp(int & argc, char ** argv);

    /** Destructor */
    ~soundKonverterApp();

    /** This function is called, when a new instance of soundKonverter should be created */
    virtual int newInstance();

// private:
    soundKonverter *mainWindow;
};

#endif // SOUNDKONVERTERAPP_H
