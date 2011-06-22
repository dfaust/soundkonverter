

#ifndef CODECOPTIMIZATIONS_H
#define CODECOPTIMIZATIONS_H

#include <KDialog>

class QRadioButton;


/**
 * @short Shows a message box with suggestions for optimizing the backend order
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class CodecOptimizations : public KDialog
{
    Q_OBJECT
public:
    struct Optimization {
        QString codecName;
        enum Mode {
            Encode,
            Decode,
            ReplayGain
        } mode;
        QString currentBackend;
        QString betterBackend;
        enum Solution {
            Fix,
            Ignore,
            Undecided
        } solution;
    };

    /** Default Constructor */
    CodecOptimizations( const QList<Optimization>& optimizationList, QWidget *parent=0, Qt::WFlags f=0 );

    /** Default Destructor */
    virtual ~CodecOptimizations();

private:
    QList<Optimization> optimizationList;
    QList<QRadioButton*> solutionFixButtons;

private slots:
    void okClicked();

signals:
    void solutions( const QList<CodecOptimizations::Optimization>& solutions );
};

#endif // CODECOPTIMIZATIONS_H
