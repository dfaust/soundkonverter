
#include "playlistopener.h"

#include "../options.h"
#include "../config.h"
#include "../codecproblems.h"

#include <KLocalizedString>
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>
#include <QTextStream>

PlaylistOpener::PlaylistOpener(Config *_config, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    dialogAborted(false),
    config(_config)
{
    ui.setupUi(this);

    setWindowTitle(i18n("Add playlist"));
    setWindowIcon(QIcon::fromTheme("view-media-playlist"));

    ui.options->init(config, i18n("Select your desired output options and click on \"Ok\"."));

    connect(ui.okButton, SIGNAL(clicked()),     this, SLOT(okClickedSlot()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    fileDialog = new QFileDialog( this, i18n("Add playlist"), "kfiledialog:///soundkonverter-add-media", "*.m3u" );
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileDialogAccepted()));
    connect(fileDialog, SIGNAL(rejected()), this, SLOT(reject()));
    const int dialogReturnCode = fileDialog->exec();
    if( dialogReturnCode == QDialog::Rejected )
        dialogAborted = true;

//         // Prevent the dialog from beeing too wide because of the directory history
//     if( parent && width() > parent->width() )
//         setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "PlaylistOpener" );
//     restoreDialogSize( group );
}

PlaylistOpener::~PlaylistOpener()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "PlaylistOpener" );
//     saveDialogSize( group );
}

void PlaylistOpener::fileDialogAccepted()
{
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
//     QStringList messageList;
    QString fileName;
    QStringList filesNotFound;

    urls.clear();
    QUrl playlistUrl = QUrl(fileDialog->selectedFiles().first());
    QFile playlistFile( playlistUrl.toLocalFile() );
    if( playlistFile.open(QIODevice::ReadOnly) )
    {
        QTextStream stream(&playlistFile);
        QString line;

        do
        {
            line = stream.readLine();
            if( !line.startsWith("#EXTM3U") && !line.startsWith("#EXTINF") && !line.isEmpty() )
            {
                QUrl url(line);
                if( url.isRelative() )
                    url = QUrl( playlistUrl.path() + "/" + line );

                if( !url.isLocalFile() || QFile::exists(url.toLocalFile()) )
                    urls += url;
                else
                    filesNotFound += url.url(QUrl::PreferLocalFile);
            }
        } while( !line.isNull() );

        playlistFile.close();
    }

    const bool canDecodeAac = config->pluginLoader()->canDecode("m4a/aac");
    const bool canDecodeAlac = config->pluginLoader()->canDecode("m4a/alac");
    const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;

    for( int i=0; i<urls.count(); i++ )
    {
        QString mimeType;
        QString codecName = config->pluginLoader()->getCodecFromFile( urls.at(i), &mimeType, checkM4a );

        if( !config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            fileName = urls.at(i).url(QUrl::PreferLocalFile);

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
            urls.removeAt(i);
            i--;
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
                problem.affectedFiles += i18n("... and %1 more files",problems.value(problem.codecName).at(0).count()-2);
            }
            problemList += problem;
//             messageList += "<b>Possible solutions for " + codecName + "</b>:\n" + problems.value(codecName).at(1).join("\n<b>or</b>\n") + i18n("\n\nAffected files:\n") + affectedFiles.join("\n");
        }
    }

    if( problemList.count() > 0 )
    {
        CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Decode, problemList, this );
        problemsDialog->exec();
    }

//     if( !messageList.isEmpty() )
//     {
//         messageList.prepend( i18n("Some files can't be decoded.\nPossible solutions are listed below.") );
//         QMessageBox *messageBox = new QMessageBox( this );
//         messageBox->setIcon( QMessageBox::Information );
//         messageBox->setWindowTitle( i18n("Missing backends") );
//         messageBox->setText( messageList.join("\n\n").replace("\n","<br>") );
//         messageBox->setTextFormat( Qt::RichText );
//         messageBox->exec();
//     }

    if( !filesNotFound.isEmpty() )
    {
        int filesNotFoundCount = filesNotFound.count();
        if( filesNotFoundCount > 5 )
        {
            do {
                filesNotFound.removeLast();
            } while( filesNotFound.count() >= 5 );
            filesNotFound += i18n("... and %1 more files",filesNotFoundCount-4);
        }
        filesNotFound.prepend( i18n("The following files couldn't be found:\n") );
        QMessageBox *messageBox = new QMessageBox( this );
        messageBox->setIcon( QMessageBox::Information );
        messageBox->setWindowTitle( i18n("Files not found") );
        messageBox->setText( filesNotFound.join("\n").replace("\n","<br>") );
        messageBox->setTextFormat( Qt::RichText );
        messageBox->exec();
    }

    if( urls.count() <= 0 ) reject();
}

void PlaylistOpener::okClickedSlot()
{
    ConversionOptions *conversionOptions = ui.options->currentConversionOptions();
    if( conversionOptions )
    {
        ui.options->accepted();
        emit open(urls, conversionOptions);
        accept();
    }
    else
    {
        QMessageBox::critical(this, "soundKonverter", i18n("No conversion options selected."));
    }
}
