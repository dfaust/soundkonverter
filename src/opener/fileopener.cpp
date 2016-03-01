//
// C++ Implementation: opener
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fileopener.h"
#include "../options.h"
#include "../config.h"
#include "../codecproblems.h"

#include <QApplication>
#include <KLocale>
#include <KPushButton>
#include <QLabel>
#include <QLayout>
#include <QBoxLayout>
#include <KMessageBox>
#include <KFileDialog>
#include <QDir>


FileOpener::FileOpener( Config *_config, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    dialogAborted( false ),
    config( _config )
{
    setCaption( i18n("Add Files") );
    setWindowIcon( KIcon("audio-x-generic") );
    setButtons( 0 );

    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QWidget *widget = new QWidget();
    setMainWidget( widget );

    QGridLayout *mainGrid = new QGridLayout( widget );

    QStringList filterList;
    QStringList allFilter;
    const QStringList formats = config->pluginLoader()->formatList( PluginLoader::Decode, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    foreach( QString format, formats )
    {
        QString extensionFilter = config->pluginLoader()->codecExtensions(format).join(" *.");
        if( extensionFilter.length() == 0 )
            continue;
        extensionFilter = "*." + extensionFilter;
        allFilter += extensionFilter;
        filterList += extensionFilter + "|" + i18n("%1 files",format.replace("/","\\/"));
    }
    filterList.prepend( allFilter.join(" ") + "|" + i18n("All supported files") );
    filterList += "*.*|" + i18n("All files");

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    mainGrid->addWidget( options, 1, 0 );

    // add a horizontal box layout for the control elements
    QHBoxLayout *controlBox = new QHBoxLayout();
    mainGrid->addLayout( controlBox, 2, 0 );
    controlBox->addStretch();

    pAdd = new KPushButton( KIcon("dialog-ok"), i18n("Ok"), widget );
    controlBox->addWidget( pAdd );
    connect( pAdd, SIGNAL(clicked()), this, SLOT(okClickedSlot()) );
    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), widget );
    controlBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()) );

    // add the control elements
    formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("Are you missing some file formats?") + "</a>", widget );
    connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()) );

    fileDialog = new KFileDialog( KUrl("kfiledialog:///soundkonverter-add-media"), filterList.join("\n"), this, formatHelp );
    fileDialog->setWindowTitle( i18n("Add Files") );
    fileDialog->setMode( KFile::Files | KFile::ExistingOnly );
    connect( fileDialog, SIGNAL(accepted()), this, SLOT(fileDialogAccepted()) );
    connect( fileDialog, SIGNAL(rejected()), this, SLOT(reject()) );
    const int dialogReturnCode = fileDialog->exec();
    if( dialogReturnCode == QDialog::Rejected )
        dialogAborted = true;

    // Prevent the dialog from beeing too wide because of the directory history
    if( parent && width() > parent->width() )
        setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "FileOpener" );
    restoreDialogSize( group );
}

FileOpener::~FileOpener()
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "FileOpener" );
    saveDialogSize( group );
}

void FileOpener::fileDialogAccepted()
{
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    urls.clear();
    urls = fileDialog->selectedUrls();

    const bool canDecodeAac = config->pluginLoader()->canDecode( "m4a/aac" );
    const bool canDecodeAlac = config->pluginLoader()->canDecode( "m4a/alac" );
    const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;

    for( int i=0; i<urls.count(); i++ )
    {
        QString mimeType;
        QString codecName = config->pluginLoader()->getCodecFromFile( urls.at(i), &mimeType, checkM4a );

        if( !config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            fileName = urls.at(i).pathOrUrl();

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
            #if QT_VERSION >= 0x040500
                problems[problem.codecName][1].removeDuplicates();
            #else
                QStringList found;
                for( int j=0; j<problems.value(problem.codecName).at(1).count(); j++ )
                {
                    if( found.contains(problems.value(problem.codecName).at(1).at(j)) )
                    {
                        problems[problem.codecName][1].removeAt(j);
                        j--;
                    }
                    else
                    {
                        found += problems.value(problem.codecName).at(1).at(j);
                    }
                }
            #endif
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
        }
    }

    if( problemList.count() > 0 )
    {
        CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Decode, problemList, this );
        problemsDialog->exec();
    }

    if( urls.count() <= 0 )
        reject();
}

void FileOpener::okClickedSlot()
{
    ConversionOptions *conversionOptions = options->currentConversionOptions();
    if( conversionOptions )
    {
        options->accepted();
        emit open( urls, conversionOptions );
        accept();
    }
    else
    {
        KMessageBox::error( this, i18n("No conversion options selected.") );
    }
}

void FileOpener::showHelp()
{
    QList<CodecProblems::Problem> problemList;
    QMap<QString,QStringList> problems = config->pluginLoader()->decodeProblems();
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
