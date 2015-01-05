
#include "soundkonverterview.h"
#include "filelist.h"
#include "filelistitem.h"
#include "combobutton.h"
// #include "progressindicator.h"
#include "config.h"
#include "logger.h"
#include "opener/fileopener.h"
#include "opener/diropener.h"
#include "opener/cdopener.h"
#include "opener/urlopener.h"
#include "opener/playlistopener.h"
#include "convert.h"
#include "options.h"
#include "codecproblems.h"

#include <KLocalizedString>
#include <QPushButton>
#include <QIcon>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <KActionMenu>
#include <QMessageBox>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTreeView>
#include <QToolButton>
#include <QDebug>

SoundKonverterView::SoundKonverterView(Logger *_logger, Config *_config, QWidget *parent) :
    QWidget(parent),
    config( _config ),
    logger( _logger )
{
    ui.setupUi(this);

    ui.fileList->init(config, logger);

    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QFont font = ui.addComboButton->font();
    font.setPointSize(font.pointSize() + 3);
    ui.addComboButton->setFont(font);
    ui.addComboButton->insertItem(QIcon::fromTheme("audio-x-generic"),     i18n("Add files..."));
    ui.addComboButton->insertItem(QIcon::fromTheme("folder"),              i18n("Add folder..."));
    ui.addComboButton->insertItem(QIcon::fromTheme("media-optical-audio"), i18n("Add CD tracks..."));
    ui.addComboButton->insertItem(QIcon::fromTheme("network-workgroup"),   i18n("Add url..."));
    ui.addComboButton->insertItem(QIcon::fromTheme("view-media-playlist"), i18n("Add playlist..."));
    ui.addComboButton->increaseHeight(0.6 * fontHeight);
    ui.addComboButton->setFocus();

    connect(ui.addComboButton, SIGNAL(clicked(int)), this, SLOT(addClicked(int)));

    ui.stopButton->hide();

    startAction    = new QAction(QIcon::fromTheme("system-run"),  i18n("Start"), this);
    killAction     = new QAction(QIcon::fromTheme("flag-red"),    i18n("Stop immediatelly"), this);
    stopAction     = new QAction(QIcon::fromTheme("flag-yellow"), i18n("Stop after current conversions are completed"), this);
    continueAction = new QAction(QIcon::fromTheme("flag-green"),  i18n("Don't stop after current conversions are completed"), this);

    connect(startAction,    SIGNAL(triggered()), ui.fileList, SLOT(startConversion()));
    connect(killAction,     SIGNAL(triggered()), ui.fileList, SLOT(killConversion()));
    connect(stopAction,     SIGNAL(triggered()), ui.fileList, SLOT(stopConversion()));
    connect(continueAction, SIGNAL(triggered()), ui.fileList, SLOT(continueConversion()));

    stopActionMenu = new KActionMenu(QIcon::fromTheme("process-stop"), i18n("Stop"), this);
    stopActionMenu->setDelayed(false);
    stopActionMenu->addAction(killAction);
    stopActionMenu->addAction(stopAction);
    stopActionMenu->addAction(continueAction);

    queueModeChanged(true);

    setAcceptDrops(true);

    return;

    // the grid for all widgets in the main window
    QGridLayout* gridLayout = new QGridLayout( this );

    ui.fileList = new FileList( this );
    gridLayout->addWidget( ui.fileList, 1, 0 );
    gridLayout->setRowStretch( 1, 1 );
    connect( ui.fileList, SIGNAL(fileCountChanged(int)), this, SLOT(fileCountChanged(int)) );
    connect( ui.fileList, SIGNAL(conversionStarted()), this, SLOT(conversionStarted()) );
    connect( ui.fileList, SIGNAL(conversionStopped(bool)), this, SLOT(conversionStopped(bool)) );
    connect( ui.fileList, SIGNAL(queueModeChanged(bool)), this, SLOT(queueModeChanged(bool)) );
    connect( ui.fileList, SIGNAL(showLog(int)), this, SIGNAL(showLog(int)) );


    // add a horizontal box layout for the add combobutton to the grid
    QHBoxLayout *addBox = new QHBoxLayout();
    addBox->setContentsMargins( 1, 0, 1, 0 ); // extra margin - determined by experiments
    gridLayout->addLayout( addBox, 3, 0 );


    addBox->addSpacing( fontHeight );

    ui.startButton = new QPushButton( QIcon::fromTheme("system-run"), i18n("Start"), this );
    ui.startButton->setFixedHeight( ui.startButton->size().height() );
    ui.startButton->setEnabled( false );
    startAction->setEnabled( false );
    addBox->addWidget( ui.startButton, 0, Qt::AlignVCenter );
    connect( ui.startButton, SIGNAL(clicked()), ui.fileList, SLOT(startConversion()) );

    ui.stopButton = new QPushButton( QIcon::fromTheme("process-stop"), i18n("Stop"), this );
    ui.stopButton->setFixedHeight( ui.stopButton->size().height() );
    ui.stopButton->hide();
    stopActionMenu->setEnabled( false );
    ui.stopButton->setMenu( stopActionMenu->menu() );
    addBox->addWidget( ui.stopButton, 0, Qt::AlignVCenter );

    addBox->addSpacing( fontHeight );

//     progressIndicator = new ProgressIndicator( this, ProgressIndicator::Feature( ProgressIndicator::FeatureSpeed | ProgressIndicator::FeatureTime ) );
//     addBox->addWidget( progressIndicator, 0, Qt::AlignVCenter );
//     connect( progressIndicator, SIGNAL(progressChanged(const QString&)), this, SIGNAL(progressChanged(const QString&)) );
//     connect( ui.fileList, SIGNAL(timeChanged(float)), progressIndicator, SLOT(timeChanged(float)) );
//     connect( ui.fileList, SIGNAL(finished(bool)), progressIndicator, SLOT(finished(bool)) );
//
//     Convert *convert = new Convert( config, ui.fileList, logger, this );
//     connect( ui.fileList, SIGNAL(convertItem(FileListItem*)), convert, SLOT(add(FileListItem*)) );
//     connect( ui.fileList, SIGNAL(killItem(FileListItem*)), convert, SLOT(kill(FileListItem*)) );
//     connect( ui.fileList, SIGNAL(itemRemoved(FileListItem*)), convert, SLOT(itemRemoved(FileListItem*)) );
//     connect( convert, SIGNAL(finished(FileListItem*,FileListItem::ReturnCode,bool)), ui.fileList, SLOT(itemFinished(FileListItem*,FileListItem::ReturnCode,bool)) );
//     connect( convert, SIGNAL(rippingFinished(const QString&)), ui.fileList, SLOT(rippingFinished(const QString&)) );
//
//     connect( convert, SIGNAL(finishedProcess(int,bool,bool)), logger, SLOT(processCompleted(int,bool,bool)) );
//
//     connect( convert, SIGNAL(updateTime(float)), progressIndicator, SLOT(update(float)) );
//     connect( convert, SIGNAL(timeFinished(float)), progressIndicator, SLOT(timeFinished(float)) );
}

