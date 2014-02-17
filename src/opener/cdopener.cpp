
#include "cdopener.h"
#include "../metadata/tagengine.h"
#include "../config.h"
#include "../options.h"
#include "../outputdirectory.h"
#include "../global.h"

#include <KLocale>
#include <KPushButton>
#include <KLineEdit>
#include <KComboBox>
#include <KNumInput>
#include <KTextEdit>
#include <KFileDialog>
#include <KMessageBox>
#include <KStandardDirs>
#include <KInputDialog>

#include <QApplication>
#include <QLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTreeWidget>
#include <QDateTime>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QCheckBox>
#include <QHeaderView>

#include <solid/device.h>
#include <solid/block.h>
#include <solid/opticaldisc.h>


PlayerWidget::PlayerWidget( Phonon::MediaObject *mediaObject, int _track, QTreeWidgetItem *_treeWidgetItem, QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
    track( _track ),
    m_treeWidgetItem( _treeWidgetItem )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QHBoxLayout *trackPlayerBox = new QHBoxLayout();
    setLayout( trackPlayerBox );

    pStartPlayback = new KPushButton( KIcon("media-playback-start"), "", this );
    pStartPlayback->setFixedSize( 1.5*fontHeight, 1.5*fontHeight );
    trackPlayerBox->addWidget( pStartPlayback );
    connect( pStartPlayback, SIGNAL(clicked()), this, SLOT(startPlaybackClicked()) );
    pStopPlayback = new KPushButton( KIcon("media-playback-stop"), "", this );
    pStopPlayback->setFixedSize( 1.5*fontHeight, 1.5*fontHeight );
    pStopPlayback->hide();
    trackPlayerBox->addWidget( pStopPlayback );
    connect( pStopPlayback, SIGNAL(clicked()), this, SLOT(stopPlaybackClicked()) );
    seekSlider = new Phonon::SeekSlider( this );
    seekSlider->setMediaObject( mediaObject );
    seekSlider->setIconVisible( false );
    seekSlider->hide();
    trackPlayerBox->addWidget( seekSlider, 1 );
    trackPlayerBox->addStretch();
}

PlayerWidget::~PlayerWidget()
{}

void PlayerWidget::startPlaybackClicked()
{
/*    playing = true;
    pStartPlayback->hide();
    pStopPlayback->show();
    seekSlider->show();*/
    emit startPlayback( track );
}

void PlayerWidget::stopPlaybackClicked()
{
/*    playing = false;
    pStartPlayback->show();
    pStopPlayback->hide();
    seekSlider->hide();*/
    emit stopPlayback();
}

void PlayerWidget::trackChanged( int _track )
{
    if( _track != track )
    {
        playing = false;
        pStartPlayback->show();
        pStopPlayback->hide();
        seekSlider->hide();
    }
    else
    {
        playing = true;
        pStartPlayback->hide();
        pStopPlayback->show();
        seekSlider->show();
    }
}


