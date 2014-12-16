
#include "soundkonverterview.h"
#include "filelist.h"
#include "filelistitem.h"
#include "combobutton.h"
#include "progressindicator.h"
#include "optionslayer.h"
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
    qDebug() << "soundKonverterView";

    setAcceptDrops( true );

    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    // the grid for all widgets in the main window
    QGridLayout* gridLayout = new QGridLayout( this );

    fileList = new FileList( logger, config, this );
    gridLayout->addWidget( fileList, 1, 0 );
    gridLayout->setRowStretch( 1, 1 );
    connect( fileList, SIGNAL(fileCountChanged(int)), this, SLOT(fileCountChanged(int)) );
    connect( fileList, SIGNAL(conversionStarted()), this, SLOT(conversionStarted()) );
    connect( fileList, SIGNAL(conversionStopped(bool)), this, SLOT(conversionStopped(bool)) );
    connect( fileList, SIGNAL(queueModeChanged(bool)), this, SLOT(queueModeChanged(bool)) );
    connect( fileList, SIGNAL(showLog(int)), this, SIGNAL(showLog(int)) );

    optionsLayer = new OptionsLayer( config, this );
    fileList->setOptionsLayer( optionsLayer );
    optionsLayer->hide();
    gridLayout->addWidget( optionsLayer, 1, 0 );
    connect( optionsLayer, SIGNAL(done(const QList<QUrl>&,ConversionOptions*,const QString&)), fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*,const QString&)) );
    connect( optionsLayer, SIGNAL(saveFileList()), fileList, SLOT(save()) );


    // add a horizontal box layout for the add combobutton to the grid
    QHBoxLayout *addBox = new QHBoxLayout();
    addBox->setContentsMargins( 1, 0, 1, 0 ); // extra margin - determined by experiments
    gridLayout->addLayout( addBox, 3, 0 );

    // create the combobutton for adding files to the file list
    cAdd = new ComboButton( this );
    QFont font = cAdd->font();
    //font.setWeight( QFont::DemiBold );
    font.setPointSize( font.pointSize() + 3 );
    cAdd->setFont( font );
    cAdd->insertItem( QIcon::fromTheme("audio-x-generic"), i18n("Add files...") );
    cAdd->insertItem( QIcon::fromTheme("folder"), i18n("Add folder...") );
    cAdd->insertItem( QIcon::fromTheme("media-optical-audio"), i18n("Add CD tracks...") );
    cAdd->insertItem( QIcon::fromTheme("network-workgroup"), i18n("Add url...") );
    cAdd->insertItem( QIcon::fromTheme("view-media-playlist"), i18n("Add playlist...") );
    cAdd->increaseHeight( 0.6*fontHeight );
    addBox->addWidget( cAdd, 0, Qt::AlignVCenter );
    connect( cAdd, SIGNAL(clicked(int)), this, SLOT(addClicked(int)) );
    cAdd->setFocus();

    addBox->addSpacing( fontHeight );

    startAction = new QAction( QIcon::fromTheme("system-run"), i18n("Start"), this );
    connect( startAction, SIGNAL(triggered()), fileList, SLOT(startConversion()) );

    pStart = new QPushButton( QIcon::fromTheme("system-run"), i18n("Start"), this );
    pStart->setFixedHeight( pStart->size().height() );
    pStart->setEnabled( false );
    startAction->setEnabled( false );
    addBox->addWidget( pStart, 0, Qt::AlignVCenter );
    connect( pStart, SIGNAL(clicked()), fileList, SLOT(startConversion()) );

    stopActionMenu = new KActionMenu( QIcon::fromTheme("process-stop"), i18n("Stop"), this );
    stopActionMenu->setDelayed( false );
    killAction = new QAction( QIcon::fromTheme("flag-red"), i18n("Stop immediatelly"), this );
    stopActionMenu->addAction( killAction );
    connect( killAction, SIGNAL(triggered()), fileList, SLOT(killConversion()) );
    stopAction = new QAction( QIcon::fromTheme("flag-yellow"), i18n("Stop after current conversions are completed"), this );
    stopActionMenu->addAction( stopAction );
    connect( stopAction, SIGNAL(triggered()), fileList, SLOT(stopConversion()) );
    continueAction = new QAction( QIcon::fromTheme("flag-green"), i18n("Continue after current conversions are completed"), this );
    stopActionMenu->addAction( continueAction );
    connect( continueAction, SIGNAL(triggered()), fileList, SLOT(continueConversion()) );
    queueModeChanged( true );

    pStop = new QPushButton( QIcon::fromTheme("process-stop"), i18n("Stop"), this );
    pStop->setFixedHeight( pStop->size().height() );
    pStop->hide();
    stopActionMenu->setEnabled( false );
    pStop->setMenu( stopActionMenu->menu() );
    addBox->addWidget( pStop, 0, Qt::AlignVCenter );

    addBox->addSpacing( fontHeight );

    progressIndicator = new ProgressIndicator( this, ProgressIndicator::Feature( ProgressIndicator::FeatureSpeed | ProgressIndicator::FeatureTime ) );
    addBox->addWidget( progressIndicator, 0, Qt::AlignVCenter );
    connect( progressIndicator, SIGNAL(progressChanged(const QString&)), this, SIGNAL(progressChanged(const QString&)) );
    connect( fileList, SIGNAL(timeChanged(float)), progressIndicator, SLOT(timeChanged(float)) );
    connect( fileList, SIGNAL(finished(bool)), progressIndicator, SLOT(finished(bool)) );

    Convert *convert = new Convert( config, fileList, logger, this );
    connect( fileList, SIGNAL(convertItem(FileListItem*)), convert, SLOT(add(FileListItem*)) );
    connect( fileList, SIGNAL(killItem(FileListItem*)), convert, SLOT(kill(FileListItem*)) );
    connect( fileList, SIGNAL(itemRemoved(FileListItem*)), convert, SLOT(itemRemoved(FileListItem*)) );
    connect( convert, SIGNAL(finished(FileListItem*,FileListItem::ReturnCode,bool)), fileList, SLOT(itemFinished(FileListItem*,FileListItem::ReturnCode,bool)) );
    connect( convert, SIGNAL(rippingFinished(const QString&)), fileList, SLOT(rippingFinished(const QString&)) );

    connect( convert, SIGNAL(finishedProcess(int,bool,bool)), logger, SLOT(processCompleted(int,bool,bool)) );

    connect( convert, SIGNAL(updateTime(float)), progressIndicator, SLOT(update(float)) );
    connect( convert, SIGNAL(timeFinished(float)), progressIndicator, SLOT(timeFinished(float)) );
}

