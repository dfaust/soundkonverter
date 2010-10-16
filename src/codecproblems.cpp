
#include "codecproblems.h"

#include <KLocale>
#include <KIcon>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>


CodecProblems::CodecProblems( Mode mode, const QList<Problem>& problemList, QWidget* parent, Qt::WFlags f )
    : KDialog( parent, f )
{
    setCaption( i18n("Solutions for backend problems") );
    setWindowIcon( KIcon("help-about") );
    setButtons( KDialog::Close );
    setButtonFocus( KDialog::Close );
    
    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );
    
    QString message;
    if( mode == Debug )
    {
        if( problemList.isEmpty() )
        {
            message = i18n("soundKonverter couldn't find any missing packages.\nMaybe you need to install an additional plugin via the package manager of your distribution.");
        }
        else
        {
            message = i18n("Some of the installed plugins aren't working because they are missing additional programs.\nPossible solutions are listed below.");
        }
    }
    else if( mode == Decode )
    {
        message = i18n("Some files can't be decoded.\nPossible solutions are listed below.");
    }
    else if( mode == ReplayGain )
    {
        message = i18n("Replay Gain isn't supported for some files.\nPossible solutions are listed below.");
    }
    else if( mode == AudioCd )
    {
        if( problemList.isEmpty() )
        {
            message = i18n("Ripping audio CDs is not supported by any installed plugin.\nPlease have a look at your distributions package manager in order to get a cd ripper plugin for soundKonverter.");
        }
        else
        {
            message = i18n("Ripping audio CDs is currently not supported because of missing backends.\nPossible solutions are listed below.");
        }
    }
    QLabel *messageLabel = new QLabel( message, this );
    box->addWidget( messageLabel );

    if( !problemList.isEmpty() )
    {
        QStringList messageList;
        for( int i=0; i<problemList.count(); i++ )
        {
            QString codecName = problemList.at(i).codecName;
            if( codecName != "wav" )
            {
                if( problemList.at(i).affectedFiles.isEmpty() )
                {
                    messageList += "<b>Possible solutions for " + codecName + "</b>:\n" + problemList.at(i).solutions.join("\n<b>or</b>\n");
                }
                else
                {
                    messageList += "<b>Possible solutions for " + codecName + "</b>:\n" + problemList.at(i).solutions.join("\n<b>or</b>\n") + "\n\n" + i18n("Affected files:") + "\n" + problemList.at(i).affectedFiles.join("\n");
                }
            }
        }
        QLabel *solutionsLabel = new QLabel( messageList.join("\n\n").replace("\n","<br>"), this );
        solutionsLabel->setMargin( 8 );
        solutionsLabel->setWordWrap( true );
        
        QScrollArea *solutionsScrollArea = new QScrollArea();
        solutionsScrollArea->setWidget( solutionsLabel );
        box->addWidget( solutionsScrollArea );
    }
}

CodecProblems::~CodecProblems()
{}

