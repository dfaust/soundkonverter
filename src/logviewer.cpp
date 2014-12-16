
#include "logviewer.h"
#include "ui_logviewer.h"
#include "logger.h"

#include <KLocalizedString>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore> // KDevelop foreach syntax highlighting fix

LogViewer::LogViewer(Logger* _logger, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::LogViewer),
    logger(_logger)
{
//     const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    connect(logger, SIGNAL(removedProcess(int)), this, SLOT(processRemoved(int)));
    connect(logger, SIGNAL(updateProcess(int)),  this, SLOT(updateProcess(int)));

    ui->setupUi(this);

    connect(ui->closeButton, SIGNAL(clicked()),   this, SLOT(close()));
    connect(ui->saveButton, SIGNAL(clicked()),    this, SLOT(save()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refillLogs()));


    refillLogs();

//     setInitialSize( QSize(60*fontHeight,40*fontHeight) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "LogViewer" );
//     restoreDialogSize( group );
}

LogViewer::~LogViewer()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "LogViewer" );
//     saveDialogSize( group );
}

void LogViewer::refillLogs()
{
    const int currentProcess = ui->logSelect->itemData(ui->logSelect->currentIndex()).toInt();

    ui->logSelect->clear();

    foreach( LoggerItem *logItem, logger->getLogs() )
    {
        QString name = logItem->identifier;
        // TODO make the string width dependend on the window width
        if( name.length() > 73 )
            name = name.left(35) + "..." + name.right(35);

        if( logItem->id == 1000 )
            ui->logSelect->addItem(i18n("soundKonverter application log"), QVariant(logItem->id));
        else
            ui->logSelect->addItem(name, QVariant(logItem->id));
    }

    if( ui->logSelect->findData(currentProcess) != -1 )
        ui->logSelect->setCurrentIndex(ui->logSelect->findData(currentProcess));
    else
        ui->logSelect->setCurrentIndex(0);

    itemChanged();
}

void LogViewer::itemChanged()
{
    // HACK avoid Qt bug? changing the color of 'uncolored' text when switching the log file
    QTextCursor cursor = ui->logView->textCursor();
    cursor.setPosition(0);
    ui->logView->setTextCursor(cursor);

    ui->logView->clear();
    LoggerItem* item = logger->getLog(ui->logSelect->itemData(ui->logSelect->currentIndex()).toInt());

    if( !item )
        return;

    foreach( const QString line, item->data )
        ui->logView->append(line);

    QPalette currentPalette = ui->logView->palette();
    if( item->completed )
    {
        currentPalette.setColor(QPalette::Base, QApplication::palette().base().color());
    }
    else
    {
        currentPalette.setColor(QPalette::Base, QColor(255,234,234));
    }
    ui->logView->setPalette(currentPalette);
}

void LogViewer::save()
{
    const QString fileName = QFileDialog::getSaveFileName(this, i18n("Save log file"), "", "*.txt\n*.log");

    if( fileName.isEmpty() )
        return;

    QFile file(fileName);

    if( file.exists() )
    {
        if( QMessageBox::question(this, i18n("Overwrite file?"), i18n("File already exists. Do you really want to overwrite it?")) == QMessageBox::No )
            return;
    }

    if( !file.open(QIODevice::WriteOnly) )
    {
        QMessageBox::critical(this, "soundKonverter", i18n("Writing to file failed.\nMaybe you haven't got write permission."));
        return;
    }

    file.write(ui->logView->toPlainText().toUtf8().data());
    file.close();
}

void LogViewer::processRemoved(int id)
{
    Q_UNUSED(id)

    refillLogs();
}

void LogViewer::updateProcess(int id)
{
    Q_UNUSED(id)

    refillLogs();
}

void LogViewer::showLog(int id)
{
    if( ui->logSelect->findData(QVariant(id)) != -1 )
        ui->logSelect->setCurrentIndex(ui->logSelect->findData(QVariant(id)));
    else
        ui->logSelect->setCurrentIndex(0);

    itemChanged();
}
