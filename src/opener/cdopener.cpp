
#include "cdopener.h"
// #include "../audiocd/cdmanager.h"
#include "../metadata/tagengine.h"
#include "../config.h"
#include "../options.h"
#include "../outputdirectory.h"

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

#include <QLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTreeWidget>
// #include <QList>
#include <QDateTime>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QCheckBox>

#include <solid/device.h>
#include <solid/block.h>
#include <solid/opticaldrive.h>
#include <solid/opticaldisc.h>


CDOpener::CDOpener( Config *_config, const QString& _device, QWidget *parent /*Mode default_mode, const QString& default_text,*/, Qt::WFlags f )
    : KDialog( parent, f ),
    config( _config )
{
    setButtons( 0 );
    
    noCdFound = false;
    
    page = CdOpenPage;

    // let the dialog look nice
    setCaption( i18n("Add CD tracks") );
    setWindowIcon( KIcon("media-optical-audio") );
    
    QWidget *widget = new QWidget( this );
    QGridLayout *mainGrid = new QGridLayout( widget );
    QGridLayout *topGrid = new QGridLayout( 0 );
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
    QHBoxLayout *topBoxLayout = new QHBoxLayout( 0 );
    gridLayout->addLayout( topBoxLayout, 0, 0 );

    // the album cover
    QLabel *lAlbumCover = new QLabel( "", cdOpenerWidget );
    topBoxLayout->addWidget( lAlbumCover );
    lAlbumCover->setPixmap( QPixmap( KStandardDirs::locate("data","soundkonverter/images/nocover.png") ) );
    lAlbumCover->setContentsMargins( 0, 0, 6, 0 );

    // the grid for the artist and album input
    QGridLayout *topGridLayout = new QGridLayout( 0 );
    topBoxLayout->addLayout( topGridLayout );

    // set up the first row at the top
    QLabel *lArtistLabel = new QLabel( i18n("Artist:"), cdOpenerWidget );
    topGridLayout->addWidget( lArtistLabel, 0, 0 );
    cArtist = new KComboBox( true, cdOpenerWidget );
    topGridLayout->addWidget( cArtist, 0, 1 );
    cArtist->setMinimumWidth( 180 );
    cArtist->addItem( i18n("Various Artists") );
    cArtist->setEditText( "" );
    connect( cArtist, SIGNAL(textChanged(const QString&)), this, SLOT(artistChanged(const QString&)) );
    // add a horizontal box layout for the composer
    QHBoxLayout *artistBox = new QHBoxLayout( 0 );
    topGridLayout->addLayout( artistBox, 0, 3 );
    // and fill it up
    QLabel *lComposerLabel = new QLabel( i18n("Composer:"), cdOpenerWidget );
    lComposerLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    topGridLayout->addWidget( lComposerLabel, 0, 2 );
    cComposer = new KComboBox( true, cdOpenerWidget );
    artistBox->addWidget( cComposer );
    cComposer->setMinimumWidth( 180 );
    cComposer->addItem( i18n("Various Composer") );
    cComposer->setEditText( "" );
    //cComposer->setSizePolicy( QSizePolicy::Maximum );
    connect( cComposer, SIGNAL(textChanged(const QString&)), this, SLOT(composerChanged(const QString&)) );
    //artistBox->addStretch();
//     artistBox->addSpacing( 130 );
//     pCDDB = new KPushButton( iconLoader->loadIcon("cdaudio_unmount",KIcon::Small), i18n("Request CDDB"), this, "pCDDB" );
//     topGridLayout->addWidget( pCDDB, 0, 8 );

    // set up the second row at the top
    QLabel *lAlbumLabel = new QLabel( i18n("Album:"), cdOpenerWidget );
    topGridLayout->addWidget( lAlbumLabel, 1, 0 );
    lAlbum = new KLineEdit( cdOpenerWidget );
    topGridLayout->addWidget( lAlbum, 1, 1 );
    // add a horizontal box layout for the disc number
    QHBoxLayout *albumBox = new QHBoxLayout( 0 );
    topGridLayout->addLayout( albumBox, 1, 3 );
    // and fill it up
    QLabel *lDiscLabel = new QLabel( i18n("Disc No.:"), cdOpenerWidget );
    lDiscLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    topGridLayout->addWidget( lDiscLabel, 1, 2 );
    iDisc = new KIntSpinBox( 1, 99, 1, 1, cdOpenerWidget );
    albumBox->addWidget( iDisc );
    albumBox->addStretch();

    // set up the third row at the top
    QLabel *lYearLabel = new QLabel( i18n("Year:"), cdOpenerWidget );
    topGridLayout->addWidget( lYearLabel, 2, 0 );
    // add a horizontal box layout for the year and genre
    QHBoxLayout *yearBox = new QHBoxLayout( 0 );
    topGridLayout->addLayout( yearBox, 2, 1 );
    // and fill it up
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
    topGridLayout->setColumnStretch( 3, 1 );
    

    // generate the list view for the tracks
    trackList = new QTreeWidget( cdOpenerWidget );
    gridLayout->addWidget( trackList, 1, 0 );
    // and fill in the headers
    trackList->setColumnCount( 5 );
    QStringList labels;
    labels.append( i18n("Rip") );
    labels.append( i18n("Track") );
    labels.append( i18n("Artist") );
    labels.append( i18n("Composer") );
    labels.append( i18n("Title") );
    labels.append( i18n("Length") );
    trackList->setHeaderLabels( labels );
    trackList->setSelectionBehavior( QAbstractItemView::SelectRows );
    trackList->setSelectionMode( QAbstractItemView::ExtendedSelection );
    trackList->setSortingEnabled( false );
    trackList->setRootIsDecorated( false );
    connect( trackList, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );
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
    QHBoxLayout *trackTitleBox = new QHBoxLayout( 0 );
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
    QHBoxLayout *trackArtistBox = new QHBoxLayout( 0 );
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
    QHBoxLayout *trackCommentBox = new QHBoxLayout( 0 );
    tagGridLayout->addLayout( trackCommentBox, 2, 2 );
    // and fill it up
    QLabel *lTrackCommentLabel = new QLabel( i18n("Comment:"), tagGroupBox );
    tagGridLayout->addWidget( lTrackCommentLabel, 2, 1 );
    tTrackComment = new KTextEdit( tagGroupBox );
    trackCommentBox->addWidget( tTrackComment );
    tTrackComment->setFixedHeight( 45 );
    connect( tTrackComment, SIGNAL(textChanged()), this, SLOT(trackCommentChanged()) );
    pTrackCommentEdit = new KPushButton( "", tagGroupBox );
    pTrackCommentEdit->setIcon( KIcon("document-edit") );
    pTrackCommentEdit->setFixedSize( lTrackTitle->sizeHint().height(), lTrackTitle->sizeHint().height() );
    pTrackCommentEdit->hide();
    trackCommentBox->addWidget( pTrackCommentEdit );
    connect( pTrackCommentEdit, SIGNAL(clicked()), this, SLOT(editTrackCommentClicked()) );

    
    
    // Cd Opener Overlay Widget

    cdOpenerOverlayWidget = new QWidget( widget );
    mainGrid->addWidget( cdOpenerOverlayWidget, 2, 0 );
    QHBoxLayout *cdOpenerOverlayLayout = new QHBoxLayout();
    cdOpenerOverlayWidget->setLayout( cdOpenerOverlayLayout );
    lOverlayLabel = new QLabel( i18n("Please wait, trying to read audio CD ..."), cdOpenerOverlayWidget );
    cdOpenerOverlayLayout->addWidget( lOverlayLabel );
    lOverlayLabel->setAlignment( Qt::AlignCenter );
    cdOpenerOverlayWidget->setAutoFillBackground( true );
    QPalette newPalette = cdOpenerOverlayWidget->palette();
    newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192.0f ) );
    cdOpenerOverlayWidget->setPalette( newPalette );

    
    // Conversion Options Widget
    
    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), widget );
    mainGrid->addWidget( options, 2, 0 );
    adjustSize();
    int h_margin = ( cdOpenerWidget->sizeHint().width() - options->sizeHint().width() ) / 4;
    int v_margin = ( cdOpenerWidget->sizeHint().height() - options->sizeHint().height() ) / 4;
    options->setContentsMargins( h_margin, v_margin, h_margin, v_margin );
    options->hide();


    // draw a horizontal line
    QFrame *buttonLineFrame = new QFrame( widget );
    buttonLineFrame->setFrameShape( QFrame::HLine );
    buttonLineFrame->setFrameShadow( QFrame::Sunken );
    buttonLineFrame->setFrameShape( QFrame::HLine );
    mainGrid->addWidget( buttonLineFrame, 4, 0 );

    // add a horizontal box layout for the control elements
    QHBoxLayout *controlBox = new QHBoxLayout( 0 );
    mainGrid->addLayout( controlBox, 5, 0 );

    // add the control elements
    pSaveCue = new KPushButton( KIcon("document-save"), i18n("Save cue sheet..."), widget );
    controlBox->addWidget( pSaveCue );
    connect( pSaveCue, SIGNAL(clicked()), this, SLOT(saveCuesheetClicked()) );
    controlBox->addStretch();

    cEntireCd = new QCheckBox( i18n("Rip entire CD to one file"), widget );
    QStringList errorList;
    cEntireCd->setEnabled( config->pluginLoader()->canRipEntireCd(&errorList) );
    if( !cEntireCd->isEnabled() )
    {
        QPalette notificationPalette = cEntireCd->palette();
//         notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(181,96,101) );
        notificationPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor(174,127,130) );
        cEntireCd->setPalette( notificationPalette );
        if( !errorList.isEmpty() )
        {  
            errorList.prepend( i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPossible solutions are listed below.\n") );
        }
        else
        {
            errorList += i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPlease check your distribution's package manager in order to install an additional ripper plugin wich supports ripping to one file.");
        }
        cEntireCd->setToolTip( errorList.join("\n") );
    }
    controlBox->addWidget( cEntireCd );
    controlBox->addSpacing( 20 );

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

    compact_disc = new KCompactDisc();
    if( !compact_disc )
    {
//         kDebug() << "Unable to create KCompactDisc object. Low mem?";
//         error = Error(i18n("Unable to create KCompactDisc object."), i18n("This is an internal error. Check your hardware. If all okay please make bug report."), Error::ERROR, this);
        return;
    }
    connect( compact_disc, SIGNAL(discChanged(unsigned int)), this, SLOT(slot_disc_changed(unsigned int)) );
    connect( compact_disc, SIGNAL(discInformation(KCompactDisc::DiscInfo)), this, SLOT(slot_disc_information(KCompactDisc::DiscInfo)) );
    connect( compact_disc, SIGNAL(discStatusChanged(KCompactDisc::DiscStatus)), this, SLOT(slot_disc_status_changed(KCompactDisc::DiscStatus)) );

    QStringList devices;
    
    if( !_device.isEmpty() )
    {
        devices.append( _device );
    }
    else
    {
        // finds all optical discs (not limited to audio cds)
        QList<Solid::Device> solid_devices = Solid::Device::listFromType(Solid::DeviceInterface::StorageVolume, QString());
        for( int i=0; i<solid_devices.count(); i++ )
        {
            if( solid_devices.value(i).is<Solid::OpticalDisc>() && solid_devices.value(i).is<Solid::Block>() )
            {
                Solid::OpticalDisc *solid_disc = solid_devices.value(i).as<Solid::OpticalDisc>();
                Solid::Block *solid_block = solid_devices.value(i).as<Solid::Block>();
//                 KMessageBox::information( 0, QString("found: %1").arg(solid_block->device()), "cool" );
                devices.append( solid_block->device() );
            }
        }
    }
    
    cddb = new KCDDB::Client();
    if( !cddb )
    {
//         kDebug() << "Unable to create KCDDB object. Low mem?";
//         error = Error(i18n("Unable to create KCDDB object."), i18n("This is an internal error. Check your hardware. If all okay please make bug report."), Error::ERROR, this);
        return;
    }
    connect( cddb, SIGNAL(finished(KCDDB::Result)), this, SLOT(lookup_cddb_done(KCDDB::Result)) );

    cdTextFound = false;
    cddbFound = false;
    
    QString device;
    if( devices.count() > 1 )
    {
        bool ok = false;
        QString res = KInputDialog::getItem( i18n("Select CD device"), i18n("Multiple CD devices where found. Please select one:"), devices, 0, false, &ok, this );

        if( ok )
        {
            device = res;
        }
    }
    else if( devices.count() > 0 )
    {
        device = devices.at(0);
    }
    else
    {
//         KMessageBox::error( this, i18n("No CD device found") );
//         reject();
        noCdFound = true;
        return;
    }
    
    if( !device.isEmpty() ) compact_disc->setDevice( device, 50, true, "cdin" );
    
    #if KDE_IS_VERSION(4,4,0)
    #else