SoundKonverterView::~SoundKonverterView()
{
}

void SoundKonverterView::addClicked( int index )
{
    if( index == 0 )
    {
        showFileDialog();
    }
    else if( index == 1 )
    {
        showDirDialog();
    }
    else if( index == 2 )
    {
        showCdDialog();
    }
    else if( index == 3 )
    {
        showUrlDialog();
    }
    else
    {
        showPlaylistDialog();
    }
}

void SoundKonverterView::showFileDialog()
{
    FileOpener *dialog = new FileOpener( config, this );
//     dialog->resize( size().width() - 10, size().height() );

    if( !dialog->dialogAborted )
    {
        connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), ui.fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

        ui.fileList->save( false );
    }

    delete dialog;
}

void SoundKonverterView::showDirDialog()
{
    DirOpener *dialog = new DirOpener( config, DirOpener::Convert, this );

    if( !dialog->dialogAborted )
    {
        connect( dialog, SIGNAL(open(const QUrl&,bool,const QStringList&,ConversionOptions*)), ui.fileList, SLOT(addDir(const QUrl&,bool,const QStringList&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QUrl&,bool,const QStringList&,ConversionOptions*)), 0, 0 );

        ui.fileList->save( false );
    }

    delete dialog;
}

bool SoundKonverterView::showCdDialog( const QString& device, QString _profile, QString _format, const QString& directory, const QString& notifyCommand )
{
    QString profile = _profile;
    QString format = _format;

    cleanupParameters( &profile, &format );

    bool success = false;

    QString message;
    QStringList errorList;
    if( !config->pluginLoader()->canDecode("audio cd",&errorList) )
    {
        QList<CodecProblems::Problem> problemList;
        CodecProblems::Problem problem;
        problem.codecName = "audio cd";
        problem.solutions = errorList;
        problemList += problem;
        CodecProblems *problemsDialog = new CodecProblems( CodecProblems::AudioCd, problemList, this );
        problemsDialog->exec();
        return false;
    }

    // create a new CDOpener object for letting the user add some tracks from a CD
    CDOpener *dialog = new CDOpener( config, device, this );

    if( !dialog->noCdFound )
    {
        if( !profile.isEmpty() )
            dialog->setProfile( profile );

        if( !format.isEmpty() )
            dialog->setFormat( format );

        if( !directory.isEmpty() )
            dialog->setOutputDirectory( directory );

        if( !notifyCommand.isEmpty() )
            dialog->setCommand( notifyCommand );

        connect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)), ui.fileList, SLOT(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)), 0, 0 );

        if( dialog->result() == QDialog::Accepted )
        {
            success = true;
            ui.fileList->save( false );
        }
    }
    else
    {
        QMessageBox::information( this, i18n("No CD device found"), i18n("No CD device found") );
    }

    delete dialog;

    return success;
}

