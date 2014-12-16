
#ifndef PLAYLISTOPENER_H
#define PLAYLISTOPENER_H

#include <QDialog>

#include <QUrl>

class Config;
class ConversionOptions;
class QFileDialog;

namespace Ui {
    class OptionsDialog;
}

class PlaylistOpener : public QDialog
{
    Q_OBJECT
public:
    PlaylistOpener(Config *_config, QWidget *parent=0, Qt::WindowFlags f=0);
    ~PlaylistOpener();

    /** true if the file dialog was aborted (don't execute the dialog) */
    bool dialogAborted;

private:
    Ui::OptionsDialog *ui;

    Config *config;

    QFileDialog *fileDialog;
    QList<QUrl> urls;

private slots:
    void fileDialogAccepted();
    void okClickedSlot();

signals:
    void open( const QList<QUrl>& files, ConversionOptions *conversionOptions );
};

#endif
