
#include "urlopener.h"
#include "ui_urlopener.h"
#include "../options.h"
#include "../config.h"

#include <KLocalizedString>
#include <QMessageBox>

// TODO enable proceed button only if at least one file got selected // copy'n'paste error ???

// TODO message box if url can't be added -> maybe in file list

UrlOpener::UrlOpener(Config *_config, QWidget *parent, Qt::WindowFlags f) :
    QDialog( parent, f ),
    ui(new Ui::UrlOpener),
    config( _config )
{
    ui->setupUi(this);

    QFont font;
    font.setBold(true);
    ui->step1Label->setFont(font);

    ui->okButton->hide();

    connect(ui->proceedButton, SIGNAL(clicked()), this, SLOT(proceedClicked()));
    connect(ui->okButton, SIGNAL(clicked()),      this, SLOT(okClickedSlot()));
    connect(ui->cancelButton, SIGNAL(clicked()),  this, SLOT(reject()));

    ui->options->init(config, i18n("Select your desired output options and click on \"Ok\"."));

//         // Prevent the dialog from beeing too wide because of the directory history
//     if( parent && width() > parent->width() )
//         setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "UrlOpener" );
//     restoreDialogSize( group );
}

UrlOpener::~UrlOpener()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "UrlOpener" );
//     saveDialogSize( group );
}

void UrlOpener::proceedClicked()
{
    if( ui->stackedWidget->currentIndex() == 0 )
    {
        if( !ui->urlRequester->url().isValid() )
        {
            QMessageBox::information(this, "soundKonverter", i18n("The Url you entered is invalid. Please try again."));
            return;
        }

        urls += ui->urlRequester->url();

        ui->stackedWidget->setCurrentIndex(1);

        QFont font;
        ui->step1Label->setFont(font);
        font.setBold(true);
        ui->step2Label->setFont(font);

        ui->proceedButton->hide();
        ui->okButton->show();
    }
}

void UrlOpener::okClickedSlot()
{
    ConversionOptions *conversionOptions = ui->options->currentConversionOptions();
    if( conversionOptions )
    {
        ui->options->accepted();
        emit open(urls, conversionOptions);
        accept();
    }
    else
    {
        QMessageBox::critical(this, "soundKonverter", i18n("No conversion ui->options selected."));
    }
}
