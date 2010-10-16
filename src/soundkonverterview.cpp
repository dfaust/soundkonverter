/*
 * soundkonverterview.cpp
 *
 * Copyright (C) 2007 Daniel Faust <hessijames@gmail.com>
 */
#include "soundkonverterview.h"
#include "filelist.h"
#include "filelistitem.h"
#include "combobutton.h"
#include "progressindicator.h"
#include "optionslayer.h"
#include "config.h"
#include "opener/fileopener.h"
#include "opener/diropener.h"
#include "opener/cdopener.h"
#include "opener/urlopener.h"
#include "opener/playlistopener.h"
#include "convert.h"
#include "options.h"
#include "codecproblems.h"

#include <KLocale>
#include <KPushButton>
#include <KIcon>
#include <KFileDialog>
#include <KMenu>
#include <KAction>
#include <KActionMenu>
#include <KStandardDirs>

#include <QLabel>
#include <QLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTreeView>
#include <QToolButton>
// #include <QMessageBox>
#include <KMessageBox>

soundKonverterView::soundKonverterView( Logger *_logger, Config *_config, CDManager *_cdManager, QWidget *parent )
    : config( _config ),
      logger( _logger ),
      cdManager( _cdManager )
{
//     resize( 600, 400 );
    setAcceptDrops( true );

    // the grid for all widgets in the main window
    QGridLayout* gridLayout = new QGridLayout( this );
    gridLayout->setContentsMargins( 6, 6, 6, 6 );
//     gridLayout->setSpacing( 0 );

    fileList = new FileList( config, this );
    gridLayout->addWidget( fileList, 1, 0 );
    gridLayout->setRowStretch( 1, 1 );
    connect( fileList, SIGNAL(fileCountChanged(int)), this, SLOT(fileCountChanged(int)) );
    connect( fileList, SIGNAL(conversionStarted()), this, SLOT(conversionStarted()) );
    connect( fileList, SIGNAL(conversionStopped(int)), this, SLOT(conversionStopped(int)) );
    connect( fileList, SIGNAL(queueModeChanged(bool)), this, SLOT(queueModeChanged(bool)) );

    optionsLayer = new OptionsLayer( config, this );
    fileList->setOptionsLayer( optionsLayer );
    optionsLayer->hide();
//     optionsLayer->fadeIn();
    gridLayout->addWidget( optionsLayer, 1, 0 );
    connect( optionsLayer, SIGNAL(done(const KUrl::List&,ConversionOptions*,const QString&)), fileList, SLOT(addFiles(const KUrl::List&,ConversionOptions*,const QString&)) );


    // add a horizontal box layout for the add combobutton to the grid
    QHBoxLayout *addBox = new QHBoxLayout( 0 ); // TODO destroy
    addBox->setContentsMargins( 0, 0, 0, 0 );
//     addBox->setSpacing( 0 );
    gridLayout->addLayout( addBox, 3, 0 );

    // create the combobutton for adding files to the file list
    cAdd = new ComboButton( this );
    QFont font = cAdd->font();
    //font.setWeight( QFont::DemiBold );
    font.setPointSize( font.pointSize() + 3 );
    cAdd->setFont( font );
    cAdd->insertItem( KIcon("audio-x-generic"), i18n("Add files...") );
    cAdd->insertItem( KIcon("folder"), i18n("Add folder...") );
    cAdd->insertItem( KIcon("media-optical-audio"), i18n("Add CD tracks...") );
    cAdd->insertItem( KIcon("network-workgroup"), i18n("Add url...") );
    cAdd->insertItem( KIcon("view-media-playlist"), i18n("Add playlist...") );
    cAdd->increaseHeight( 6 );
    addBox->addWidget( cAdd );
    connect( cAdd, SIGNAL(clicked(int)), this, SLOT(addClicked(int)) );
    cAdd->setFocus();

    addBox->addSpacing( 10 );

    startAction = new KAction( KIcon("system-run"), i18n("Start"), this );
    connect( startAction, SIGNAL(triggered()), fileList, SLOT(startConversion()) );

    pStart = new KPushButton( KIcon("system-run"), i18n("Start"), this );
    pStart->setFixedHeight( pStart->size().height() );
    pStart->setEnabled( false );
    startAction->setEnabled( false );
    addBox->addWidget( pStart );
    connect( pStart, SIGNAL(clicked()), fileList, SLOT(startConversion()) );

    stopActionMenu = new KActionMenu( KIcon("process-stop"), i18n("Stop"), this );
    killAction = new KAction( KIcon("flag-red"), i18n("Stop imediatelly"), this );
    stopActionMenu->addAction( killAction );
    connect( killAction, SIGNAL(triggered()), fileList, SLOT(killConversion()) );
    stopAction = new KAction( KIcon("flag-yellow"), i18n("Stop after current conversions are completed"), this );
    stopActionMenu->addAction( stopAction );
    connect( stopAction, SIGNAL(triggered()), fileList, SLOT(stopConversion()) );
    continueAction = new KAction( KIcon("flag-green"), i18n("Continue after current conversions are completed"), this );
    stopActionMenu->addAction( continueAction );
    connect( continueAction, SIGNAL(triggered()), fileList, SLOT(continueConversion()) );
    queueModeChanged( true );

    pStop = new KPushButton( KIcon("process-stop"), i18n("Stop"), this );
    pStop->setFixedHeight( pStop->size().height() );
    pStop->hide();
    stopActionMenu->setEnabled( false );
    pStop->setMenu( stopActionMenu->menu() );
    addBox->addWidget( pStop );

    addBox->addSpacing( 10 );

    progressIndicator = new ProgressIndicator( /*systemTrayIcon,*/ this );
    addBox->addWidget( progressIndicator );
    connect( progressIndicator, SIGNAL(progressChanged(const QString&)), this, SIGNAL(progressChanged(const QString&)) );
    connect( fileList, SIGNAL(timeChanged(float)), progressIndicator, SLOT(timeChanged(float)) );
    connect( fileList, SIGNAL(finished(float)), progressIndicator, SLOT(finished(float)) );

    Convert *convert = new Convert( config, fileList, logger );
    connect( convert, SIGNAL(updateTime(float)), progressIndicator, SLOT(update(float)) );
    connect( convert, SIGNAL(timeFinished(float)), progressIndicator, SLOT(timeFinished(float)) );

    if( QFile::exists(KStandardDirs::locateLocal("data","soundkonverter/filelist_autosave.xml")) )
        fileList->load( false );
}

