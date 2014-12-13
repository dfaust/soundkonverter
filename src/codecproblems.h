

#ifndef CODECPROBLEMS_H
#define CODECPROBLEMS_H

#include <QDialog>


/**
 * @short Shows a message box with possible solutions for backend problems
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class CodecProblems : public QDialog
{
    Q_OBJECT
public:
    enum Mode {
        Debug,
        Decode,
        ReplayGain,
        AudioCd
    };
    
    struct Problem {
        QString codecName;
        QStringList solutions;
        QStringList affectedFiles;
    };
    
    /** Default Constructor */
    CodecProblems( Mode mode, const QList<Problem>& problemList, QWidget *parent=0, Qt::WindowFlags f=0 );

    /** Default Destructor */
    ~CodecProblems();

};

#endif // CODECPROBLEMS_H
