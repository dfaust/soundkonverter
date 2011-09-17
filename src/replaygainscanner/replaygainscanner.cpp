
#include "replaygainscanner.h"

#include "replaygainfilelist.h"
#include "config.h"
#include "logger.h"
#include "combobutton.h"
#include "opener/diropener.h"
#include "codecproblems.h"

#include <qlayout.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kfiledialog.h>
#include <kpushbutton.h>

#include <QProgressBar>
#include <KIcon>
#include <QTreeWidget>
#include <QMessageBox>

// FIXME file name encoding !!!


ReplayGainScanner::ReplayGainScanner( Config* _config, Logger* _logger, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f )
{
    setButtons( 0 );

    config = _config;
    logger = _logger;

    setCaption( i18n("Replay Gain tool") );
    resize( 600, 400 );
    setWindowIcon( KIcon("soundkonverter-replaygain") );

    QWidget *widget = new QWidget( this );
    setMainWidget( widget );

    QGridLayout* grid = new QGridLayout( widget );

    QHBoxLayout* filterBox = new QHBoxLayout();
    grid->addLayout( filterBox, 0, 0 );

    cAdd = new ComboButton( widget );
    cAdd->insertItem( KIcon("folder"), i18n("Add folder...") );
    cAdd->insertItem( KIcon("audio-x-generic"), i18n("Add files...") );
    filterBox->addWidget( cAdd );
    connect( cAdd, SIGNAL(clicked(int)), this, SLOT(addClicked(int)) );

    filterBox->addStretch();

    cForce = new QCheckBox( i18n("Force recalculation"), this );
//     QToolTip::add( cForce, i18n("Recalculate Replay Gain tag for files that already have a Replay Gain tag set.") );
    filterBox->addWidget( cForce );

    lList = new ReplayGainFileList( config, logger, widget );
    grid->addWidget( lList, 1, 0 );
//     connect( this, SIGNAL(addFile(const QString&)), lList, SLOT(addFile(const QString&)) );
//     connect( this, SIGNAL(addDir(const QString&,const QStringList&,bool)), lList, SLOT(addDir(const QString&,const QStringList&,bool)) );
//     connect( this, SIGNAL(calcAllReplayGain(bool)), lList, SLOT(calcAllReplayGain(bool)) );
//     connect( this, SIGNAL(removeAllReplayGain()), lList, SLOT(removeAllReplayGain()) );
//     connect( this, SIGNAL(cancelProcess()), lList, SLOT(cancelProcess()) );
    connect( lList, SIGNAL(processStarted()), this, SLOT(processStarted()) );
    connect( lList, SIGNAL(processStopped()), this, SLOT(processStopped()) );
    connect( lList, SIGNAL(updateProgress(int,int)), this, SLOT(updateProgress(int,int)) );

    QHBoxLayout* progressBox = new QHBoxLayout();
    grid->addLayout( progressBox, 2, 0 );

    pProgressBar = new QProgressBar( widget );
    progressBox->addWidget( pProgressBar );

    QHBoxLayout* buttonBox = new QHBoxLayout();
    grid->addLayout( buttonBox, 3, 0 );

    pTagVisible = new KPushButton( KIcon("list-add"), i18n("Tag untagged"), widget );
//     QToolTip::add( pTagVisible, i18n("Calculate Replay Gain tag for all files in the file list without Replay Gain tag.") );
    buttonBox->addWidget( pTagVisible );
    connect( pTagVisible, SIGNAL(clicked()), this, SLOT(calcReplayGainClicked()) );

    pRemoveTag = new KPushButton( KIcon("list-remove"), i18n("Untag tagged"), widget );
//     QToolTip::add( pRemoveTag, i18n("Remove the Replay Gain tag from all files in the file list.") );
    buttonBox->addWidget( pRemoveTag );
    connect( pRemoveTag, SIGNAL(clicked()), this, SLOT(removeReplayGainClicked()) );

    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), widget );
    pCancel->hide();
    buttonBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(cancelClicked()) );

    buttonBox->addStretch();

    pClose = new KPushButton( KIcon("dialog-close"), i18n("Close"), widget );
    pClose->setFocus();
    buttonBox->addWidget( pClose );
    connect( pClose, SIGNAL(clicked()), this, SLOT(accept()) );
}