SoundKonverterView::~SoundKonverterView()
{}

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
        connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

        fileList->save( false );
    }

    delete dialog;
}

void SoundKonverterView::showDirDialog()
{
    DirOpener *dialog = new DirOpener( config, DirOpener::Convert, this );

    if( !dialog->dialogAborted )
    {
        connect( dialog, SIGNAL(open(const QUrl&,bool,const QStringList&,ConversionOptions*)), fileList, SLOT(addDir(const QUrl&,bool,const QStringList&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QUrl&,bool,const QStringList&,ConversionOptions*)), 0, 0 );

        fileList->save( false );
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

        connect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)), fileList, SLOT(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*,const QString&)), 0, 0 );

        if( dialog->result() == QDialog::Accepted )
        {
            success = true;
            fileList->save( false );
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

    connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

    delete dialog;

    fileList->save( false );
}

void SoundKonverterView::showPlaylistDialog()
{
    PlaylistOpener *dialog = new PlaylistOpener( config, this );
//     dialog->resize( size().width() - 10, size().height() );

    if( !dialog->dialogAborted )
    {
        connect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), fileList, SLOT(addFiles(const QList<QUrl>&,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(open(const QList<QUrl>&,ConversionOptions*)), 0, 0 );

        fileList->save( false );
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
                fileList->addFiles( k_urls, conversionOptions, notifyCommand );
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
            optionsLayer->addUrls( k_urls );

            if( !profile.isEmpty() )
                optionsLayer->setProfile( profile );

            if( !format.isEmpty() )
                optionsLayer->setFormat( format );

            if( !directory.isEmpty() )
                optionsLayer->setOutputDirectory( directory );

            if( !notifyCommand.isEmpty() )
                optionsLayer->setCommand( notifyCommand );

            optionsLayer->fadeIn();
        }
    }

    fileList->save( false );
}

void SoundKonverterView::loadAutosaveFileList()
{
    fileList->load( false );
}

void SoundKonverterView::startConversion()
{
    fileList->startConversion();
}

void SoundKonverterView::killConversion()
{
    fileList->killConversion();
}

void SoundKonverterView::fileCountChanged( int count )
{
    pStart->setEnabled( count > 0 );
    startAction->setEnabled( count > 0 );
}

void SoundKonverterView::conversionStarted()
{
    pStart->hide();
    startAction->setEnabled( false );
    pStop->show();
    stopActionMenu->setEnabled( true );
    emit signalConversionStarted();
}

void SoundKonverterView::conversionStopped( bool failed )
{
    pStart->show();
    startAction->setEnabled( true );
    pStop->hide();
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
    fileList->load( user );
}

void SoundKonverterView::saveFileList( bool user )
{
    fileList->save( user );
}

void SoundKonverterView::updateFileList()
{
    fileList->updateAllItems();
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
