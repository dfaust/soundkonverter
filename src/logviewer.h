

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>

class Logger;
class QComboBox;
class QTextEdit;


/**
 * @short Shows the logs that are collected by the logger
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class LogViewer : public QDialog
{
Q_OBJECT
public:
    /** Default Constructor */
    LogViewer( Logger* _logger, QWidget* parent=0, Qt::WindowFlags f=0 );

    /** Default Destructor */
    ~LogViewer();

private:
    Logger* logger;

    QComboBox *cItem;
    QTextEdit *kLog;

private slots:
    void refillLogs();
    void itemChanged();
    void save();
    void updateProcess( int id );

public slots:
    /** get notification when a job has been removed */
    void processRemoved( int id );
    /** show the corresponding log */
    void showLog( int id );
};

#endif // LOGVIEWER_H