//         KMessageBox::information( this, i18n("You are using a KDE version older than KDE 4.4.0. The library libkcompactdisc included in you release contains an error causing soundKonverter to crash. In order to rip audio CDs you need to add the file to the conversion list, save the list via the file menu, restart soundKonverter and load the saved file list again."), i18n("Known bug"), "libkcompactdisc_delete_bug" );
        KMessageBox::information( this, i18n("You are using a KDE version older than KDE 4.4.0. The library libkcompactdisc included in you release contains an error causing which prevents the soundKonverter CD dialog from working after it has been opened once. In order to rip a second cd you need to restart soundKonverter."), i18n("Known bug"), "libkcompactdisc_delete_bug" );
    #endif
}

CDOpener::~CDOpener()
{
    delete cddb;
    #if KDE_IS_VERSION(4,4,0)
    delete compact_disc;
    #endif
}

void CDOpener::slot_disc_changed( unsigned int tracks )
{
    lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_changed: "+QString::number(tracks));
    
    if( tracks > 0 && !compact_disc->isNoDisc() && compact_disc->discId() != 0 )
    {
        TagData *newTags = new TagData();
        newTags->artist = i18n("Unknown");
        newTags->album = i18n("Unknown");
        newTags->disc = 1;
        newTags->year = (QDate::currentDate()).year();
        newTags->genre = i18n("Unknown");
        tags += newTags;
        
        for( int i=0; i<tracks; i++ )
        {
            TagData *newTags = new TagData();
            newTags->track = i+1;
            newTags->artist = i18n("Unknown");
            newTags->title = i18n("Unknown");
            newTags->length = compact_disc->trackLength(newTags->track);
            tags += newTags;
            
            QStringList data;
            data.append( "" );
            data.append( QString().sprintf("%02i",newTags->track) );
            data.append( newTags->artist );
            data.append( newTags->composer );
            data.append( newTags->title );
            data.append( QString().sprintf("%i:%02i",newTags->length/60,newTags->length%60) );
            QTreeWidgetItem *item = new QTreeWidgetItem( trackList, data );
            item->setCheckState( 0, Qt::Checked );
        }
        trackList->resizeColumnToContents( 0 );
        trackList->resizeColumnToContents( 1 );
        trackList->resizeColumnToContents( 5 );
        
        if( trackList->topLevelItem(0) ) trackList->topLevelItem(0)->setSelected( true );

        cArtist->setEditText( tags.at(0)->artist );
        cComposer->setEditText( tags.at(0)->composer );
        lAlbum->setText( tags.at(0)->album );
        iDisc->setValue( tags.at(0)->disc );
        iYear->setValue( tags.at(0)->year );
        cGenre->setEditText( tags.at(0)->genre );
        
        artistChanged( cArtist->currentText() );
        composerChanged( cComposer->currentText() );

        cddb->config().reparse();
        cddb->setBlockingMode(false);
        cddb->lookup(compact_disc->discSignature());
    }
    else
    {
//         KMessageBox::error( this, i18n("No audio CD found") );
//         reject();
    }
}

