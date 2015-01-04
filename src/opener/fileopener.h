
#ifndef FILEOPENER_H
#define FILEOPENER_H

#include "ui_optionsdialog.h"

#include <QDialog>

#include <QUrl>

class Config;
class QLabel;
class ConversionOptions;
class QDialog;
class QFileDialog;

class FileOpener : public QDialog
{
    Q_OBJECT
public:
    FileOpener( Config *_config, QWidget *parent=0, Qt::WindowFlags f=0 );
    ~FileOpener();

    /** true if the file dialog was aborted (don't execute the dialog) */
    bool dialogAborted;

private:
    Ui::OptionsDialog ui;

    Config *config;

    QFileDialog *fileDialog;
    QList<QUrl> urls;

    QLabel *formatHelp;

private slots:
    void fileDialogAccepted();
    void okClickedSlot();
    void showHelp();

signals:
    void open( const QList<QUrl>& files, ConversionOptions *conversionOptions );
};

#endif
