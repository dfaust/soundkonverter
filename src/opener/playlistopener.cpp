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
#include "playlistopener.h"
#include "../options.h"
#include "../config.h"
#include "../codecproblems.h"

#include <KLocale>
#include <KPushButton>
#include <QLabel>
#include <QLayout>
#include <KMessageBox>
#include <KFileDialog>
#include <QDir>


PlaylistOpener::PlaylistOpener( Config *_config, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    config( _config )
{
    setCaption( i18n("Add playlist") );
    setWindowIcon( KIcon("view-media-playlist") );
    setButtons( 0 );
    
    QWidget *widget = new QWidget();
    setMainWidget( widget );

    QGridLayout *mainGrid = new QGridLayout( widget );
    
    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    mainGrid->addWidget( options, 1, 0 );
    
    // add a horizontal box layout for the control elements
    QHBoxLayout *controlBox = new QHBoxLayout( 0 );
    mainGrid->addLayout( controlBox, 2, 0 );
    controlBox->addStretch();

    pAdd = new KPushButton( KIcon("dialog-ok"), i18n("Ok"), widget );
    controlBox->addWidget( pAdd );
    connect( pAdd, SIGNAL(clicked()), this, SLOT(okClickedSlot()) );
    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), widget );
    controlBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()) );
    
    fileDialog = new KFileDialog( KUrl(QDir::homePath()), "*.m3u", this );
    fileDialog->setWindowTitle( i18n("Add Files") );
    fileDialog->setMode( KFile::File | KFile::ExistingOnly );
    connect( fileDialog, SIGNAL(accepted()), this, SLOT(fileDialogAccepted()) );
    connect( fileDialog, SIGNAL(rejected()), this, SLOT(reject()) );
    fileDialog->show();
}

PlaylistOpener::~PlaylistOpener()
{}

void PlaylistOpener::fileDialogAccepted()
{
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
//     QStringList messageList;
    QString fileName;
    QStringList filesNotFound;
  
    urls.clear();
    KUrl playlistUrl = fileDialog->selectedUrl();
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
                KUrl url(line);
                if( url.isRelative() ) url = KUrl( playlistUrl.directory() + "/" + line );
                url.cleanPath();

                if( !url.isLocalFile() || QFile::exists(url.toLocalFile()) ) urls += url;
                else filesNotFound += url.pathOrUrl();
            }
        } while( !line.isNull() );
        playlistFile.close();
    }

    for( int i=0; i<urls.count(); i++ )
    {
        QString codecName = config->pluginLoader()->getCodecFromFile( urls.at(i) );

        if( !config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            fileName = urls.at(i).pathOrUrl();
            if( codecName.isEmpty() ) codecName = fileName.right(fileName.length()-fileName.lastIndexOf(".")-1);
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
    if( options->currentConversionOptions() )
    {
        emit done( urls, options->currentConversionOptions() );
        accept();
    }
    else
    {
        KMessageBox::error( this, i18n("No conversion options selected.") );
    }
}