CDOpener::CDOpener( Config *_config, const QString& _device, QWidget *parent, Qt::WFlags f )
    : KDialog( parent, f ),
    noCdFound( false ),
    config( _config ),
    cdDrive( 0 ),
    cdParanoia( 0 ),
    cddb( 0 ),
    cdTextFound( false ),
    cddbFound( false )
{
    setButtons( 0 );

    page = CdOpenPage;

    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    // let the dialog look nice
    setCaption( i18n("Add CD tracks") );
    setWindowIcon( KIcon("media-optical-audio") );

    QWidget *widget = new QWidget( this );
    QGridLayout *mainGrid = new QGridLayout( widget );
    QGridLayout *topGrid = new QGridLayout();
    mainGrid->addLayout( topGrid, 0, 0 );
    setMainWidget( widget );

    lSelector = new QLabel( i18n("1. Select CD tracks"), widget );
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


    // CD Opener Widget

    cdOpenerWidget = new QWidget( widget );
    mainGrid->addWidget( cdOpenerWidget, 2, 0 );

    // the grid for all widgets in the dialog
    QGridLayout *gridLayout = new QGridLayout( cdOpenerWidget );

    // the box for the cover and artist/album grid
    QHBoxLayout *topBoxLayout = new QHBoxLayout();
    gridLayout->addLayout( topBoxLayout, 0, 0 );

    // the album cover
    QLabel *lAlbumCover = new QLabel( "", cdOpenerWidget );
    topBoxLayout->addWidget( lAlbumCover );
    lAlbumCover->setPixmap( QPixmap( KStandardDirs::locate("data","soundkonverter/images/nocover.png") ) );
    lAlbumCover->setContentsMargins( 0, 0, 0.5*fontHeight, 0 );

    // the grid for the artist and album input
    QGridLayout *topGridLayout = new QGridLayout();
    topBoxLayout->addLayout( topGridLayout );

    // set up the first row at the top
    QLabel *lArtistLabel = new QLabel( i18n("Album artist:"), cdOpenerWidget );
    topGridLayout->addWidget( lArtistLabel, 0, 0 );
    lArtist = new KLineEdit( cdOpenerWidget );
    topGridLayout->addWidget( lArtist, 0, 1 );
    connect( lArtist, SIGNAL(textChanged(const QString&)), this, SLOT(artistChanged(const QString&)) );

    // set up the second row at the top
    QLabel *lAlbumLabel = new QLabel( i18n("Album:"), cdOpenerWidget );
    topGridLayout->addWidget( lAlbumLabel, 1, 0 );
    lAlbum = new KLineEdit( cdOpenerWidget );
    topGridLayout->addWidget( lAlbum, 1, 1 );

    // set up the third row at the top
    QLabel *lDiscLabel = new QLabel( i18n("Disc No.:"), cdOpenerWidget );
    topGridLayout->addWidget( lDiscLabel, 2, 0 );
    // add a horizontal box layout for the year and genre
    QHBoxLayout *yearBox = new QHBoxLayout();
    topGridLayout->addLayout( yearBox, 2, 1 );
    // and fill it up
    iDisc = new KIntSpinBox( 1, 99, 1, 1, cdOpenerWidget );
    yearBox->addWidget( iDisc );
    QLabel *lDiscTotalLabel = new QLabel( i18nc("Track/Disc No. x of y","of"), cdOpenerWidget );
    lDiscTotalLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    yearBox->addWidget( lDiscTotalLabel );
    iDiscTotal = new KIntSpinBox( 1, 99, 1, 1, cdOpenerWidget );
    yearBox->addWidget( iDiscTotal );
    QLabel *lYearLabel = new QLabel( i18n("Year:"), cdOpenerWidget );
    lYearLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    yearBox->addWidget( lYearLabel );
    iYear = new KIntSpinBox( 0, 99999, 1, QDate::currentDate().year(), cdOpenerWidget );
    yearBox->addWidget( iYear );
    QLabel *lGenreLabel = new QLabel( i18n("Genre:"), cdOpenerWidget );
    lGenreLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    yearBox->addWidget( lGenreLabel );
    cGenre = new KComboBox( true, cdOpenerWidget );
    cGenre->addItems( config->tagEngine()->genreList );
    cGenre->setEditText( "" );
    KCompletion *cGenreCompletion = cGenre->completionObject();
    cGenreCompletion->insertItems( config->tagEngine()->genreList );
    cGenreCompletion->setIgnoreCase( true );
    yearBox->addWidget( cGenre );

    topGridLayout->setColumnStretch( 1, 1 );


    // generate the list view for the tracks
    trackList = new QTreeWidget( cdOpenerWidget );
    gridLayout->addWidget( trackList, 1, 0 );
    // and fill in the headers
    trackList->setColumnCount( 5 );
    QStringList labels;
    labels.append( i18nc("column title","Rip") );
    labels.append( i18n("Track") );
    labels.append( i18n("Artist") );
    labels.append( i18n("Composer") );
    labels.append( i18n("Title") );
    labels.append( i18n("Length") );
    labels.append( i18n("Player") );
    trackList->setHeaderLabels( labels );
    trackList->setSelectionBehavior( QAbstractItemView::SelectRows );
    trackList->setSelectionMode( QAbstractItemView::ExtendedSelection );
    trackList->setSortingEnabled( false );
    trackList->setRootIsDecorated( false );
    trackList->header()->setResizeMode( Column_Artist, QHeaderView::ResizeToContents );
    trackList->header()->setResizeMode( Column_Composer, QHeaderView::ResizeToContents );
    trackList->header()->setResizeMode( Column_Title, QHeaderView::ResizeToContents );
//     trackList->setMouseTracking( true );
    connect( trackList, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );
//     connect( trackList, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(itemHighlighted(QTreeWidgetItem*,int)) );
    gridLayout->setRowStretch( 1, 1 );


    // create the box at the bottom for editing the tags
    tagGroupBox = new QGroupBox( i18n("No track selected"), cdOpenerWidget );
    gridLayout->addWidget( tagGroupBox, 2, 0 );
    QGridLayout *tagGridLayout = new QGridLayout( tagGroupBox );

    // add the up and down buttons
    pTrackUp = new KPushButton( "", tagGroupBox );
    pTrackUp->setIcon( KIcon("arrow-up") );
    pTrackUp->setFixedSize( pTrackUp->sizeHint().height(), pTrackUp->sizeHint().height() );
    pTrackUp->setAutoRepeat( true );
    connect( pTrackUp, SIGNAL(clicked()), this, SLOT(trackUpPressed()) );
    tagGridLayout->addWidget( pTrackUp, 0, 0 );
    pTrackDown = new KPushButton( "", tagGroupBox );
    pTrackDown->setIcon( KIcon("arrow-down") );
    pTrackDown->setFixedSize( pTrackDown->sizeHint().height(), pTrackDown->sizeHint().height() );
    pTrackDown->setAutoRepeat( true );
    connect( pTrackDown, SIGNAL(clicked()), this, SLOT(trackDownPressed()) );
    tagGridLayout->addWidget( pTrackDown, 1, 0 );

    // add the inputs
    // add a horizontal box layout for the title
    QHBoxLayout *trackTitleBox = new QHBoxLayout();
    tagGridLayout->addLayout( trackTitleBox, 0, 2 );
    // and fill it up
    QLabel *lTrackTitleLabel = new QLabel( i18n("Title:"), tagGroupBox );
    tagGridLayout->addWidget( lTrackTitleLabel, 0, 1 );
    lTrackTitle = new KLineEdit( tagGroupBox );
    trackTitleBox->addWidget( lTrackTitle );
    connect( lTrackTitle, SIGNAL(textChanged(const QString&)), this, SLOT(trackTitleChanged(const QString&)) );
    pTrackTitleEdit = new KPushButton( "", tagGroupBox );
    pTrackTitleEdit->setIcon( KIcon("document-edit") );
    pTrackTitleEdit->setFixedSize( lTrackTitle->sizeHint().height(), lTrackTitle->sizeHint().height() );
    pTrackTitleEdit->hide();
    trackTitleBox->addWidget( pTrackTitleEdit );
    connect( pTrackTitleEdit, SIGNAL(clicked()), this, SLOT(editTrackTitleClicked()) );
    // add a horizontal box layout for the composer
    QHBoxLayout *trackArtistBox = new QHBoxLayout();
    tagGridLayout->addLayout( trackArtistBox, 1, 2 );
    // and fill it up
    QLabel *lTrackArtistLabel = new QLabel( i18n("Artist:"), tagGroupBox );
    tagGridLayout->addWidget( lTrackArtistLabel, 1, 1 );
    lTrackArtist = new KLineEdit( tagGroupBox );
    trackArtistBox->addWidget( lTrackArtist );
    connect( lTrackArtist, SIGNAL(textChanged(const QString&)), this, SLOT(trackArtistChanged(const QString&)) );
    pTrackArtistEdit = new KPushButton( "", tagGroupBox );
    pTrackArtistEdit->setIcon( KIcon("document-edit") );
    pTrackArtistEdit->setFixedSize( lTrackArtist->sizeHint().height(), lTrackArtist->sizeHint().height() );
    pTrackArtistEdit->hide();
    trackArtistBox->addWidget( pTrackArtistEdit );
    connect( pTrackArtistEdit, SIGNAL(clicked()), this, SLOT(editTrackArtistClicked()) );
    QLabel *lTrackComposerLabel = new QLabel( i18n("Composer:"), tagGroupBox );
    trackArtistBox->addWidget( lTrackComposerLabel );
    lTrackComposer = new KLineEdit( tagGroupBox );
    trackArtistBox->addWidget( lTrackComposer );
    connect( lTrackComposer, SIGNAL(textChanged(const QString&)), this, SLOT(trackComposerChanged(const QString&)) );
    pTrackComposerEdit = new KPushButton( "", tagGroupBox );
    pTrackComposerEdit->setIcon( KIcon("document-edit") );
    pTrackComposerEdit->setFixedSize( lTrackComposer->sizeHint().height(), lTrackComposer->sizeHint().height() );
    pTrackComposerEdit->hide();
    trackArtistBox->addWidget( pTrackComposerEdit );
    connect( pTrackComposerEdit, SIGNAL(clicked()), this, SLOT(editTrackComposerClicked()) );
    // add a horizontal box layout for the comment
    QHBoxLayout *trackCommentBox = new QHBoxLayout();
    tagGridLayout->addLayout( trackCommentBox, 2, 2 );
    // and fill it up
    QLabel *lTrackCommentLabel = new QLabel( i18n("Comment:"), tagGroupBox );
    tagGridLayout->addWidget( lTrackCommentLabel, 2, 1 );
    tTrackComment = new KTextEdit( tagGroupBox );
    trackCommentBox->addWidget( tTrackComment );
    tTrackComment->setFixedHeight( 4*fontHeight );
    connect( tTrackComment, SIGNAL(textChanged()), this, SLOT(trackCommentChanged()) );
    pTrackCommentEdit = new KPushButton( "", tagGroupBox );
    pTrackCommentEdit->setIcon( KIcon("document-edit") );
    pTrackCommentEdit->setFixedSize( lTrackTitle->sizeHint().height(), lTrackTitle->sizeHint().height() );
    pTrackCommentEdit->hide();
    trackCommentBox->addWidget( pTrackCommentEdit );
    connect( pTrackCommentEdit, SIGNAL(clicked()), this, SLOT(editTrackCommentClicked()) );


    audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
    audioOutput->setVolume( 0.5 );
    mediaObject = new Phonon::MediaObject( this );
    mediaObject->setTickInterval( 500 );

    Phonon::createPath( mediaObject, audioOutput );

    mediaController = new Phonon::MediaController( mediaObject );
    mediaController->setAutoplayTitles( false );

    connect( mediaController, SIGNAL(titleChanged(int)), this, SLOT(playbackTitleChanged(int)) );
    connect( mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(playbackStateChanged(Phonon::State,Phonon::State)) );



    // Cd Opener Overlay Widget

    cdOpenerOverlayWidget = new QWidget( widget );
    mainGrid->addWidget( cdOpenerOverlayWidget, 2, 0 );
    QHBoxLayout *cdOpenerOverlayLayout = new QHBoxLayout();
    cdOpenerOverlayWidget->setLayout( cdOpenerOverlayLayout );
//     lOverlayLabel = new QLabel( i18n("Please wait, trying to read audio CD ..."), cdOpenerOverlayWidget );
    lOverlayLabel = new QLabel( cdOpenerOverlayWidget );
    cdOpenerOverlayLayout->addWidget( lOverlayLabel );
    lOverlayLabel->setAlignment( Qt::AlignCenter );
    cdOpenerOverlayWidget->setAutoFillBackground( true );
    QPalette newPalette = cdOpenerOverlayWidget->palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192 ) );
    cdOpenerOverlayWidget->setPalette( newPalette );


    // Conversion Options Widget

    // add a vertical box layout for the options widget
    QVBoxLayout *optionsBox = new QVBoxLayout();
    mainGrid->addLayout( optionsBox, 2, 0 );

    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    optionsBox->addWidget( options );
    options->hide();
    optionsBox->addStretch();


    // draw a horizontal line
    QFrame *buttonLineFrame = new QFrame( widget );
    buttonLineFrame->setFrameShape( QFrame::HLine );
    buttonLineFrame->setFrameShadow( QFrame::Sunken );
    buttonLineFrame->setFrameShape( QFrame::HLine );
    mainGrid->addWidget( buttonLineFrame, 4, 0 );

    // add a horizontal box layout for the control elements
    QHBoxLayout *controlBox = new QHBoxLayout();
    mainGrid->addLayout( controlBox, 5, 0 );

    // add the control elements
    pSaveCue = new KPushButton( KIcon("document-save"), i18n("Save cue sheet..."), widget );
    controlBox->addWidget( pSaveCue );
    connect( pSaveCue, SIGNAL(clicked()), this, SLOT(saveCuesheetClicked()) );
    controlBox->addSpacing( fontHeight );

    pCDDB = new KPushButton( KIcon("download"), i18n("Request CDDB"), widget );
    controlBox->addWidget( pCDDB );
    connect( pCDDB, SIGNAL(clicked()), this, SLOT(requestCddb()) );
    controlBox->addStretch();

    cEntireCd = new QCheckBox( i18n("Rip entire CD to one file"), widget );
    QStringList errorList;
    cEntireCd->setEnabled( config->pluginLoader()->canRipEntireCd(&errorList) );
    if( !cEntireCd->isEnabled() )
    {
        QPalette notificationPalette = cEntireCd->palette();
        notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(174,127,130) ); // QColor(181,96,101)
        cEntireCd->setPalette( notificationPalette );
        if( !errorList.isEmpty() )
        {
            errorList.prepend( i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPossible solutions are listed below.\n") );
        }
        else
        {
            errorList += i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPlease check your distribution's package manager in order to install an additional ripper plugin which supports ripping to one file.");
        }
        cEntireCd->setToolTip( errorList.join("\n") );
    }
    controlBox->addWidget( cEntireCd );
    controlBox->addSpacing( 2*fontHeight );

    pProceed = new KPushButton( KIcon("go-next"), i18n("Proceed"), widget );
    controlBox->addWidget( pProceed );
    connect( pProceed, SIGNAL(clicked()), this, SLOT(proceedClicked()) );
    pAdd = new KPushButton( KIcon("dialog-ok"), i18n("Ok"), widget );
    controlBox->addWidget( pAdd );
    pAdd->hide();
    connect( pAdd, SIGNAL(clicked()), this, SLOT(addClicked()) );
    pCancel = new KPushButton( KIcon("dialog-cancel"), i18n("Cancel"), widget );
    controlBox->addWidget( pCancel );
    connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()) );

    connect( &fadeTimer, SIGNAL(timeout()), this, SLOT(fadeAnim()) );
    fadeAlpha = 255.0f;


    cddb = new KCDDB::Client();
    connect( cddb, SIGNAL(finished(KCDDB::Result)), this, SLOT(lookup_cddb_done(KCDDB::Result)) );


    // set up timeout timer
    timeoutTimer.setSingleShot( true );
    connect( &timeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()) );


    if( !_device.isEmpty() )
    {
        device = _device;
    }
    else
    {
        const QMap<QString,QString> devices = cdDevices();
        if( devices.count() <= 0 )
        {
            noCdFound = true;
            return;
        }
        else if( devices.count() == 1 )
        {
            device = devices.keys().at(0);
        }
        else
        {
            QStringList list;
            foreach( const QString desc, devices.values() )
            {
                list.append( desc );
            }
            bool ok = false;
            const QString selection = KInputDialog::getItem( i18n("Select CD-ROM drive"), i18n("Multiple CD-ROM drives where found. Please select one:"), list, 0, false, &ok, this );

            if( ok )
            {
                // The user selected an item and pressed OK
                device = devices.keys().at(list.indexOf(selection));
            }
            else
            {
                noCdFound = true;
                return;
            }
        }
    }

    const bool success = openCdDevice( device );
    if( !success )
    {
        KMessageBox::information(this,"success = false, couldn't open audio device.\nplease report this bug.");
        noCdFound = true;
        return;
    }


    mediaSource = new Phonon::MediaSource( Phonon::Cd, device );
    mediaObject->setCurrentSource( *mediaSource ); // WARNING doesn't work with phonon-xine


    // Prevent the dialog from beeing too wide because of the directory history
    if( parent && width() > parent->width() )
        setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );

    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "CDOpener" );
    restoreDialogSize( group );
}