soundKonverterView::~soundKonverterView()
{}

void soundKonverterView::addClicked( int index )
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

void soundKonverterView::showFileDialog()
{
    FileOpener *dialog = new FileOpener( config, this );
//     dialog->resize( size().width() - 10, size().height() );

    connect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), fileList, SLOT(addFiles(const KUrl::List&,ConversionOptions*)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), 0, 0 );

    delete dialog;

    fileList->save( false );
}

void soundKonverterView::showDirDialog()
{
    DirOpener *dialog = new DirOpener( config, DirOpener::Convert, this );
    
    connect( dialog, SIGNAL(done(const KUrl&,bool,const QStringList&,ConversionOptions*)), fileList, SLOT(addDir(const KUrl&,bool,const QStringList&,ConversionOptions*)) );

    dialog->exec();
    
    disconnect( dialog, SIGNAL(done(const KUrl&,bool,const QStringList&,ConversionOptions*)), 0, 0 );

    delete dialog;

    fileList->save( false );
}

void soundKonverterView::showCdDialog( const QString& device, bool intern )
{
    /*
    ConversionOptions conversionOptions = options->getCurrentOptions();

    if( ( instances <= 1 || config->data.general.askForNewOptions ) && ( profile == "" || format == "" || directory == "" ) && !intern )
    {
        OptionsRequester* dialog = new OptionsRequester( config, "", this );

        connect( dialog, SIGNAL(setCurrentOptions(const ConversionOptions&)),
                 options, SLOT(setCurrentOptions(const ConversionOptions&))
               );
//         connect( dialog, SIGNAL(addFiles(QStringList)),
//                    fileList, SLOT(addFiles(QStringList))
//                  );

        Q_CHECK_PTR( dialog );

        if( profile != "" ) {
            dialog->setProfile( profile );
            profile = "";
        }
        if( format != "" ) {
            dialog->setFormat( format );
            format = "";
        }
        if( directory != "" ) {
            dialog->setOutputDirectory( directory );
            directory = "";
        }

        dialog->exec();

        disconnect( dialog, SIGNAL(setCurrentOptions(const ConversionOptions&)), 0, 0 );
//         disconnect( dialog, SIGNAL(addFiles(QStringList)), 0, 0 );

        delete dialog;
    }
    else
    {
        if( profile != "" ) {
            options->setProfile( profile );
            profile = "";
        }
        if( format != "" ) {
            options->setFormat( format );
            format = "";
        }
        if( directory != "" ) {
            options->setOutputDirectory( directory );
            directory = "";
        }
    }

    kapp->eventLoop()->exitLoop();
*/
    
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
        return;
    }

    // create a new CDOpener object for letting the user add some tracks from a CD
    CDOpener *dialog = new CDOpener( config, device, this );

    if( !dialog->noCdFound )
    {
        connect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*)), fileList, SLOT(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*)) );

        dialog->exec();

        disconnect( dialog, SIGNAL(addTracks(const QString&,QList<int>,int,QList<TagData*>,ConversionOptions*)), 0, 0 );
    }
    else
    {
//         KMessageBox::information( this, i18n("No audio CD found.") );
        KMessageBox::error( this, i18n("No CD device found") );
    }
    
    delete dialog;
