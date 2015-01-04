
#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include "ui_logviewer.h"

#include <QDialog>

class Logger;

/** Shows the logs that are collected by the logger */
class LogViewer : public QDialog
{
    Q_OBJECT

public:
    LogViewer(Logger* _logger, QWidget* parent=0, Qt::WindowFlags f=0);
    ~LogViewer();

private:
    Ui::LogViewer ui;

    Logger* logger;

private slots:
    void refillLogs();
    void itemChanged();
    void save();
    void updateProcess(int id);

public slots:
    /** get notification when a job has been removed */
    void processRemoved(int id);
    /** show the corresponding log */
    void showLog(int id);
};

#endif // LOGVIEWER_H