CDOpener::~CDOpener()
{
    KSharedConfig::Ptr conf = KGlobal::config();
    KConfigGroup group = conf->group( "CDOpener" );
    saveDialogSize( group );

    if( cdParanoia )
    {
        paranoia_free( cdParanoia );
//         delete cdParanoia;
    }
    if( cdDrive )
    {
        cdda_close( cdDrive );
//         delete cdDrive;
    }

    if( cddb )
        delete cddb;
}

void CDOpener::setProfile( const QString& profile )
{
    options->setProfile( profile );
}

void CDOpener::setFormat( const QString& format )
{
    options->setFormat( format );
}

void CDOpener::setOutputDirectory( const QString& directory )
{
    options->setOutputDirectory( directory );
}

void CDOpener::setCommand( const QString& _command )
{
    command = _command;
}

QMap<QString,QString> CDOpener::cdDevices()
{
    QMap<QString,QString> devices;

    QList<Solid::Device> solidDevices = Solid::Device::listFromType(Solid::DeviceInterface::OpticalDisc, QString());
    foreach( Solid::Device solidDevice, solidDevices )
    {
        Solid::OpticalDisc *opticalDisc = solidDevice.as<Solid::OpticalDisc>();
        if( opticalDisc && opticalDisc->availableContent() & Solid::OpticalDisc::Audio )
        {
            Solid::Block *block = solidDevice.as<Solid::Block>();
            if( block )
            {
                const QString device = block->device();
                const Solid::Device parentDevice( solidDevice.parentUdi() );
                const QString name = parentDevice.product();

                cdDrive = cdda_identify( device.toAscii(), CDDA_MESSAGE_PRINTIT, 0 );
                if( cdDrive && cdda_open(cdDrive) == 0 )
                {
                    const QString desc = i18n("%1 (%2): Audio CD with %3 tracks").arg(name).arg(device).arg(cdda_audio_tracks(cdDrive));
                    devices.insert( device, desc );
                }
            }
        }
    }

    return devices;
}