void CDOpener::slot_disc_information( KCompactDisc::DiscInfo info )
{
    switch (info)
    {
        case KCompactDisc::Cdtext : lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_information: Cdtext"); break;
        case KCompactDisc::Cddb : lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_information: Cddb"); break;
        case KCompactDisc::PhononMetadata : lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_information: PhononMetadata"); break;
        default: lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_information: Nothing"); break;
    }

    if( info == KCompactDisc::Cdtext )
    {
        cdTextFound = true;
        
        if( cddbFound )
        {
            int answer = KMessageBox::questionYesNo( this, i18n("The cd contains title information. Do you want to use them instead of the CDDB data?\n\nFound: %1 - %2").arg(compact_disc->discArtist()).arg(compact_disc->discTitle()), i18n("CD text found") );
            if( answer != KMessageBox::Yes )
            {
                return;
            }
        }
        
        QString artist = "";
        bool various_artists = false;
        QString composer = "";
        bool various_composer = false;

        for( int i=1; i<=compact_disc->tracks(); i++ )
        {
            tags[i]->artist = compact_disc->trackArtist(i);
            tags[i]->title = compact_disc->trackTitle(i);
            
            if( artist == "" ) artist = tags.at(i)->artist;
            else if( artist != tags.at(i)->artist ) various_artists = true;
            
            if( composer == "" ) composer = tags.at(i)->composer;
            else if( composer != tags.at(i)->composer ) various_composer = true;

            QTreeWidgetItem *item = trackList->topLevelItem(i-1);
            item->setText( 2, tags.at(i)->artist );
            item->setText( 4, tags.at(i)->title );
        }
        
        if( various_artists ) tags[0]->artist = i18n("Various Artists");
        else tags[0]->artist = artist;

        if( various_composer ) tags[0]->composer = i18n("Various Composer");
        else tags[0]->composer = composer;
        
        tags[0]->album = compact_disc->discTitle();

        cArtist->setEditText( tags.at(0)->artist );
        cComposer->setEditText( tags.at(0)->composer );
        lAlbum->setText( tags.at(0)->album );
        iDisc->setValue( tags.at(0)->disc );
        iYear->setValue( tags.at(0)->year );
        cGenre->setEditText( tags.at(0)->genre );
        
        artistChanged( cArtist->currentText() );
        composerChanged( cComposer->currentText() );
    }
}

