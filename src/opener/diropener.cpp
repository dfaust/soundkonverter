
#include "diropener.h"
#include "ui_diropener.h"

#include "../config.h"
#include "../options.h"
#include "../codecproblems.h"

#include <QDir>
#include <KLocalizedString>
#include <QFileDialog>
#include <QMessageBox>


DirOpener::DirOpener(Config *_config, Mode _mode, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DirOpener),
    dialogAborted(false),
    config(_config),
    mode(_mode)
{
    ui->setupUi(this);

    if( mode == ReplayGain )
    {
        ui->step1Label->hide();
        ui->step2Label->hide();
        ui->line->hide();
    }

    QFont font;
    font.setBold(true);
    ui->step1Label->setFont(font);

    ui->okButton->hide();

    connect(ui->proceedButton, SIGNAL(clicked()), this, SLOT(proceedClicked()));
    connect(ui->okButton, SIGNAL(clicked()),      this, SLOT(okClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()),  this, SLOT(reject()));

    connect(ui->formatHelpLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()));

    connect(ui->selectAllButton, SIGNAL(clicked()),  this, SLOT(selectAllClicked()));
    connect(ui->selectNoneButton, SIGNAL(clicked()), this, SLOT(selectNoneClicked()));

    ui->options->init(config, i18n("Select your desired output options and click on \"Ok\"."));

    QStringList codecList;
    if( mode == Convert )
    {
        codecList = config->pluginLoader()->formatList(PluginLoader::Decode, PluginLoader::CompressionType(PluginLoader::InferiorQuality | PluginLoader::Lossy | PluginLoader::Lossless | PluginLoader::Hybrid));
    }
    else if( mode == ReplayGain )
    {
        codecList = config->pluginLoader()->formatList(PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::InferiorQuality | PluginLoader::Lossy | PluginLoader::Lossless | PluginLoader::Hybrid));
    }

    foreach( const QString& codecName, codecList )
    {
        if( codecName != "audio cd" )
        {
            QListWidgetItem *newItem = new QListWidgetItem(codecName, ui->formatListWidget);
            newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            newItem->setCheckState(Qt::Checked);
        }
    }

    const QUrl url = QUrl::fromLocalFile(QFileDialog::getExistingDirectory(this, i18n("Add directory"), ui->urlRequester->url().toLocalFile()));
    if( !url.isEmpty() )
        ui->urlRequester->setUrl(url);
    else
        dialogAborted = true;

//         // Prevent the dialog from beeing too wide because of the directory history
//     if( parent && width() > parent->width() )
//         setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "DirOpener" );
//     restoreDialogSize( group );
}

DirOpener::~DirOpener()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "DirOpener" );
//     saveDialogSize( group );
}

void DirOpener::proceedClicked()
{
    if( ui->stackedWidget->currentIndex() == 0 )
    {
        ui->stackedWidget->setCurrentIndex(1);

        QFont font;
        ui->step1Label->setFont(font);
        font.setBold(true);
        ui->step2Label->setFont(font);

        ui->proceedButton->hide();
        ui->okButton->show();
    }
}

void DirOpener::okClicked()
{
    QStringList selectedCodecs;
    for( int i = 0; i < ui->formatListWidget->count(); i++ )
    {
        if( ui->formatListWidget->item(i)->checkState() == Qt::Checked )
            selectedCodecs += ui->formatListWidget->item(i)->text();
    }

    if( mode == Convert )
    {
        ConversionOptions *conversionOptions = ui->options->currentConversionOptions();
        if( conversionOptions )
        {
            hide();
            ui->options->accepted();
            emit open(ui->urlRequester->url(), ui->recursiveCheckBox->checkState() == Qt::Checked, selectedCodecs, conversionOptions);
            accept();
        }
        else
        {
            QMessageBox::critical(this, "soundKonverter", i18n("No conversion options selected."));
        }
    }
    else if( mode == ReplayGain )
    {
        hide();
        emit open(ui->urlRequester->url(), ui->recursiveCheckBox->checkState() == Qt::Checked, selectedCodecs);
        accept();
    }
}

void DirOpener::selectAllClicked()
{
    for( int i = 0; i < ui->formatListWidget->count(); i++ )
    {
        ui->formatListWidget->item(i)->setCheckState( Qt::Checked );
    }
}

void DirOpener::selectNoneClicked()
{
    for( int i = 0; i < ui->formatListWidget->count(); i++ )
    {
        ui->formatListWidget->item(i)->setCheckState( Qt::Unchecked );
    }
}

void DirOpener::showHelp()
{
    QList<CodecProblems::Problem> problemList;

    QMap<QString,QStringList> problems = ( mode == Convert ) ? config->pluginLoader()->decodeProblems() : config->pluginLoader()->replaygainProblems();
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


