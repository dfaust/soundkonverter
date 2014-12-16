
#include "replaygainscanner.h"
#include "ui_replaygainscanner.h"

#include "replaygainfilelist.h"
#include "replaygainprocessor.h"
#include "config.h"
#include "logger.h"
#include "combobutton.h"
#include "opener/diropener.h"
#include "codecproblems.h"
#include "progressindicator.h"

#include <KLocalizedString>

#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QUrl>
#include <QtCore> // KDevelop foreach syntax highlighting fix

ReplayGainScanner::ReplayGainScanner(Config* _config, Logger* _logger, bool showMainWindowButton, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::ReplayGainScanner),
    config( _config ),
    logger( _logger )
{
//     const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    ui->setupUi(this);

    connect(ui->closeButton, SIGNAL(clicked()),          this, SLOT(closeClicked()));
    connect(ui->addComboButton, SIGNAL(clicked(int)),    this, SLOT(addClicked(int)));
    connect(ui->showMainWindowButton, SIGNAL(clicked()), this, SLOT(showMainWindowClicked()));
    connect(ui->tagButton, SIGNAL(clicked()),            this, SLOT(calcReplayGainClicked()));
    connect(ui->untagButton, SIGNAL(clicked()),          this, SLOT(removeReplayGainClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()),         this, SLOT(cancelClicked()));

    connect(ui->fileList, SIGNAL(processStarted()),      this, SLOT(processStarted()));
    connect(ui->fileList, SIGNAL(processStopped()),      this, SLOT(processStopped()));

    connect(ui->fileList, SIGNAL(timeChanged(float)),    ui->progressIndicator, SLOT(timeChanged(float)));
    connect(ui->fileList, SIGNAL(finished(bool)),        ui->progressIndicator, SLOT(finished(bool)));

    connect(ui->progressIndicator, SIGNAL(progressChanged(const QString&)), this, SLOT(progressChanged(const QString&)));

    ui->fileList->setConfig(config);

    ui->addComboButton->insertItem(QIcon::fromTheme("folder"),          i18n("Add folder..."));
    ui->addComboButton->insertItem(QIcon::fromTheme("audio-x-generic"), i18n("Add files..."));

    ui->showMainWindowButton->setVisible(showMainWindowButton);

    ui->cancelButton->hide();

    ReplayGainProcessor *replayGainProcessor = new ReplayGainProcessor(config, ui->fileList, logger);
    connect(ui->fileList, SIGNAL(processItem(ReplayGainFileListItem*,ReplayGainPlugin::ApplyMode)),            replayGainProcessor, SLOT(add(ReplayGainFileListItem*,ReplayGainPlugin::ApplyMode)));
    connect(ui->fileList, SIGNAL(killItem(ReplayGainFileListItem*)),                                           replayGainProcessor, SLOT(kill(ReplayGainFileListItem*)));
    connect(replayGainProcessor, SIGNAL(finished(ReplayGainFileListItem*,ReplayGainFileListItem::ReturnCode)), ui->fileList, SLOT(itemFinished(ReplayGainFileListItem*,ReplayGainFileListItem::ReturnCode)));
    connect(replayGainProcessor, SIGNAL(updateItem(ReplayGainFileListItem*,bool)),                             ui->fileList, SLOT(updateItem(ReplayGainFileListItem*,bool)));

    connect(replayGainProcessor, SIGNAL(finishedProcess(int,bool)), logger, SLOT(processCompleted(int,bool)));

    connect(replayGainProcessor, SIGNAL(updateTime(float)),   ui->progressIndicator, SLOT(update(float)));
    connect(replayGainProcessor, SIGNAL(timeFinished(float)), ui->progressIndicator, SLOT(timeFinished(float)));

//     setInitialSize( QSize(60*fontHeight,40*fontHeight) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "ReplayGainTool" );
//     restoreDialogSize( group );
}

ReplayGainScanner::~ReplayGainScanner()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "ReplayGainTool" );
//     saveDialogSize( group );
}

void ReplayGainScanner::addClicked(int index)
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
    const QStringList formats = config->pluginLoader()->formatList(PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid));
    foreach( QString format, formats )
    {
        QString extensionFilter = config->pluginLoader()->codecExtensions(format).join(" *.");
        if( extensionFilter.isEmpty() )
            continue;

        extensionFilter = "*." + extensionFilter;
        allFilter += extensionFilter;
        filterList += extensionFilter + "|" + i18n("%1 files", format.replace("/","\\/"));
    }
    filterList.prepend(allFilter.join(" ") + "|" + i18n("All supported files"));
    filterList += "*.*|" + i18n("All files");

    // add the control elements
    QLabel *formatHelp = new QLabel("<a href=\"format-help\">" + i18n("Are you missing some file formats?") + "</a>", this);
    connect(formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()));

    fileDialog = new QFileDialog(this, i18n("Add Files"), "kfiledialog:///soundkonverter-add-media", filterList.join("\n"));
    fileDialog->setWindowTitle(i18n("Add Files"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileDialogAccepted()));
    fileDialog->exec();
}

