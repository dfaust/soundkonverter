
#ifndef URLOPENER_H
#define URLOPENER_H

#include <QDialog>
#include <QUrl>

class Config;
class Options;
class ConversionOptions;

namespace Ui {
    class UrlOpener;
}

class UrlOpener : public QDialog
{
    Q_OBJECT

public:
    UrlOpener(Config *_config, QWidget *parent=0, Qt::WindowFlags f=0);
    ~UrlOpener();

private:
    Ui::UrlOpener *ui;

    Config *config;

    QList<QUrl> urls;

private slots:
    void proceedClicked();
    void okClickedSlot();

signals:
    void open(const QList<QUrl>& files, ConversionOptions *conversionOptions);
};

#endif