ReplayGainScanner::~ReplayGainScanner()
{}

void ReplayGainScanner::addClicked( int index )
{
    if( index == 1 )
    {
        showFileDialog();
    }
    else
    {
        showDirDialog();
    }
}

void ReplayGainScanner::showFileDialog()
{
    QStringList filterList;
    QStringList allFilter;
    QStringList formats = config->pluginLoader()->formatList( PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    for( int i=0; i<formats.count(); i++ )
    {
        QString extensionFilter = config->pluginLoader()->codecExtensions(formats.at(i)).join(" *.");
        if( extensionFilter.length() == 0 ) continue;
        extensionFilter = "*." + extensionFilter;
        allFilter += extensionFilter;
        filterList += extensionFilter + "|" + i18n("%1 files",formats.at(i));
    }
    filterList.prepend( allFilter.join(" ") + "|" + i18n("All supported files") );
    filterList += "*.*|" + i18n("All files");

    // add the control elements
    QLabel *formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("Are you missing some file formats?") + "</a>", this );
    connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()) );

    fileDialog = new KFileDialog( KUrl(QDir::homePath()), filterList.join("\n"), this, formatHelp );
    fileDialog->setWindowTitle( i18n("Add Files") );
    fileDialog->setMode( KFile::Files | KFile::ExistingOnly );
    connect( fileDialog, SIGNAL(accepted()), this, SLOT(fileDialogAccepted()) );
    fileDialog->show();

//     lList->addFiles( KFileDialog::getOpenUrls( KUrl(QDir::homePath()), filterList.join("\n"), this, i18n("Open files") ) );
}

void ReplayGainScanner::fileDialogAccepted()
{
    lList->addFiles( fileDialog->selectedUrls() );
}

void ReplayGainScanner::showHelp()
{
    QList<CodecProblems::Problem> problemList;
    QMap<QString,QStringList> problems = config->pluginLoader()->replaygainProblems();
    for( int i=0; i<problems.count(); i++ )
    {
        CodecProblems::Problem problem;
        problem.codecName = problems.keys().at(i);
        if( problem.codecName != "wav" )
        {
            problem.solutions = problems.value(problem.codecName);
            problemList += problem;
        }
    }

    CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Debug, problemList, this );
    problemsDialog->exec();
}

void ReplayGainScanner::showDirDialog()
{
    DirOpener *dialog = new DirOpener( config, DirOpener::ReplayGain, this );

    connect( dialog, SIGNAL(open(const KUrl&,bool,const QStringList&)), lList, SLOT(addDir(const KUrl&,bool,const QStringList&)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(open(const KUrl&,bool,const QStringList&)), 0, 0 );

    delete dialog;
}

void ReplayGainScanner::addFiles( KUrl::List urls )
{
    lList->addFiles( urls );
}

void ReplayGainScanner::calcReplayGainClicked()
{
//     emit calcAllReplayGain( cForce->isChecked() );
    lList->calcAllReplayGain( cForce->isChecked() );
}

void ReplayGainScanner::removeReplayGainClicked()
{
//     emit removeAllReplayGain();
    lList->removeAllReplayGain();
}

void ReplayGainScanner::cancelClicked()
{
//     emit cancelProcess();
    lList->cancelProcess();
}

void ReplayGainScanner::processStarted()
{
    pTagVisible->hide();
    pRemoveTag->hide();
    pCancel->show();
}

void ReplayGainScanner::processStopped()
{
    pTagVisible->show();
    pRemoveTag->show();
    pCancel->hide();
    pProgressBar->setMaximum( 100 );
    pProgressBar->setValue( 100 );
    setCaption( i18n("Finished") + " - " + i18n("Replay Gain tool") );
}

void ReplayGainScanner::updateProgress( int progress, int totalSteps )
{
    pProgressBar->setMaximum( totalSteps );
    pProgressBar->setValue( progress );
    const float fPercent = totalSteps > 0 ? progress * 100 / totalSteps : 0;

    QString percent;
    percent.sprintf( "%i%%", (int)fPercent );
    setCaption( percent + " - " + i18n("Replay Gain tool") );
}

