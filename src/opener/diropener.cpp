
#include "diropener.h"
#include "../config.h"
#include "../options.h"
#include "../codecproblems.h"

#include <QLayout>
#include <QLabel>
#include <QDir>
#include <QCheckBox>
#include <KLocale>
#include <KPushButton>
#include <KFileDialog>
#include <KIcon>
#include <KListWidget>
#include <KUrlRequester>
#include <KMessageBox>


DirOpener::DirOpener( Config *_config, Mode _mode, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    dialogAborted( false ),
    config( _config ),
    mode( _mode )
{
    setCaption( i18n("Add folder") );
    setWindowIcon( KIcon("folder") );

    if( mode == Convert )
    {
        setButtons( KDialog::User1 | KDialog::Cancel );
    }
    else if( mode == ReplayGain )
    {
        setButtons( KDialog::Ok | KDialog::Cancel );
    }

    setButtonText( KDialog::User1, i18n("Proceed") );
    setButtonIcon( KDialog::User1, KIcon("go-next") );

    connect( this, SIGNAL(user1Clicked()), this, SLOT(proceedClicked()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(addClicked()) );

    page = DirOpenPage;

    QWidget *widget = new QWidget();
    QGridLayout *mainGrid = new QGridLayout( widget );
    QGridLayout *topGrid = new QGridLayout();
    mainGrid->addLayout( topGrid, 0, 0 );
    setMainWidget( widget );

    lSelector = new QLabel( i18n("1. Select directory"), widget );
    QFont font;
    font.setBold( true );
    lSelector->setFont( font );
    topGrid->addWidget( lSelector, 0, 0 );
    lOptions = new QLabel( i18n("2. Set conversion options"), widget );
    topGrid->addWidget( lOptions, 0, 1 );

    // draw a horizontal line
    QFrame *lineFrame = new QFrame( widget );
    lineFrame->setFrameShape( QFrame::HLine );
    lineFrame->setFrameShadow( QFrame::Sunken );
    mainGrid->addWidget( lineFrame, 1, 0 );

    if( mode == ReplayGain )
    {
        lSelector->hide();
        lOptions->hide();
        lineFrame->hide();
    }

    // Dir Opener Widget

    dirOpenerWidget = new QWidget( widget );
    mainGrid->addWidget( dirOpenerWidget, 2, 0 );

    QVBoxLayout *box = new QVBoxLayout( dirOpenerWidget );

    QHBoxLayout *directoryBox = new QHBoxLayout();
    box->addLayout( directoryBox );

    QLabel *labelFilter = new QLabel( i18n("Directory:"), dirOpenerWidget );
    directoryBox->addWidget( labelFilter );

    uDirectory = new KUrlRequester( QDir::homePath(), dirOpenerWidget );
    uDirectory->setMode( KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly );
    directoryBox->addWidget( uDirectory );

    QLabel *labelDirectory = new QLabel( i18n("Only add selected file formats:"), dirOpenerWidget );
    box->addWidget( labelDirectory );

    QHBoxLayout *fileTypesBox = new QHBoxLayout();
    box->addLayout( fileTypesBox );

    QStringList codecList;
    fileTypes = new KListWidget( dirOpenerWidget );
    if( mode == Convert )
    {
        codecList = config->pluginLoader()->formatList( PluginLoader::Decode, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    }
    else if( mode == ReplayGain )
    {
        codecList = config->pluginLoader()->formatList( PluginLoader::ReplayGain, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    }
    for( int i = 0; i < codecList.size(); i++ )
    {
        if( codecList.at(i) == "audio cd" ) continue;
        QListWidgetItem *newItem = new QListWidgetItem( codecList.at(i), fileTypes );
        newItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        newItem->setCheckState( Qt::Checked );
    }

    QVBoxLayout *fileTypesFormatsBox = new QVBoxLayout();
    fileTypesBox->addLayout( fileTypesFormatsBox );

    fileTypesFormatsBox->addWidget( fileTypes );
    QLabel *formatHelp = new QLabel( "<a href=\"format-help\">" + i18n("Are you missing some file formats?") + "</a>", this );
    connect( formatHelp, SIGNAL(linkActivated(const QString&)), this, SLOT(showHelp()) );
    fileTypesFormatsBox->addWidget( formatHelp );

    QVBoxLayout *fileTypesButtonsBox = new QVBoxLayout();
    fileTypesBox->addLayout( fileTypesButtonsBox );
    fileTypesButtonsBox->addStretch();

    pSelectAll = new KPushButton( KIcon("edit-select-all"), i18n("Select all"), dirOpenerWidget );
    fileTypesButtonsBox->addWidget( pSelectAll );
    connect( pSelectAll, SIGNAL(clicked()), this, SLOT(selectAllClicked()) );

    pSelectNone = new KPushButton( KIcon("application-x-zerosize"), i18n("Select none"), dirOpenerWidget );
    fileTypesButtonsBox->addWidget( pSelectNone );
    connect( pSelectNone, SIGNAL(clicked()), this, SLOT(selectNoneClicked()) );

    cRecursive = new QCheckBox( i18n("Recursive"), dirOpenerWidget );
    cRecursive->setChecked( true );
    cRecursive->setToolTip( i18n("If checked, files from subdirectories will be added, too.") );
    fileTypesButtonsBox->addWidget( cRecursive );

    fileTypesButtonsBox->addStretch();


    // Conversion Options Widget

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    mainGrid->addWidget( options, 2, 0 );
    adjustSize();
    options->hide();


    const KUrl url = KFileDialog::getExistingDirectoryUrl( uDirectory->url(), this );
    if( !url.isEmpty() )
        uDirectory->setUrl( url );
    else
        dialogAborted = true;

        // Prevent the dialog from beeing too wide because of the directory history
    if( parent && width() > parent->width() )
        setInitialSize( QSize(parent->width()-10,sizeHint().height()) );
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "DirOpener" );
    restoreDialogSize( group );
}

DirOpener::~DirOpener()
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "DirOpener" );
    saveDialogSize( group );
}

void DirOpener::proceedClicked()
{
    if( page == DirOpenPage )
    {
        dirOpenerWidget->hide();
        options->show();
        page = ConversionOptionsPage;
        QFont font;
        font.setBold( false );
        lSelector->setFont( font );
        font.setBold( true );
        lOptions->setFont( font );
        setButtons( KDialog::Ok | KDialog::Cancel );
    }
}

void DirOpener::addClicked()
{
    QStringList selectedCodecs;
    for( int i = 0; i < fileTypes->count(); i++ )
    {
        if( fileTypes->item(i)->checkState() == Qt::Checked )
            selectedCodecs += fileTypes->item(i)->text();
    }

    if( mode == Convert )
    {
        ConversionOptions *conversionOptions = options->currentConversionOptions();
        if( conversionOptions )
        {
            hide();
            options->accepted();
            emit open( uDirectory->url(), cRecursive->checkState() == Qt::Checked, selectedCodecs, conversionOptions );
            accept();
        }
        else
        {
            KMessageBox::error( this, i18n("No conversion options selected.") );
        }
    }
    else if( mode == ReplayGain )
    {
        hide();
        emit open( uDirectory->url(), cRecursive->checkState() == Qt::Checked, selectedCodecs );
        accept();
    }
}

void DirOpener::selectAllClicked()
{
    for( int i = 0; i < fileTypes->count(); i++ )
    {
        fileTypes->item(i)->setCheckState( Qt::Checked );
    }
}

void DirOpener::selectNoneClicked()
{
    for( int i = 0; i < fileTypes->count(); i++ )
    {
        fileTypes->item(i)->setCheckState( Qt::Unchecked );
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