int CDOpener::cdda_audio_tracks( cdrom_drive *cdDrive ) const
{
    const int nrOfTracks = cdda_tracks(cdDrive);
    for( int i=1; i<=nrOfTracks; i++ )
    {
        if( !(IS_AUDIO(cdDrive,i-1)) )
            return i-1;
    }
    return nrOfTracks;
}

bool CDOpener::openCdDevice( const QString& _device )
{
    // paranoia init

    QFile deviceFile(_device);

    if( !deviceFile.exists() )
    {
        return false;
    }
    else
    {
        cdDrive = cdda_identify( _device.toAscii(), CDDA_MESSAGE_PRINTIT, 0 );
        if( !cdDrive || cdda_open( cdDrive ) != 0 )
        {
            return false;
        }
    }
    cdParanoia = paranoia_init( cdDrive );

    // cd text

//     const int status = wm_cd_init( device.toAscii().data(), "", "", "", &wmHandle );
//
//     struct cdtext_info *info = 0;
//
//     if( !WM_CDS_ERROR(status) )
//     {
//         info = wm_cd_get_cdtext( wmHandle );
//
//         if( !info || !info->valid || info->count_of_entries != cdda_tracks(cdDrive) )
//         if( !info || !info->valid )
//         {
//             kDebug() << "no or invalid CDTEXT";
//             info = 0;
//         }
//     }


    // add tracks to list

    qDeleteAll( tags );
    tags.clear();

    TagData *newTags = new TagData();
    newTags->track = 1;
    newTags->trackTotal = 1;
    newTags->disc = 1;
    newTags->discTotal = 1;
    newTags->year = (QDate::currentDate()).year();
    tags += newTags;

    const int trackTotal = cdda_audio_tracks( cdDrive );
    for( int i=0; i<trackTotal; i++ )
    {
        TagData *newTags = new TagData();
        newTags->track = i+1;
        newTags->trackTotal = trackTotal;
        const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,newTags->track)-cdda_track_firstsector(cdDrive,newTags->track));
        newTags->length = (8 * size) / (44100 * 2 * 16);
        tags += newTags;

        QStringList data;
        data.append( "" );
        data.append( QString().sprintf("%02i",newTags->track) );
        data.append( newTags->artist );
        data.append( newTags->composer );
        data.append( newTags->title );
        data.append( QString().sprintf("%i:%02i",newTags->length/60,newTags->length%60) );
        QTreeWidgetItem *item = new QTreeWidgetItem( trackList, data );
        PlayerWidget *playerWidget = new PlayerWidget( mediaObject, newTags->track, item, this );
