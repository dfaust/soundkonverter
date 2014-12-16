
#ifndef CODECPROBLEMS_H
#define CODECPROBLEMS_H

#include <QDialog>

namespace Ui {
    class CodecProblems;
}

/** Shows a message box with possible solutions for backend problems */
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

    CodecProblems(Mode mode, const QList<Problem>& problemList, QWidget *parent, Qt::WindowFlags f=0);
    ~CodecProblems();

private:
    Ui::CodecProblems *ui;
};

#endif // CODECPROBLEMS_H
