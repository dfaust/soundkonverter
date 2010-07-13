
#include "logviewer.h"
#include "logger.h"

#include <KLocale>
#include <KIcon>
#include <QLayout>
#include <KPushButton>
#include <KComboBox>
#include <KTextEdit>
#include <KFileDialog>
#include <KMessageBox>


LogViewer::LogViewer( Logger* _logger, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f )
{
    logger = _logger;
    connect( logger, SIGNAL(removedProcess(int)), this, SLOT(processRemoved(int)) );
    connect( logger, SIGNAL(updateProcess(int)), this, SLOT(updateProcess(int)) );

    setCaption( i18n("Log Viewer") );
    setWindowIcon( KIcon("view-list-text") );
    setButtons( KDialog::User1 | KDialog::User2 | KDialog::Close );
    setButtonText( KDialog::User1, i18n("Update") );
    setButtonIcon( KDialog::User1, KIcon("view-refresh") );
    connect( this, SIGNAL(user1Clicked()), this, SLOT(refillLogs()) );
    setButtonText( KDialog::User2, i18n("Save to file...") );
    setButtonIcon( KDialog::User2, KIcon("document-save") );
    connect( this, SIGNAL(user2Clicked()), this, SLOT(save()) );
    setButtonFocus( KDialog::Close );
    resize( 600, 390 );

    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );

    cItem = new KComboBox( this );
    box->addWidget( cItem );
    connect( cItem, SIGNAL(activated(int)), this, SLOT(itemChanged()) );

    kLog = new KTextEdit( this );
    kLog->setTabStopWidth( kLog->tabStopWidth()/2 );
    box->addWidget( kLog );
    kLog->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard );

    refillLogs();
}

LogViewer::~LogViewer()
{}

void LogViewer::refillLogs()
{
    QString name;
    QString currentProcess = cItem->currentText();
    cItem->clear();
    QList<LoggerItem*> logs = logger->getLogs();
    for( QList<LoggerItem*>::Iterator a = logs.begin(); a != logs.end(); ++a )
    {
        name = (*a)->filename.pathOrUrl();
        // TODO make the string width dependend on the window width
        if( name.length() > 73 ) name = name.left(35) + "..." + name.right(35);
        cItem->addItem( name + " - " + QString::number((*a)->id), QVariant((*a)->id) );
    }
    if( cItem->findText(currentProcess) != -1 ) cItem->setCurrentIndex( cItem->findText(currentProcess) );
    else cItem->setCurrentIndex( 0 );
    itemChanged();
}

void LogViewer::itemChanged()
{
    kLog->clear();
    LoggerItem* item = logger->getLog( cItem->itemData(cItem->currentIndex()).toInt() );

    for( QStringList::Iterator b = item->data.begin(); b != item->data.end(); ++b )
    {
        kLog->append( *b );
    }
    
    QPalette currentPalette = kLog->palette();
    if( item->completed )
    {
        currentPalette.setColor( QPalette::Base, QColor(255,255,255) );
    }
    else
    {
        currentPalette.setColor( QPalette::Base, QColor(255,234,234) );
    }
    kLog->setPalette( currentPalette );
}

void LogViewer::save()
{
    QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.txt\n*.log", this, i18n("Save log file") );
    if( fileName.isEmpty() ) return;
    QFile file( fileName );
    if( file.exists() )
    {
        if( KMessageBox::questionYesNo(this,i18n("File already exists. Do you really want to overwrite it?")) == KMessageBox::No ) return;
    }
    if( !file.open(QIODevice::WriteOnly) )
    {
        KMessageBox::error( this, i18n("Writing to file failed.\nMaybe you haven't got write permission.") );
        return;
    }
    QTextStream textStream;
    textStream.setDevice( &file );
    textStream << kLog->toPlainText();
    file.close();
}

void LogViewer::processRemoved( int id )
{
    refillLogs();
}

void LogViewer::updateProcess( int id )
{
    refillLogs();
}