//         playerWidget->hide();
        connect( playerWidget, SIGNAL(startPlayback(int)), this, SLOT(startPlayback(int)) );
        connect( playerWidget, SIGNAL(stopPlayback()), this, SLOT(stopPlayback()) );
        playerWidgets.append( playerWidget );
        trackList->setItemWidget( item, Column_Player, playerWidget );
        item->setCheckState( 0, Qt::Checked );
    }
    trackList->resizeColumnToContents( Column_Rip );
    trackList->resizeColumnToContents( Column_Track );
    trackList->resizeColumnToContents( Column_Length );

    if( trackList->topLevelItem(0) )
        trackList->topLevelItem(0)->setSelected( true );

    lArtist->setText( tags.at(0)->artist );
    lAlbum->setText( tags.at(0)->album );
    iDisc->setValue( tags.at(0)->disc );
    iDiscTotal->setValue( tags.at(0)->discTotal );
    iYear->setValue( tags.at(0)->year );
    cGenre->setEditText( tags.at(0)->genre );

    artistChanged( lArtist->text() );
    adjustComposerColumn();


    // request cddb data
    requestCddb( true );

    return true;
}

void CDOpener::requestCddb( bool autoRequest )
{
    lOverlayLabel->setText( i18n("Please wait, trying to download CDDB data ...") );

    timeoutTimer.start( autoRequest ? 10000 : 20000 );

    // cddb needs offsets +150 frames (2 seconds * 75 frames per second)
    KCDDB::TrackOffsetList offsets;
    for( int i=1; i<=cdda_tracks(cdDrive); i++ )
    {
        if( !(IS_AUDIO(cdDrive,i-1)) )
            break;

        offsets.append( cdda_track_firstsector(cdDrive,i) + 150 );
    }
    offsets.append( cdda_disc_lastsector(cdDrive) + 150 + 1 );

    cddb->config().reparse();
    cddb->setBlockingMode( false );
    cddb->lookup( offsets );
}

void CDOpener::lookup_cddb_done( KCDDB::Result result )
{
    timeoutTimer.stop();

    if( result != KCDDB::Success && result != KCDDB::MultipleRecordFound )
    {
        // TODO error message if request was initiated by the user
        // Error(i18n("No entry found in CDDB."), i18n("This means no data found in the CDDB database. Please enter the data manually. Maybe try another CDDB server."), Error::ERROR, this);
        fadeOut();
        return;
    }

    cddbFound = true;

    KCDDB::CDInfo info = cddb->lookupResponse().first();
    if( cddb->lookupResponse().count() > 1 || cdTextFound )
    {
        KCDDB::CDInfoList cddb_info = cddb->lookupResponse();
        QStringList list;
        if( cdTextFound )
        {
            // list.append( QString("CD Text: %1, %2").arg(compact_disc->discArtist()).arg(compact_disc->discTitle()) );
        }
        for( int i=0; i<cddb_info.count(); i++ )
        {
            list.append( QString("%1, %2, %3").arg(cddb_info.at(i).get(KCDDB::Artist).toString()).arg(cddb_info.at(i).get(KCDDB::Title).toString()).arg(cddb_info.at(i).get(KCDDB::Genre).toString()) );
        }

        bool ok = false;
        const QString cddbItem = KInputDialog::getItem( i18n("Select CDDB Entry"), i18n("Multiple CDDB entries where found. Please select one:"), list, 0, false, &ok, this );

        if( ok )
        {
            // The user selected and item and pressed OK
            const int offset = cdTextFound ? 1 : 0;
            const int index = list.indexOf( cddbItem );
            if( index - offset < 0 )
            {
                fadeOut();
                return;
            }
            info = cddb_info.at( index - offset );
        }
        else
        {
            // user pressed Cancel
            fadeOut();
            return;
        }
    }

    for( int i=1; i<=cdda_audio_tracks(cdDrive); i++ )
    {
        tags.at(i)->artist = info.track(i-1).get(KCDDB::Artist).toString();
        tags.at(i)->title = info.track(i-1).get(KCDDB::Title).toString();
        tags.at(i)->comment = info.track(i-1).get(KCDDB::Comment).toString();

        QTreeWidgetItem *item = trackList->topLevelItem(i-1);
        item->setText( 2, tags.at(i)->artist );
        item->setText( 4, tags.at(i)->title );
    }

    tags.at(0)->album = info.get(KCDDB::Title).toString();
    tags.at(0)->artist = info.get(KCDDB::Artist).toString();
    tags.at(0)->year = info.get(KCDDB::Year).toInt();
    tags.at(0)->genre = info.get(KCDDB::Genre).toString();

    // TODO resize colums up to a certain width

    lArtist->setText( tags.at(0)->artist );
    lAlbum->setText( tags.at(0)->album );
    iDisc->setValue( tags.at(0)->disc );
    iDiscTotal->setValue( tags.at(0)->discTotal );
    iYear->setValue( tags.at(0)->year );
    cGenre->setEditText( tags.at(0)->genre );

    artistChanged( lArtist->text() );

    fadeOut();
}

