
#ifndef DIRDIALOG_H
#define DIRDIALOG_H

#include <QDialog>

class Config;
class ConversionOptions;

namespace Ui {
    class DirOpener;
}

class DirOpener : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        Convert    = 0x0001,
        ReplayGain = 0x0002
    };

    DirOpener(Config *config, Mode _mode, QWidget *parent=0, Qt::WindowFlags f=0);
    ~DirOpener();

    /** true if the file dialog was aborted (don't execute the dialog) */
    bool dialogAborted;

private slots:
    void proceedClicked();
    void okClicked();
    void selectAllClicked();
    void selectNoneClicked();
    void showHelp();

private:
    Ui::DirOpener *ui;

    Config *config;

    /** the dialog mode */
    Mode mode;

signals:
    void open(const QUrl& directory, bool recursive, const QStringList& codecList, ConversionOptions *conversionOptions=0);
};

#endif
