
#ifndef BACKENDSGROUPBOX_H
#define BACKENDSGROUPBOX_H

#include "ui_backendsgroupbox.h"

#include "core/backendplugin.h"

class Config;

class BackendsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    BackendsGroupBox(QWidget *parent=0);
    ~BackendsGroupBox();

    void init(BackendPlugin::ActionType actionType, Config *config);

    void setFormat(const QString& format);
    void addItem(const QString& item);
    void clear();
    QStringList getList();
    bool changed();
    void resetOrder();

private:
    Ui::BackendsGroupBox ui;

    BackendPlugin::ActionType actionType;
    Config *config;

    QString format;
    QStringList originalOrder;

private slots:
    void itemSelected(int item);
    void up();
    void down();
    void configure();
    void info();

signals:
    void orderChanged();
};

#endif