void CDOpener::timeout()
{
    fadeOut();
}

void CDOpener::trackUpPressed()
{
    QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.first() - 2 );

    if( !item )
        return;

    disconnect( trackList, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.at(i)-1 );
        if( item )
            item->setSelected( false );
    }

    item->setSelected( true );
    trackList->scrollToItem( item );

    connect( trackList, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );

    trackChanged();
}

void CDOpener::trackDownPressed()
{
    QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.last() );

    if( !item )
        return;

    disconnect( trackList, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.at(i)-1 );
        if( item )
            item->setSelected( false );
    }

    item->setSelected( true );
    trackList->scrollToItem( item );

    connect( trackList, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );

    trackChanged();
}

void CDOpener::trackChanged()
{
    // NOTE if no track is selected soundkonverter could use the current item as default item (like qlistview does)

    // rebuild the list of the selected tracks
    selectedTracks.clear();
    QTreeWidgetItem *item;
    for( int i=0; i<trackList->topLevelItemCount(); i++ )
    {
        item = trackList->topLevelItem( i );
        if( item->isSelected() )
        {
            selectedTracks.append( i+1 );
        }
    }

    // insert the new values
    if( selectedTracks.count() < 1 )
    {
        pTrackUp->setEnabled( false );
        pTrackDown->setEnabled( false );

        lTrackTitle->setEnabled( false );
        lTrackTitle->setText( "" );
        pTrackTitleEdit->hide();
        lTrackArtist->setEnabled( false );
        lTrackArtist->setText( "" );
        pTrackArtistEdit->hide();
        lTrackComposer->setEnabled( false );
        lTrackComposer->setText( "" );
        pTrackComposerEdit->hide();
        tTrackComment->setEnabled( false );
        tTrackComment->setReadOnly( true );
        tTrackComment->setText( "" );
        pTrackCommentEdit->hide();

        pTrackUp->setEnabled( false );
        pTrackDown->setEnabled( false );

        return;
    }
    else if( selectedTracks.count() > 1 )
    {
        if( selectedTracks.first() > 1 )
            pTrackUp->setEnabled( true );
        else
            pTrackUp->setEnabled( false );

        if( selectedTracks.last() < trackList->topLevelItemCount() )
            pTrackDown->setEnabled( true );
        else
            pTrackDown->setEnabled( false );

        QString trackListString = "";
        if( selectedTracks.count() == trackList->topLevelItemCount() )
        {
            trackListString = i18n("All tracks");
        }
        else
        {
            trackListString = i18n("Tracks") + QString().sprintf( " %02i", selectedTracks.at(0) );
            for( int i=1; i<selectedTracks.count(); i++ )
            {
                trackListString += QString().sprintf( ", %02i", selectedTracks.at(i) );
            }
        }
        tagGroupBox->setTitle( trackListString );

        const QString title = tags.at(selectedTracks.at(0))->title;
        bool equalTitles = true;
        const QString artist = tags.at(selectedTracks.at(0))->artist;
        bool equalArtists = true;
        const QString composer = tags.at(selectedTracks.at(0))->composer;
        bool equalComposers = true;
        const QString comment = tags.at(selectedTracks.at(0))->comment;
        bool equalComments = true;
        for( int i=1; i<selectedTracks.count(); i++ )
        {
            if( title != tags.at(selectedTracks.at(i))->title )
                equalTitles = false;
            if( artist != tags.at(selectedTracks.at(i))->artist )
                equalArtists = false;
            if( composer != tags.at(selectedTracks.at(i))->composer )
                equalComposers = false;
            if( comment != tags.at(selectedTracks.at(i))->comment )
                equalComments = false;
        }

        if( equalTitles )
        {
            lTrackTitle->setEnabled( true );
            lTrackTitle->setText( title );
            pTrackTitleEdit->hide();
        }
        else
        {
            lTrackTitle->setEnabled( false );
            lTrackTitle->setText( "" );
            pTrackTitleEdit->show();
        }

        if( equalArtists )
        {
            lTrackArtist->setEnabled( true );
            lTrackArtist->setText( artist );
            pTrackArtistEdit->hide();
        }
        else
        {
            lTrackArtist->setEnabled( false );
            lTrackArtist->setText( "" );
            pTrackArtistEdit->show();
        }

        if( equalComposers )
        {
            lTrackComposer->setEnabled( true );
            lTrackComposer->setText( composer );
            pTrackComposerEdit->hide();
        }
        else
        {
            lTrackComposer->setEnabled( false );
            lTrackComposer->setText( "" );
            pTrackComposerEdit->show();
        }

        if( equalComments )
        {
            tTrackComment->setEnabled( true );
            tTrackComment->setReadOnly( false );
            tTrackComment->setText( comment );
            pTrackCommentEdit->hide();
        }
        else
        {
            tTrackComment->setEnabled( false );
            tTrackComment->setReadOnly( true );
            tTrackComment->setText( "" );
            pTrackCommentEdit->show();
        }
    }
    else
    {
        if( selectedTracks.first() > 1 )
            pTrackUp->setEnabled( true );
        else
            pTrackUp->setEnabled( false );

        if( selectedTracks.last() < trackList->topLevelItemCount() )
            pTrackDown->setEnabled( true );
        else
            pTrackDown->setEnabled( false );

        tagGroupBox->setTitle( i18n("Track") + QString().sprintf(" %02i",selectedTracks.at(0)) );

        lTrackTitle->setEnabled( true );
        lTrackTitle->setText( tags.at(selectedTracks.at(0))->title );
        pTrackTitleEdit->hide();

        lTrackArtist->setEnabled( true );
        lTrackArtist->setText( tags.at(selectedTracks.at(0))->artist );
        pTrackArtistEdit->hide();

        lTrackComposer->setEnabled( true );
        lTrackComposer->setText( tags.at(selectedTracks.at(0))->composer );
        pTrackComposerEdit->hide();

        tTrackComment->setEnabled( true );
        tTrackComment->setReadOnly( false );
        tTrackComment->setText( tags.at(selectedTracks.at(0))->comment );
        pTrackCommentEdit->hide();
    }
}