void CDOpener::slot_disc_status_changed( KCompactDisc::DiscStatus status )
{
    lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"slot_disc_status_changed: "+QString::number(status));

    if( status == KCompactDisc::Ejected || status == KCompactDisc::NoDisc || status == KCompactDisc::NotReady || status == KCompactDisc::Error )
    {
        trackList->clear();
        tags.clear();
        cArtist->setEditText("");
        cComposer->setEditText("");
        lAlbum->setText("");
        iDisc->setValue( 1 );
        iYear->setValue( (QDate::currentDate()).year() );
        cGenre->setEditText("");
        cdTextFound = false;
        cddbFound = false;
        fadeIn();
    }
    
    compact_disc->stop();
}
    
void CDOpener::lookup_cddb_done( KCDDB::Result result )
{
    lOverlayLabel->setText(lOverlayLabel->text()+"\n"+"lookup_cddb_done: "+QString::number(cddb->lookupResponse().count()));

    if( result != KCDDB::Success && result != KCDDB::MultipleRecordFound )
    {
    //     error = Error(i18n("No entry found in CDDB."), i18n("This means no data found in the CDDB database. Please enter the data manually. Maybe try another CDDB server."), Error::ERROR, this);
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
            list.append( QString("CD Text: %1, %2").arg(compact_disc->discArtist()).arg(compact_disc->discTitle()) );
        }
        for( int i=0; i<cddb_info.count(); i++ )
        {
            list.append( QString("%1, %2, %3").arg(cddb_info.at(i).get(KCDDB::Artist).toString()).arg(cddb_info.at(i).get(KCDDB::Title).toString()).arg(cddb_info.at(i).get(KCDDB::Genre).toString()) );
        }

        bool ok = false;
        QString res = KInputDialog::getItem( i18n("Select CDDB Entry"), i18n("Multiple CDDB entrys where found. Please select one:"), list, 0, false, &ok, this );

        if( ok )
        {
            // The user selected and item and pressed OK
            int index = list.indexOf( res );
            if( index <= 0 )
            {
                fadeOut();
                return;
            }
            int offset = cdTextFound ? 1 : 0;
            info = cddb_info.at(index-offset);
        }
        else 
        {
            // user pressed Cancel
            fadeOut();
            return;
        }
    }

    QString artist = "";
    bool various_artists = false;
    QString composer = "";
    bool various_composer = false;

    for( int i=1; i<=compact_disc->tracks(); i++ )
    {
        tags[i]->artist = info.track(i-1).get(KCDDB::Artist).toString();
        tags[i]->title = info.track(i-1).get(KCDDB::Title).toString();
        tags[i]->comment = info.track(i-1).get(KCDDB::Comment).toString();
        
        if( artist == "" ) artist = tags.at(i)->artist;
        else if( artist != tags.at(i)->artist ) various_artists = true;
        
        if( composer == "" ) composer = tags.at(i)->composer;
        else if( composer != tags.at(i)->composer ) various_composer = true;

        QTreeWidgetItem *item = trackList->topLevelItem(i-1);
        item->setText( 2, tags.at(i)->artist );
        item->setText( 4, tags.at(i)->title );
    }
    
    if( various_artists ) tags[0]->artist = i18n("Various Artists");
    else tags[0]->artist = artist;

    if( various_composer ) tags[0]->composer = i18n("Various Composer");
    else tags[0]->composer = composer;
    
    tags[0]->album = info.get(KCDDB::Title).toString();
    tags[0]->year = info.get(KCDDB::Year).toInt();
    tags[0]->genre = info.get(KCDDB::Genre).toString();

    cArtist->setEditText( tags.at(0)->artist );
    cComposer->setEditText( tags.at(0)->composer );
    lAlbum->setText( tags.at(0)->album );
    iDisc->setValue( tags.at(0)->disc );
    iYear->setValue( tags.at(0)->year );
    cGenre->setEditText( tags.at(0)->genre );
    
    artistChanged( cArtist->currentText() );
    composerChanged( cComposer->currentText() );

    fadeOut();
}