/*
    kapp->eventLoop()->enterLoop();

    options->setCurrentOptions( conversionOptions );
*/

    fileList->save( false );
}

void soundKonverterView::showUrlDialog()
{
    UrlOpener *dialog = new UrlOpener( config, this );

    connect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), fileList, SLOT(addFiles(const KUrl::List&,ConversionOptions*)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), 0, 0 );

    delete dialog;

    fileList->save( false );
}

void soundKonverterView::showPlaylistDialog()
{
    PlaylistOpener *dialog = new PlaylistOpener( config, this );
//     dialog->resize( size().width() - 10, size().height() );

    connect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), fileList, SLOT(addFiles(const KUrl::List&,ConversionOptions*)) );

    dialog->exec();

    disconnect( dialog, SIGNAL(done(const KUrl::List&,ConversionOptions*)), 0, 0 );

    delete dialog;

    fileList->save( false );
}

void soundKonverterView::addConvertFiles( const KUrl::List& urls, QString _profile, QString _format, const QString& directory, const QString& notifyCommand )
{
    KUrl::List k_urls;
    QStringList errorList;
    //    codec    @0 files @1 solutions
    QMap< QString, QList<QStringList> > problems;
    QString fileName;
    
    for( int i=0; i<urls.size(); i++ )
    {
        QString codecName = config->pluginLoader()->getCodecFromFile( urls.at(i) );

        if( codecName == "inode/directory" || config->pluginLoader()->canDecode(codecName,&errorList) )
        {
            k_urls += urls.at(i);
        }
        else
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
        QString profile;
        QString format;
        QStringList formatList = config->pluginLoader()->formatList( PluginLoader::Encode, PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid) );
        for( int i=0; i<formatList.count(); i++ )
        {
            if( _format == formatList.at(i) || config->pluginLoader()->codecExtensions(formatList.at(i)).contains(_format) )
            {
                format = formatList.at(i);
                break;
            }
        }
        bool lossy = false;
        if( _profile.toLower() == i18n("Very low").toLower() || _profile.toLower() == "very low" || _profile.toLower() == "very_low" )
        {
            profile = i18n("Very low");
            lossy = true;
        }
        else if( _profile.toLower() == i18n("Low").toLower() || _profile.toLower() == "low" )
        {
            profile = i18n("Low");
            lossy = true;
        }
        else if( _profile.toLower() == i18n("Medium").toLower() || _profile.toLower() == "medium" )
        {
            profile = i18n("Medium");
            lossy = true;
        }
        else if( _profile.toLower() == i18n("High").toLower() || _profile.toLower() == "high" )
        {
            profile = i18n("High");
            lossy = true;
        }
        else if( _profile.toLower() == i18n("Very high").toLower() || _profile.toLower() == "very high" || _profile.toLower() == "very_high" )
        {
            profile = i18n("Very high");
            lossy = true;
        }
        else if( _profile.toLower() == i18n("Lossless").toLower() || _profile.toLower() == "lossless" )
        {
            profile = i18n("Lossless");
            format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Lossless).contains(format) ? format : "";
        }
        else if( _profile.toLower() == i18n("Hybrid").toLower() || _profile.toLower() == "hybrid" )
        {
            profile = i18n("Hybrid");
            format = config->pluginLoader()->formatList(PluginLoader::Encode,PluginLoader::Hybrid).contains(format) ? format : "";
        }
        else
        {
            for( int i=0; i<config->data.profiles.count(); i++ )
            {
                if( config->data.profiles.at(i).profileName == _profile )
                {
                    profile = _profile;
                    format = config->data.profiles.at(i).codecName;
                }
            }
        }
        
        if( lossy )
        {
            format = "";
            QStringList formatList = config->pluginLoader()->formatList( PluginLoader::Encode, PluginLoader::Lossy );
            for( int i=0; i<formatList.count(); i++ )
            {
                if( _format == formatList.at(i) || config->pluginLoader()->codecExtensions(formatList.at(i)).contains(_format) )
                {
                    format = formatList.at(i);
                    break;
                }
            }
        }

        if( !profile.isEmpty() && !format.isEmpty() && !directory.isEmpty() )
        {
            Options *options = new Options( config, "", 0 );
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
//                 KMessageBox::error( this, i18n("Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nsoundKonverterView::addConvertFiles; Options::currentConversionOptions returned 0"), i18n("Internal error") );
                KMessageBox::error( this, "Sorry, this shouldn't happen.\n\nPlease report this bug and attach the following error message:\n\nsoundKonverterView::addConvertFiles; Options::currentConversionOptions returned 0", "Internal error" );
            }
        }
        else
        {
            optionsLayer->addUrls( k_urls );
            if( !profile.isEmpty() ) optionsLayer->setProfile( profile );
            if( !format.isEmpty() ) optionsLayer->setFormat( format );
            if( !directory.isEmpty() ) optionsLayer->setOutputDirectory( directory );
            if( !notifyCommand.isEmpty() ) optionsLayer->setCommand( notifyCommand );
            optionsLayer->fadeIn();
        }
    }

    fileList->save( false );
}

void soundKonverterView::startConversion()
{
    fileList->startConversion();
}

void soundKonverterView::fileCountChanged( int count )
{
    pStart->setEnabled( count > 0 );
    startAction->setEnabled( count > 0 );
}

void soundKonverterView::conversionStarted()
{
    pStart->hide();
    startAction->setEnabled( false );
    pStop->show();
    stopActionMenu->setEnabled( true );
    emit signalConversionStarted();
}

void soundKonverterView::conversionStopped( int state )
{
    pStart->show();
    startAction->setEnabled( true );
    pStop->hide();
    stopActionMenu->setEnabled( false );
    emit signalConversionStopped( state );
}

void soundKonverterView::queueModeChanged( bool enabled )
{
    stopAction->setVisible( enabled );
    continueAction->setVisible( !enabled );    
}

void soundKonverterView::loadFileList( bool user )
{
    fileList->load( user );
}

void soundKonverterView::saveFileList( bool user )
{
    fileList->save( user );
}

void soundKonverterView::updateFileList()
{
    fileList->updateAllItems();
}

#include "soundkonverterview.moc"
