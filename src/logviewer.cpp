
#include "logviewer.h"
#include "logger.h"

#include <QLayout>
#include <QLabel>
#include <QApplication>

#include <KLocale>
#include <KIcon>
#include <KPushButton>
#include <KComboBox>
#include <KTextEdit>
#include <KFileDialog>
#include <KMessageBox>


LogViewer::LogViewer( Logger* _logger, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    logger( _logger )
{
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

    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );

    QHBoxLayout *topBox = new QHBoxLayout( widget );
    box->addLayout( topBox );
    QLabel *lItem = new QLabel( i18n("Log file:") );
    topBox->addWidget( lItem );
    topBox->setStretchFactor( lItem, 0 );
    cItem = new KComboBox( this );
    topBox->addWidget( cItem );
    topBox->setStretchFactor( cItem, 1 );
    connect( cItem, SIGNAL(activated(int)), this, SLOT(itemChanged()) );

    kLog = new KTextEdit( this );
    kLog->setTabStopWidth( kLog->tabStopWidth()/2 );
    box->addWidget( kLog );
    kLog->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard );

    refillLogs();

    setInitialSize( QSize(600,390) );
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "LogViewer" );
    restoreDialogSize( group );
}

LogViewer::~LogViewer()
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "LogViewer" );
    saveDialogSize( group );
}

void LogViewer::refillLogs()
{
    const int currentProcess = cItem->itemData(cItem->currentIndex()).toInt();

    cItem->clear();

    foreach( LoggerItem *log, logger->getLogs() )
    {
        QString name = log->identifier;
        // TODO make the string width dependend on the window width
        if( name.length() > 73 )
            name = name.left(35) + "..." + name.right(35);

        if( log->id == 1000 )
            cItem->addItem( i18n("soundKonverter application log"), QVariant(log->id) );
        else
            cItem->addItem( name, QVariant(log->id) );
    }

    if( cItem->findData(currentProcess) != -1 )
        cItem->setCurrentIndex( cItem->findData(currentProcess) );
    else
        cItem->setCurrentIndex( 0 );

    itemChanged();
}

void LogViewer::itemChanged()
{
    // HACK avoid Qt bug? changing the color of 'uncolored' text when switching the log file
    QTextCursor cursor = kLog->textCursor();
    cursor.setPosition( 0 );
    kLog->setTextCursor( cursor );

    kLog->clear();
    LoggerItem* item = logger->getLog( cItem->itemData(cItem->currentIndex()).toInt() );

    if( !item )
        return;

    foreach( const QString line, item->data )
        kLog->append( line );

    QPalette currentPalette = kLog->palette();
    if( item->completed )
    {
        currentPalette.setColor( QPalette::Base, QApplication::palette().base().color() );
    }
    else
    {
        currentPalette.setColor( QPalette::Base, QColor(255,234,234) );
    }
    kLog->setPalette( currentPalette );
}

void LogViewer::save()
{
    const QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.txt\n*.log", this, i18n("Save log file") );
    if( fileName.isEmpty() )
        return;

    QFile file( fileName );
    if( file.exists() )
    {
        if( KMessageBox::questionYesNo(this,i18n("File already exists. Do you really want to overwrite it?")) == KMessageBox::No )
            return;
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
    Q_UNUSED(id)

    refillLogs();
}

void LogViewer::updateProcess( int id )
{
    Q_UNUSED(id)

    refillLogs();
}

void LogViewer::showLog( int id )
{
    if( cItem->findData(QVariant(id)) != -1 )
        cItem->setCurrentIndex( cItem->findData(QVariant(id)) );
    else
        cItem->setCurrentIndex( 0 );

    itemChanged();
}