void CDOpener::artistChanged( const QString& text )
{
    for( int i=1; i<tags.count(); i++ )
    {
        if( tags.at(i)->artist == lastAlbumArtist )
        {
            tags.at(i)->artist = text;
            if( QTreeWidgetItem *item = trackList->topLevelItem( i-1 ) )
            {
                item->setText( Column_Artist, text );
            }
        }
    }

    tags.at(0)->artist = text;

    lastAlbumArtist = text;

    adjustArtistColumn();
    trackChanged();
}

void CDOpener::adjustArtistColumn()
{
    QString albumArtist = tags.at(0)->artist;

    for( int i=1; i<tags.count(); i++ )
    {
        if( tags.at(i)->artist != albumArtist )
        {
            trackList->setColumnHidden( Column_Artist, false );
            return;
        }
    }

    trackList->setColumnHidden( Column_Artist, true );
}

void CDOpener::adjustComposerColumn()
{
    for( int i=1; i<tags.count(); i++ )
    {
        if( !tags.at(i)->composer.isEmpty() )
        {
            trackList->setColumnHidden( Column_Composer, false );
            return;
        }
    }

    trackList->setColumnHidden( Column_Composer, true );
}

void CDOpener::trackTitleChanged( const QString& text )
{
    if( !lTrackTitle->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.at(i)-1 );
        if( item )
            item->setText( Column_Title, text );
        tags.at(selectedTracks.at(i))->title = text;
    }
}

void CDOpener::trackArtistChanged( const QString& text )
{
    if( !lTrackArtist->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.at(i)-1 );
        if( item )
            item->setText( Column_Artist, text );
        tags.at(selectedTracks.at(i))->artist = text;
    }

    adjustArtistColumn();
}

void CDOpener::trackComposerChanged( const QString& text )
{
    if( !lTrackComposer->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.at(i)-1 );
        if( item )
            item->setText( Column_Composer, text );
        tags.at(selectedTracks.at(i))->composer = text;
    }

    adjustComposerColumn();
}

void CDOpener::trackCommentChanged()
{
    QString text = tTrackComment->toPlainText();

    if( !tTrackComment->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        tags.at(selectedTracks.at(i))->comment = text;
    }
}

void CDOpener::editTrackTitleClicked()
{
    lTrackTitle->setEnabled( true );
    lTrackTitle->setFocus();
    pTrackTitleEdit->hide();
    trackTitleChanged( lTrackTitle->text() );
}

void CDOpener::editTrackArtistClicked()
{
    lTrackArtist->setEnabled( true );
    lTrackArtist->setFocus();
    pTrackArtistEdit->hide();
    trackArtistChanged( lTrackArtist->text() );
}

void CDOpener::editTrackComposerClicked()
{
    lTrackComposer->setEnabled( true );
    lTrackComposer->setFocus();
    pTrackComposerEdit->hide();
    trackComposerChanged( lTrackComposer->text() );
}

void CDOpener::editTrackCommentClicked()
{
    tTrackComment->setEnabled( true );
    tTrackComment->setReadOnly( false );
    tTrackComment->setFocus();
    pTrackCommentEdit->hide();
    trackCommentChanged();
}

void CDOpener::fadeIn()
{
    fadeTimer.start( 50 );
    fadeMode = 1;
    cdOpenerOverlayWidget->show();
}

void CDOpener::fadeOut()
{
    fadeTimer.start( 50 );
    fadeMode = 2;
}

void CDOpener::fadeAnim()
{
    if( fadeMode == 1 )
    {
        fadeAlpha += 255.0f/50.0f*8.0f;
    }
    else if( fadeMode == 2 )
    {
        fadeAlpha -= 255.0f/50.0f*8.0f;
    }

    if( fadeAlpha <= 0.0f )
    {
        fadeAlpha = 0.0f;
        fadeMode = 0;
        cdOpenerOverlayWidget->hide();
    }
    else if( fadeAlpha >= 255.0f )
    {
        fadeAlpha = 255.0f;
        fadeMode = 0;
    }
    else
    {
        fadeTimer.start( 50 );
    }

    QPalette newPalette = cdOpenerOverlayWidget->palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192.0f/255.0f*fadeAlpha ) );
    cdOpenerOverlayWidget->setPalette( newPalette );

    newPalette = lOverlayLabel->palette();
    newPalette.setBrush( QPalette::WindowText, brushSetAlpha( newPalette.windowText(), fadeAlpha ) );
    lOverlayLabel->setPalette( newPalette );
}