void SoundKonverterView::showUrlDialog()
{
    UrlOpener *dialog = new UrlOpener( config, this );

    connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), ui.fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

    delete dialog;

    ui.fileList->save( false );
}

void SoundKonverterView::showPlaylistDialog()
{
    PlaylistOpener *dialog = new PlaylistOpener( config, this );
//     dialog->resize( size().width() - 10, size().height() );

    if( !dialog->dialogAborted )
    {
        connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), ui.fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

        ui.fileList->save( false );
    }

    delete dialog;
}

void SoundKonverterView::addConvertFiles( const QList<QUrl>& urls, QString _profile, QString _format, const QString& directory, const QString& notifyCommand )
{
    QList<QUrl> k_urls;
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;

    const bool canDecodeAac = config->pluginLoader()->canDecode( "m4a/aac" );
    const bool canDecodeAlac = config->pluginLoader()->canDecode( "m4a/alac" );
    const bool checkM4a = ( !canDecodeAac || !canDecodeAlac ) && canDecodeAac != canDecodeAlac;

    for( int i=0; i<urls.size(); i++ )
    {
        QString mimeType;
        QString codecName = config->pluginLoader()->getCodecFromFile( urls.at(i), &mimeType, checkM4a );

        if( codecName == "inode/directory" || config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            k_urls += urls.at(i);
        }
        else
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
                problem.affectedFiles += i18n("... and %1 more files",problems.value(problem.codecName).at(0).count()-3);
            }
            problemList += problem;
        }
    }

    if( problemList.count() > 0 )
    {
        CodecProblems *problemsDialog = new CodecProblems( CodecProblems::Decode, problemList, this );
        problemsDialog->exec();
    }

    if( k_urls.count() > 0 )
    {
        QString profile = _profile;
        QString format = _format;

        cleanupParameters( &profile, &format );

        if( !profile.isEmpty() && !format.isEmpty() && !directory.isEmpty() )
        {
            Options *options = new Options(0);
            options->init(config, "");
            options->hide();
            options->setProfile( profile );
            options->setFormat( format );
            options->setOutputDirectory( directory );
            ConversionOptions *conversionOptions = options->currentConversionOptions();
            delete options;
            if( conversionOptions )
            {
                ui.fileList->addFiles( k_urls, conversionOptions, notifyCommand );
            }
            else
            {
                // FIXME error message, null pointer for conversion options
//                 QMessageBox::critical( this, "soundKonverter", i18n("Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nsoundKonverterView::addConvertFiles; Options::currentConversionOptions returned 0"), i18n("Internal error") );
                QMessageBox::critical( this, "soundKonverter", "Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nsoundKonverterView::addConvertFiles; Options::currentConversionOptions returned 0", "Internal error" );
            }
        }
        else
        {
//             optionsLayer->addUrls( k_urls );
//
//             if( !profile.isEmpty() )
//                 optionsLayer->setProfile( profile );
//
//             if( !format.isEmpty() )
//                 optionsLayer->setFormat( format );
//
//             if( !directory.isEmpty() )
//                 optionsLayer->setOutputDirectory( directory );
//
//             if( !notifyCommand.isEmpty() )
//                 optionsLayer->setCommand( notifyCommand );
//
//             optionsLayer->fadeIn();
        }
    }

    ui.fileList->save( false );
}

