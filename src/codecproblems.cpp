
#include "codecproblems.h"

#include <KLocalizedString>
#include <QtCore> // KDevelop foreach syntax highlighting fix

CodecProblems::CodecProblems(Mode mode, const QList<Problem>& problemList, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    ui.setupUi(this);

    connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QString message;

    switch( mode )
    {
        case Debug:
        {
            if( problemList.isEmpty() )
            {
                message = i18n("soundKonverter couldn't find any missing packages.\nIf you are missing some file formats you might need to install an additional plugin via the package manager of your distribution.");
            }
            else
            {
                message = i18n("Some of the installed plugins aren't working.\nPossible solutions are listed below.");
            }
            break;
        }
        case Decode:
        {
            message = i18n("Some files can't be decoded.\nPossible solutions are listed below.");
            break;
        }
        case ReplayGain:
        {
            message = i18n("Replay Gain isn't supported for some files.\nPossible solutions are listed below.");
            break;
        }
        case AudioCd:
        {
            if( problemList.isEmpty() )
            {
                message = i18n("Ripping audio CDs is not supported by any installed plugin.\nPlease have a look at your distributions package manager in order to get a cd ripper plugin for soundKonverter.");
            }
            else
            {
                message = i18n("Ripping audio CDs is currently not supported because of missing backends.\nPossible solutions are listed below.");
            }
            break;
        }
    }

    ui.messageLabel->setText(message);

    if( !problemList.isEmpty() )
    {
        QStringList messageList;

        foreach( const Problem& problem, problemList )
        {
            const QString codecName = problem.codecName;
            if( codecName != "wav" )
            {
                if( problem.affectedFiles.isEmpty() )
                {
                    messageList += "<b>" + i18n("Possible solutions for %1", codecName) + "</b>:\n" + problem.solutions.join("\n<b>" + i18nc("like in either or", "or") + "</b>\n");
                }
                else
                {
                    messageList += "<b>" + i18n("Possible solutions for %1", codecName) + "</b>:\n" + problem.solutions.join("\n<b>" + i18nc("like in either or", "or") + "</b>\n") + "\n\n" + i18n("Affected files:") + "\n" + problem.affectedFiles.join("\n");
                }
            }
        }

        ui.solutionsLabel->setText(messageList.join("\n\n").replace("\n","<br>"));
//         ui.solutionsLabel->setMargin( 8 );
    }
}

CodecProblems::~CodecProblems()
{
}