void ReplayGainScanner::fileDialogAccepted()
{
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    QList<QUrl> urls = fileDialog->selectedUrls();

    const bool canDecodeAac = config->pluginLoader()->canDecode( "m4a/aac" );
    const bool canDecodeAlac = config->pluginLoader()->canDecode( "m4a/alac" );
    const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;

    foreach( const QUrl& url, urls )
    {
        QString mimeType;
        QString codecName = config->pluginLoader()->getCodecFromFile( url, &mimeType, checkM4a );

        if( !config->pluginLoader()->canReplayGain(codecName,0,&errorList) )
        {
            fileName = url.url(QUrl::PreferLocalFile);

            if( codecName.isEmpty() )
                codecName = mimeType;

            if( codecName.isEmpty() )
                codecName = fileName.right(fileName.length()-fileName.lastIndexOf(".")-1);

            if( problems.value(codecName).count() < 2 )
            {
                problems[codecName] += QStringList();
                problems[codecName] += QStringList();
            }
            problems[codecName][0] += fileName;
            if( !errorList.isEmpty() )
            {
                problems[codecName][1] += errorList;
            }
            else
            {
                problems[codecName][1] += i18n("This file type is unknown to soundKonverter.\nMaybe you need to install an additional soundKonverter plugin.\nYou should have a look at your distribution's package manager for this.");
            }

            urls.removeAll(url);
        }
    }

    QList<CodecProblems::Problem> problemList;
    for( int i=0; i<problems.count(); i++ )
    {
        CodecProblems::Problem problem;
        problem.codecName = problems.keys().at(i);
        if( problem.codecName != "wav" )
        {
            problems[problem.codecName][1].removeDuplicates();
            problem.solutions = problems.value(problem.codecName).at(1);
            if( problems.value(problem.codecName).at(0).count() <= 3 )
            {
                problem.affectedFiles = problems.value(problem.codecName).at(0);
            }
            else
            {
                problem.affectedFiles += problems.value(problem.codecName).at(0).at(0);
                problem.affectedFiles += problems.value(problem.codecName).at(0).at(1);
                problem.affectedFiles += i18n("... and %1 more files", problems.value(problem.codecName).at(0).count() - 2);
            }
            problemList += problem;
        }
    }

    if( problemList.count() > 0 )
    {
        CodecProblems *problemsDialog = new CodecProblems(CodecProblems::ReplayGain, problemList, this);
        problemsDialog->exec();
    }

    if( urls.count() > 0 )
        ui->fileList->addFiles( urls );
}

void ReplayGainScanner::showHelp()
{
    QList<CodecProblems::Problem> problemList;
    const QMap<QString, QStringList> problems = config->pluginLoader()->replaygainProblems();
    foreach( const QString& codecName, problems.keys() )
    {
        if( codecName != "wav" )
        {
            CodecProblems::Problem problem;
            problem.codecName = codecName;
            problem.solutions = problems.value(codecName);
            problemList += problem;
        }
    }

    CodecProblems *problemsDialog = new CodecProblems(CodecProblems::Debug, problemList, this);
    problemsDialog->exec();
}

void ReplayGainScanner::showDirDialog()
{
    DirOpener *dialog = new DirOpener(config, DirOpener::ReplayGain, this);

    if( !dialog->dialogAborted )
    {
        connect(dialog, SIGNAL(open(const QUrl&,bool,const QStringList&)), ui->fileList, SLOT(addDir(const QUrl&,bool,const QStringList&)));

        dialog->exec();

        disconnect(dialog, SIGNAL(open(const QUrl&,bool,const QStringList&)), 0, 0);
    }

    delete dialog;
}

void ReplayGainScanner::showMainWindowClicked()
{
    ui->showMainWindowButton->hide();

    emit showMainWindow();
}

void ReplayGainScanner::addFiles(QList<QUrl> urls)
{
    ui->fileList->addFiles(urls);
}

void ReplayGainScanner::calcReplayGainClicked()
{
    ui->fileList->startProcessing(ui->forceCheckBox->isChecked() ? ReplayGainPlugin::Force : ReplayGainPlugin::Add);
}

void ReplayGainScanner::removeReplayGainClicked()
{
    ui->fileList->startProcessing(ReplayGainPlugin::Remove);
}

void ReplayGainScanner::cancelClicked()
{
    ui->fileList->cancelProcess();
}

void ReplayGainScanner::closeClicked()
{
    if( ui->cancelButton->isVisible() )
    {
        const int ret = QMessageBox::question(this, "soundKonverter", i18n("There are still Replay Gain jobs running.\nDo you really want to cancel them?"));
        if( ret == QMessageBox::Yes )
        {
            ui->fileList->cancelProcess();
        }
        else
        {
            return;
        }
    }

    accept();
}

void ReplayGainScanner::processStarted()
{
    ui->tagButton->hide();
    ui->untagButton->hide();
    ui->cancelButton->show();
}

void ReplayGainScanner::processStopped()
{
    ui->tagButton->show();
    ui->untagButton->show();
    ui->cancelButton->hide();
}

void ReplayGainScanner::progressChanged(const QString& progress)
{
    setWindowTitle(progress + " - " + i18n("Replay Gain tool"));
}