void SoundKonverterView::loadAutosaveFileList()
{
    ui.fileList->load( false );
}

void SoundKonverterView::startConversion()
{
    ui.fileList->startConversion();
}

void SoundKonverterView::killConversion()
{
    ui.fileList->killConversion();
}

void SoundKonverterView::fileCountChanged( int count )
{
    ui.startButton->setEnabled( count > 0 );
    startAction->setEnabled( count > 0 );
}

void SoundKonverterView::conversionStarted()
{
    ui.startButton->hide();
    startAction->setEnabled( false );
    ui.stopButton->show();
    stopActionMenu->setEnabled( true );
    emit signalConversionStarted();
}

void SoundKonverterView::conversionStopped( bool failed )
{
    ui.startButton->show();
    startAction->setEnabled( true );
    ui.stopButton->hide();
    stopActionMenu->setEnabled( false );
    emit signalConversionStopped( failed );
}

void SoundKonverterView::queueModeChanged( bool enabled )
{
    stopAction->setVisible( enabled );
    continueAction->setVisible( !enabled );
}

void SoundKonverterView::loadFileList( bool user )
{
    ui.fileList->load( user );
}

void SoundKonverterView::saveFileList( bool user )
{
    ui.fileList->save( user );
}

void SoundKonverterView::updateFileList()
{
    ui.fileList->updateAllItems();
}

void SoundKonverterView::cleanupParameters( QString *profile, QString *format )
{
    QString old_profile = *profile;
    QString old_format = *format;

    QString new_profile;
    QString new_format;

    const QStringList formatList = config->pluginLoader()->formatList( PluginLoader::Encode, PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
    if( formatList.contains(old_format) )
    {
        new_format = old_format;
    }
    else
    {
        foreach( const QString format, formatList )
        {
            if( config->pluginLoader()->codecExtensions(format).contains(old_format) )
            {
                new_format = format;
                break;
            }
        }
    }

    if( old_profile.toLower() == i18n("Very low").toLower() || old_profile.toLower() == "very low" || old_profile.toLower() == "very_low" )
    {
        new_profile = i18n("Very low");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy)).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("Low").toLower() || old_profile.toLower() == "low" )
    {
        new_profile = i18n("Low");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy)).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("Medium").toLower() || old_profile.toLower() == "medium" )
    {
        new_profile = i18n("Medium");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy)).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("High").toLower() || old_profile.toLower() == "high" )
    {
        new_profile = i18n("High");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy)).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("Very high").toLower() || old_profile.toLower() == "very high" || old_profile.toLower() == "very_high" )
    {
        new_profile = i18n("Very high");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy)).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("Lossless").toLower() || old_profile.toLower() == "lossless" )
    {
        new_profile = i18n("Lossless");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossless).contains(new_format) ? new_format : "";
    }
    else if( old_profile.toLower() == i18n("Hybrid").toLower() || old_profile.toLower() == "hybrid" )
    {
        new_profile = i18n("Hybrid");
        new_format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Hybrid).contains(new_format) ? new_format : "";
    }
    else if( config->data.profiles.contains(old_profile) )
    {
        new_profile = old_profile;
        ConversionOptions *conversionOptions = config->data.profiles.value( new_profile );
        if( conversionOptions )
            new_format += conversionOptions->codecName;
    }

    *profile = new_profile;
    *format = new_format;
}


#include "soundkonverterview.moc"