void CDOpener::proceedClicked()
{
    int trackCount = 0;

    for( int i=0; i<trackList->topLevelItemCount(); i++ )
    {
        if( trackList->topLevelItem(i)->checkState(0) == Qt::Checked )
            trackCount++;
    }

    if( trackCount == 0 )
    {
        KMessageBox::error( this, i18n("Please select at least one track in order to proceed.") );
        return;
    }

    if( options->currentConversionOptions() && options->currentConversionOptions()->outputDirectoryMode == OutputDirectory::Source )
    {
        options->setOutputDirectoryMode( (int)OutputDirectory::MetaData );
    }

    cdOpenerWidget->hide();
    pSaveCue->hide();
    pCDDB->hide();
    cEntireCd->hide();
    options->show();
    page = ConversionOptionsPage;
    QFont font;
    font.setBold( false );
    lSelector->setFont( font );
    font.setBold( true );
    lOptions->setFont( font );
    pProceed->hide();
    pAdd->show();
}

void CDOpener::addClicked()
{
    ConversionOptions *conversionOptions = options->currentConversionOptions();
    if( conversionOptions )
    {
        QList<int> tracks;
        QList<TagData*> tagList;
        const int trackCount = cdda_audio_tracks( cdDrive );

        if( cEntireCd->isEnabled() && cEntireCd->isChecked() )
        {
            tags.at(0)->title = lAlbum->text();
            tags.at(0)->artist = lArtist->text();
            tags.at(0)->albumArtist = lArtist->text();
            tags.at(0)->album = lAlbum->text();
            tags.at(0)->disc = iDisc->value();
            tags.at(0)->discTotal = iDiscTotal->value();
            tags.at(0)->year = iYear->value();
            tags.at(0)->genre = cGenre->currentText();
            const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,trackCount)-cdda_track_firstsector(cdDrive,1));
            tags.at(0)->length = (8 * size) / (44100 * 2 * 16);

            tagList.append( tags.at(0) );
            tracks.append( 0 );
        }
        else
        {
            for( int i=0; i<trackList->topLevelItemCount(); i++ )
            {
                if( trackList->topLevelItem(i)->checkState(0) == Qt::Checked )
                {
                    tags.at(i+1)->albumArtist = lArtist->text();
                    tags.at(i+1)->album = lAlbum->text();
                    tags.at(i+1)->disc = iDisc->value();
                    tags.at(i+1)->discTotal = iDiscTotal->value();
                    tags.at(i+1)->year = iYear->value();
                    tags.at(i+1)->genre = cGenre->currentText();
                    const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,i+1)-cdda_track_firstsector(cdDrive,i+1));
                    tags.at(i+1)->length = (8 * size) / (44100 * 2 * 16);

                    tagList.append( tags.at(i+1) );
                    tracks.append( i+1 );
                }
            }
        }

        options->accepted();

        emit addTracks( device, tracks, trackCount, tagList, conversionOptions, command );

        accept();
    }
    else
    {
        KMessageBox::error( this, i18n("No conversion options selected.") );
    }
}

void CDOpener::saveCuesheetClicked()
{
    QString filename = KFileDialog::getSaveFileName( QDir::homePath(), "*.cue" );
    if( filename.isEmpty() )
        return;

    QFile cueFile( filename );
    if( cueFile.exists() )
    {
        const int ret = KMessageBox::questionYesNo( this,
                    i18n("A file with this name already exists.\n\nDo you want to overwrite the existing one?"),
                    i18n("File already exists") );
        if( ret == KMessageBox::No )
            return;
    }
    if( !cueFile.open( QIODevice::WriteOnly ) )
        return;

    QString content;

    content.append( "REM COMMENT \"soundKonverter " + QString(SOUNDKONVERTER_VERSION_STRING) + "\"\n" );
    content.append( "TITLE \"" + lAlbum->text() + "\"\n" );
    content.append( "PERFORMER \"" + lArtist->text() + "\"\n" );
    content.append( "FILE \"\" WAVE\n" );

    for( int i=1; i<tags.count(); i++ )
    {
        content.append( QString().sprintf("  TRACK %02i AUDIO\n",tags.at(i)->track ) );
        content.append( "    TITLE \"" + tags.at(i)->title + "\"\n" );
        content.append( "    PERFORMER \"" + tags.at(i)->artist + "\"\n" );
        content.append( "    SONGWRITER \"" + tags.at(i)->composer + "\"\n" );
        const long size = CD_FRAMESIZE_RAW * cdda_track_firstsector(cdDrive,i);
        const long length = (8 * size) / (44100 * 2 * 16);
        const long frames = (8 * size) / (588 * 2 * 16);
        content.append( QString().sprintf("    INDEX 01 %02li:%02li:%02li\n",length/60,length%60,frames%75) );
    }

    QTextStream ts( &cueFile );
    ts << content;

    cueFile.close();
}

// void CDOpener::itemHighlighted( QTreeWidgetItem *item, int column )
// {
//     for( int i=0; i<playerWidgets.count(); i++ )
//     {
//         if( item == playerWidgets.at(i)->treeWidgetItem() )
//             playerWidgets[i]->show();
//         else if( !playerWidgets.at(i)->isPlaying() )
//             playerWidgets[i]->hide();
//     }
// }

void CDOpener::startPlayback( int track )
{
    for( int i=0; i<playerWidgets.count(); i++ )
    {
        if( i+1 != track && playerWidgets.at(i)->isPlaying() )
            playerWidgets[i]->trackChanged( track );
    }

    mediaController->setCurrentTitle( track );
    mediaObject->play();
}

void CDOpener::stopPlayback()
{
    mediaObject->stop();
}

void CDOpener::playbackTitleChanged( int title )
{
    for( int i=0; i<playerWidgets.count(); i++ )
    {
        if( ( i+1 != title &&  playerWidgets.at(i)->isPlaying() ) ||
            ( i+1 == title && !playerWidgets.at(i)->isPlaying() )
        )
            playerWidgets[i]->trackChanged( title );
    }
}

void CDOpener::playbackStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
    Q_UNUSED(oldstate)

    if( newstate == Phonon::StoppedState )
    {
        playbackTitleChanged( 0 );
    }
    else if( newstate == Phonon::PlayingState )
    {
        playbackTitleChanged( mediaController->currentTitle() );
    }
}