int CDOpener::columnByName( const QString& name )
{
    QTreeWidgetItem *header = trackList->headerItem();

    for( int i=0; i<trackList->columnCount(); ++i ) {
        if( header->text(i) == name ) return i;
    }
    return -1;
}

void CDOpener::trackUpPressed()
{
    QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.first() - 2 );

    if( !item ) return;

    disconnect( trackList, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedItems.count(); i++ )
    {
        selectedItems.at(i)->setSelected( false );
    }

    item->setSelected( true );
    trackList->scrollToItem( item );

    connect( trackList, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );

    trackChanged();
}

void CDOpener::trackDownPressed()
{
    QTreeWidgetItem *item = trackList->topLevelItem( selectedTracks.last() );

    if( !item ) return;

    disconnect( trackList, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedItems.count(); i++ )
    {
        selectedItems.at(i)->setSelected( false );
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
    selectedItems.clear();
    QTreeWidgetItem *item;
    for( int i=0; i<trackList->topLevelItemCount(); i++ )
    {
        item = trackList->topLevelItem( i );
        if( item->isSelected() )
        {
            selectedTracks.append( i+1 );
            selectedItems.append( item );
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
        if( selectedTracks.first() > 1 ) pTrackUp->setEnabled( true );
        else pTrackUp->setEnabled( false );

        if( selectedTracks.last() < trackList->topLevelItemCount() ) pTrackDown->setEnabled( true );
        else pTrackDown->setEnabled( false );

        QString trackListString = "";
        if( selectedTracks.count() == trackList->topLevelItemCount() )
        {
            trackListString = i18n("All tracks");
        }
        else
        {
            trackListString = i18n("Tracks") + QString().sprintf( " %02i", selectedTracks.at(0) );
            for( int i = 1; i < selectedTracks.count(); i++ )
            {
                trackListString += QString().sprintf( ", %02i", selectedTracks.at(i) );
            }
        }
        tagGroupBox->setTitle( trackListString );

        QString title = tags.at(selectedTracks.at(0))->title;
        bool equalTitles = true;
        QString artist = tags.at(selectedTracks.at(0))->artist;
        bool equalArtists = true;
        QString composer = tags.at(selectedTracks.at(0))->composer;
        bool equalComposers = true;
        QString comment = tags.at(selectedTracks.at(0))->comment;
        bool equalComments = true;
        for( int i=1; i<selectedTracks.count(); i++ )
        {
            if( title != tags.at(selectedTracks.at(i))->title ) equalTitles = false;
            if( artist != tags.at(selectedTracks.at(i))->artist ) equalArtists = false;
            if( composer != tags.at(selectedTracks.at(i))->composer ) equalComposers = false;
            if( comment != tags.at(selectedTracks.at(i))->comment ) equalComments = false;
        }

        if( equalTitles ) {
            lTrackTitle->setEnabled( true );
            lTrackTitle->setText( title );
            pTrackTitleEdit->hide();
        } else {
            lTrackTitle->setEnabled( false );
            lTrackTitle->setText( "" );
            pTrackTitleEdit->show();
        }

        if( cArtist->currentText() == i18n("Various Artists") && equalArtists ) {
            lTrackArtist->setEnabled( true );
            lTrackArtist->setText( artist );
            pTrackArtistEdit->hide();
        } else if( cArtist->currentText() == i18n("Various Artists") ) {
            lTrackArtist->setEnabled( false );
            lTrackArtist->setText( "" );
            pTrackArtistEdit->show();
        } else {
            lTrackArtist->setEnabled( false );
            lTrackArtist->setText( cArtist->currentText() );
            pTrackArtistEdit->hide();
        }

        if( cComposer->currentText() == i18n("Various Composer") && equalComposers ) {
            lTrackComposer->setEnabled( true );
            lTrackComposer->setText( composer );
            pTrackComposerEdit->hide();
        } else if( cComposer->currentText() == i18n("Various Composer") ) {
            lTrackComposer->setEnabled( false );
            lTrackComposer->setText( "" );
            pTrackComposerEdit->show();
        } else {
            lTrackComposer->setEnabled( false );
            lTrackComposer->setText( cComposer->currentText() );
            pTrackComposerEdit->hide();
        }

        if( equalComments ) {
            tTrackComment->setEnabled( true );
            tTrackComment->setReadOnly( false );
            tTrackComment->setText( comment );
            pTrackCommentEdit->hide();
        } else {
            tTrackComment->setEnabled( false );
            tTrackComment->setReadOnly( true );
            tTrackComment->setText( "" );
            pTrackCommentEdit->show();
        }
    }
    else
    {
        if( selectedTracks.first() > 1 ) pTrackUp->setEnabled( true );
        else pTrackUp->setEnabled( false );

        if( selectedTracks.last() < trackList->topLevelItemCount() ) pTrackDown->setEnabled( true );
        else pTrackDown->setEnabled( false );

        tagGroupBox->setTitle( i18n("Track") + QString().sprintf(" %02i",selectedTracks.at(0)) );

        lTrackTitle->setEnabled( true );
        lTrackTitle->setText( tags.at(selectedTracks.at(0))->title );
        pTrackTitleEdit->hide();
        
        if( cArtist->currentText() == i18n("Various Artists") ) {
            lTrackArtist->setEnabled( true );
            lTrackArtist->setText( tags.at(selectedTracks.at(0))->artist );
            pTrackArtistEdit->hide();
        } else {
            lTrackArtist->setEnabled( false );
            lTrackArtist->setText( cArtist->currentText() );
            pTrackArtistEdit->hide();
        }
        
        if( cComposer->currentText() == i18n("Various Composer") ) {
            lTrackComposer->setEnabled( true );
            lTrackComposer->setText( tags.at(selectedTracks.at(0))->composer );
            pTrackComposerEdit->hide();
        } else {
            lTrackComposer->setEnabled( false );
            lTrackComposer->setText( cComposer->currentText() );
            pTrackComposerEdit->hide();
        }
        
        tTrackComment->setEnabled( true );
        tTrackComment->setReadOnly( false );
        tTrackComment->setText( tags.at(selectedTracks.at(0))->comment );
        pTrackCommentEdit->hide();
    }
}

void CDOpener::artistChanged( const QString& text )
{
    trackList->setColumnHidden( columnByName( i18n("Artist") ), text != i18n("Various Artists") );
    trackChanged();
}

void CDOpener::composerChanged( const QString& text )
{
    trackList->setColumnHidden( columnByName( i18n("Composer") ), text != i18n("Various Composer") );
    trackChanged();
}

void CDOpener::trackTitleChanged( const QString& text )
{
    if( !lTrackTitle->isEnabled() ) return;

    for( QList<QTreeWidgetItem*>::Iterator it = selectedItems.begin(); it != selectedItems.end(); ++it )
    {
        (*it)->setText( columnByName( i18n("Title") ), text );
    }
    for( QList<int>::Iterator it = selectedTracks.begin(); it != selectedTracks.end(); ++it )
    {
        tags[selectedTracks.at(0)]->title = text;
    }
}

void CDOpener::trackArtistChanged( const QString& text )
{
    if( !lTrackArtist->isEnabled() ) return;

    for( QList<QTreeWidgetItem*>::Iterator it = selectedItems.begin(); it != selectedItems.end(); ++it )
    {
        (*it)->setText( columnByName( i18n("Artist") ), text );
    }
    for( QList<int>::Iterator it = selectedTracks.begin(); it != selectedTracks.end(); ++it )
    {
        tags[selectedTracks.at(0)]->artist = text;
    }

    //trackList->resizeColumnToContents( columnByName( i18n("Artist") ) );
}

void CDOpener::trackComposerChanged( const QString& text )
{
    if( !lTrackComposer->isEnabled() ) return;

    for( QList<QTreeWidgetItem*>::Iterator it = selectedItems.begin(); it != selectedItems.end(); ++it )
    {
        (*it)->setText( columnByName( i18n("Composer") ), text );
    }
    for( QList<int>::Iterator it = selectedTracks.begin(); it != selectedTracks.end(); ++it )
    {
        tags[selectedTracks.at(0)]->composer = text;
    }

    //trackList->resizeColumnToContents( columnByName( i18n("Composer") ) );
}

void CDOpener::trackCommentChanged()
{
    QString text = tTrackComment->toPlainText();

    if( !tTrackComment->isEnabled() ) return;

    for( QList<int>::Iterator it = selectedTracks.begin(); it != selectedTracks.end(); ++it )
    {
        tags[selectedTracks.at(0)]->comment = text;
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
    if( fadeMode == 1 ) fadeAlpha += 255.0f/50.0f*8.0f;
    else if( fadeMode == 2 ) fadeAlpha -= 255.0f/50.0f*8.0f;

    if( fadeAlpha <= 0.0f ) { fadeAlpha = 0.0f; fadeMode = 0; cdOpenerOverlayWidget->hide(); }
    else if( fadeAlpha >= 255.0f ) { fadeAlpha = 255.0f; fadeMode = 0; }
    else { fadeTimer.start( 50 ); }

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
        trackCount++;
    }
    
    if( trackCount == 0 )
    {
        KMessageBox::error( this, i18n("You haven't selected a single so we can't proceed.") );
        return;
    }
    
    if( options->currentConversionOptions()->outputDirectoryMode == OutputDirectory::Source )
    {
        options->setOutputDirectoryMode( (int)OutputDirectory::MetaData );
    }

    cdOpenerWidget->hide();
    pSaveCue->hide();
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
    QList<int> tracks;
    QList<TagData*> tagList;

    if( cEntireCd->isChecked() )
    {
//         cdManager->setDiscTags( cdId, new TagData( cArtist->currentText(), cComposer->currentText(), lAlbum->text(), lAlbum->text(), cGenre->currentText(), "", 1, iDisc->value(), iYear->value(), cdManager->getTimeCount(cdId) ) );

        tracks.append(0);
        
        tags[0]->length = compact_disc->discLength();
        tagList += tags.at(0);
    }
    else
    {
        for( int i=0; i<trackList->topLevelItemCount(); i++ )
        {
            if( trackList->topLevelItem(i)->checkState(0) == Qt::Checked )
            {
                if( cArtist->currentText() != i18n("Various Artists") ) tags[i+1]->artist = cArtist->currentText();
                if( cComposer->currentText() != i18n("Various Composer") ) tags[i+1]->composer = cComposer->currentText();
                tags[i+1]->album = lAlbum->text();
                tags[i+1]->disc = iDisc->value();
                tags[i+1]->year = iYear->value();
                tags[i+1]->genre = cGenre->currentText();
                tags[i+1]->length = compact_disc->trackLength(i+1);

                tracks.append(i+1);
            }
        }
        
        tagList = tags.mid(1);

        emit addTracks( compact_disc->deviceName(), tracks, compact_disc->tracks(), tagList, options->currentConversionOptions() );
    }
    
    accept();
}

// void CDOpener::addAsOneTrackClicked()
// {
//     // TODO save all options (album artist, disc, genre, etc.)
//     cdManager->setDiscTags( cdId,
//             new TagData( cArtist->currentText(), cComposer->currentText(),
//                           lAlbum->text(), /*cArtist->currentText() + " - " + */lAlbum->text(),
//                           cGenre->currentText(), "",
//                           1, iDisc->value(), iYear->value(),
//                           cdManager->getTimeCount(cdId) ) );
// 
//     emit addDisc( cdId );
//     accept();
// }

void CDOpener::saveCuesheetClicked()
{
    QString filename = KFileDialog::getSaveFileName( QDir::homePath(), "*.cue" );
    if( filename.isEmpty() ) return;

    QFile cueFile( filename );
    if( cueFile.exists() ) {
        int ret = KMessageBox::questionYesNo( this,
                    i18n("A file with this name already exists.\n\nDo you want to overwrite the existing one?"),
                    i18n("File already exists") );
        if( ret == KMessageBox::No ) return;
    }
    if( !cueFile.open( QIODevice::WriteOnly ) ) return;

    QString content;

    content.append( "TITLE \"" + lAlbum->text() + "\"\n" );
    content.append( "PERFORMER \"" + cArtist->currentText() + "\"\n" );
    content.append( "FILE \"\" MP3\n" );

    int INDEX = 0;
    bool addFrames = false;
    for( int i=1; i<tags.count(); i++ ) {
        content.append( QString().sprintf("  TRACK %02i AUDIO\n",tags.at(i)->track ) );
        content.append( "    TITLE \"" + tags.at(i)->title + "\"\n" );
        content.append( "    PERFORMER \"" + tags.at(i)->artist + "\"\n" );
        if( addFrames ) {
            content.append( QString().sprintf("    INDEX 01 %02i:%02i:37\n",INDEX/60,INDEX%60) );
            INDEX++;
            addFrames = false;
        }
        else {
            content.append( QString().sprintf("    INDEX 01 %02i:%02i:00\n",INDEX/60,INDEX%60) );
            addFrames = true;
        }

        INDEX += tags.at(i)->length;
    }

    QTextStream ts( &cueFile );
    ts << content;

    cueFile.close();
}



