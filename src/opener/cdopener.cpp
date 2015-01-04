
#include "cdopener.h"

#include "../metadata/tagengine.h"
#include "../config.h"
#include "../options.h"
#include "../outputdirectory.h"
#include "../global.h"

#include <KLocalizedString>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QInputDialog>

#include <QDateTime>
#include <QColor>
#include <QFile>

#include <solid/device.h>
#include <solid/block.h>
#include <solid/opticaldisc.h>

#include <discid/discid.h>

#include <musicbrainz5/Query.h>
#include <musicbrainz5/Medium.h>
#include <musicbrainz5/MediumList.h>
#include <musicbrainz5/ReleaseGroup.h>
#include <musicbrainz5/Track.h>
#include <musicbrainz5/TrackList.h>
#include <musicbrainz5/Recording.h>
#include <musicbrainz5/Disc.h>
#include <musicbrainz5/HTTPFetch.h>
#include <musicbrainz5/Release.h>
#include <musicbrainz5/ArtistCredit.h>
#include <musicbrainz5/NameCredit.h>
#include <musicbrainz5/Artist.h>


PlayerWidget::PlayerWidget(Phonon::MediaObject *mediaObject, int _track, QTreeWidgetItem *_treeWidgetItem, QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    track(_track),
    m_treeWidgetItem(_treeWidgetItem)
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    QHBoxLayout *trackPlayerBox = new QHBoxLayout();
    setLayout( trackPlayerBox );

    pStartPlayback = new QPushButton( QIcon::fromTheme("media-playback-start"), "", this );
    pStartPlayback->setFixedSize( 1.5*fontHeight, 1.5*fontHeight );
    trackPlayerBox->addWidget( pStartPlayback );
    connect( pStartPlayback, SIGNAL(clicked()), this, SLOT(startPlaybackClicked()) );
    pStopPlayback = new QPushButton( QIcon::fromTheme("media-playback-stop"), "", this );
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

CDOpener::CDOpener(Config *_config, const QString& _device, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    noCdFound( false ),
    config( _config ),
    cdDrive( 0 ),
    cdParanoia( 0 ),
    discTags( 0 ),
    cddbFound( false )
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    ui.setupUi(this);

    QPixmap coverPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "soundkonverter/images/nocover.png"));
    ui.coverLabel->setFixedSize(qRound((qreal)ui.coverLabel->size().height() * coverPixmap.width() / coverPixmap.height()), ui.coverLabel->size().height());
    ui.coverLabel->setPixmap(coverPixmap);

    ui.albumArtistLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);
    ui.albumLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);
    ui.discNoLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);

    ui.titleLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);
    ui.artistLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);
    ui.commentLabel->setContentsMargins(0.5*fontHeight, 0, 0, 0);

    ui.yearSpinBox->setValue(QDate::currentDate().year());

    ui.genreComboBox->addItems(config->tagEngine()->genreList);
    ui.genreComboBox->clearEditText();

    ui.commentTextEdit->setFixedHeight(4*fontHeight);

    ui.okButton->hide();

    connect(ui.albumArtistLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(artistChanged(const QString&)));

    connect(ui.tracksTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()));
//     connect(ui.tracksTreeWidget, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(itemHighlighted(QTreeWidgetItem*,int)));

    connect(ui.trackUpButton, SIGNAL(clicked()), this, SLOT(trackUpPressed()));
    connect(ui.trackDownButton, SIGNAL(clicked()), this, SLOT(trackDownPressed()));

    connect(ui.titleLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(trackTitleChanged(const QString&)));
    connect(ui.editTitleButton, SIGNAL(clicked()), this, SLOT(editTrackTitleClicked()));
    connect(ui.artistLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(trackArtistChanged(const QString&)));
    connect(ui.editArtistButton, SIGNAL(clicked()), this, SLOT(editTrackArtistClicked()));
    connect(ui.composerLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(trackComposerChanged(const QString&)));
    connect(ui.editComposerButton, SIGNAL(clicked()), this, SLOT(editTrackComposerClicked()));
    connect(ui.commentTextEdit, SIGNAL(textChanged()), this, SLOT(trackCommentChanged()));
    connect(ui.editCommentButton, SIGNAL(clicked()), this, SLOT(editTrackCommentClicked()));


    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    audioOutput->setVolume(0.5);
    mediaObject = new Phonon::MediaObject(this);
    mediaObject->setTickInterval(500);

    Phonon::createPath(mediaObject, audioOutput);

    mediaController = new Phonon::MediaController(mediaObject);
    mediaController->setAutoplayTitles(false);

    connect(mediaController, SIGNAL(titleChanged(int)),                     this, SLOT(playbackTitleChanged(int)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(playbackStateChanged(Phonon::State,Phonon::State)));



//     // Cd Opener Overlay Widget
//
//     cdOpenerOverlayWidget = new QWidget( widget );
//     mainGrid->addWidget( cdOpenerOverlayWidget, 2, 0 );
//     QHBoxLayout *cdOpenerOverlayLayout = new QHBoxLayout();
//     cdOpenerOverlayWidget->setLayout( cdOpenerOverlayLayout );
// //     lOverlayLabel = new QLabel( i18n("Please wait, trying to read audio CD ..."), cdOpenerOverlayWidget );
//     lOverlayLabel = new QLabel( cdOpenerOverlayWidget );
//     cdOpenerOverlayLayout->addWidget( lOverlayLabel );
//     lOverlayLabel->setAlignment( Qt::AlignCenter );
//     cdOpenerOverlayWidget->setAutoFillBackground( true );
//     QPalette newPalette = cdOpenerOverlayWidget->palette();
//     newPalette.setBrush( QPalette::Window, brushSetAlpha( newPalette.window(), 192 ) );
//     cdOpenerOverlayWidget->setPalette( newPalette );


    ui.options->init(config, i18n("Select your desired output options and click on \"Ok\"."));


    connect( ui.saveCueSheetButton, SIGNAL(clicked()), this, SLOT(saveCuesheetClicked()) );

//     connect( pCDDB, SIGNAL(clicked()), this, SLOT(requestCddb()) );

    QStringList errorList;
    ui.ripEntireCdCheckBox->setEnabled(config->pluginLoader()->canRipEntireCd(&errorList));
    if( !ui.ripEntireCdCheckBox->isEnabled() )
    {
        QPalette notificationPalette = ui.ripEntireCdCheckBox->palette();
        notificationPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(174,127,130)); // QColor(181,96,101)
        ui.ripEntireCdCheckBox->setPalette(notificationPalette);
        if( !errorList.isEmpty() )
        {
            errorList.prepend(i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPossible solutions are listed below.\n"));
        }
        else
        {
            errorList += i18n("Ripping an entire cd to a single file is not supported by the installed backends.\nPlease check your distribution's package manager in order to install an additional ripper plugin which supports ripping to one file.");
        }
        ui.ripEntireCdCheckBox->setToolTip(errorList.join("\n"));
    }

    connect(ui.proceedButton, SIGNAL(clicked()), this, SLOT(proceedClicked()));
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

//     connect( &fadeTimer, SIGNAL(timeout()), this, SLOT(fadeAnim()) );
//     fadeAlpha = 255.0f;
//
//
//     if( !_device.isEmpty() )
//     {
//         device = _device;
//     }
//     else
//     {
//         const QMap<QString,QString> devices = cdDevices();
//         if( devices.count() <= 0 )
//         {
//             noCdFound = true;
//             return;
//         }
//         else if( devices.count() == 1 )
//         {
//             device = devices.keys().at(0);
//         }
//         else
//         {
//             QStringList list;
//             foreach( const QString desc, devices.values() )
//             {
//                 list.append( desc );
//             }
//             bool ok = false;
//             const QString selection = QInputDialog::getItem( this, i18n("Select CD-ROM drive"), i18n("Multiple CD-ROM drives where found. Please select one:"), list, 0, false, &ok );
//
//             if( ok )
//             {
//                 // The user selected an item and pressed OK
//                 device = devices.keys().at(list.indexOf(selection));
//             }
//             else
//             {
//                 noCdFound = true;
//                 return;
//             }
//         }
//     }
//
//     const bool success = openCdDevice( device );
//     if( !success )
//     {
// //         QMessageBox::information(this,"success = false, couldn't open audio device.\nplease report this bug.");
//         noCdFound = true;
//         return;
//     }
//
//
//     mediaSource = new Phonon::MediaSource( Phonon::Cd, device );
//     mediaObject->setCurrentSource( *mediaSource ); // WARNING doesn't work with phonon-xine


//     // Prevent the dialog from beeing too wide because of the directory history
//     if( parent && width() > parent->width() )
//         setInitialSize( QSize(parent->width()-fontHeight,sizeHint().height()) );
//
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "CDOpener" );
//     restoreDialogSize( group );
}

CDOpener::~CDOpener()
{
//     KSharedConfig::Ptr conf = KGlobal::config();
//     KConfigGroup group = conf->group( "CDOpener" );
//     saveDialogSize( group );

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
}

void CDOpener::setProfile( const QString& profile )
{
    ui.options->setProfile( profile );
}

void CDOpener::setFormat( const QString& format )
{
    ui.options->setFormat( format );
}

void CDOpener::setOutputDirectory( const QString& directory )
{
    ui.options->setOutputDirectory( directory );
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

                cdDrive = cdda_identify( device.toLocal8Bit(), CDDA_MESSAGE_PRINTIT, 0 );
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
        cdDrive = cdda_identify( _device.toLocal8Bit(), CDDA_MESSAGE_PRINTIT, 0 );
        if( !cdDrive || cdda_open( cdDrive ) != 0 )
        {
            return false;
        }
    }
    cdParanoia = paranoia_init( cdDrive );

    // add tracks to list

    if( discTags )
        delete discTags;

    qDeleteAll( trackTags );
    trackTags.clear();

    discTags = new TagData();
    discTags->track = 1;
    discTags->trackTotal = 1;
    discTags->disc = 1;
    discTags->discTotal = 1;
    discTags->year = (QDate::currentDate()).year();

    const int trackTotal = cdda_audio_tracks( cdDrive );
    for( int i=0; i<trackTotal; i++ )
    {
        TagData *newTags = new TagData();
        newTags->track = i+1;
        newTags->trackTotal = trackTotal;
        const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,newTags->track)-cdda_track_firstsector(cdDrive,newTags->track));
        newTags->length = (8 * size) / (44100 * 2 * 16);
        trackTags += newTags;

        QStringList data;
        data.append( "" );
        data.append( QString().sprintf("%02i",newTags->track) );
        data.append( newTags->artist );
        data.append( newTags->composer );
        data.append( newTags->title );
        data.append( QString().sprintf("%i:%02i",newTags->length/60,newTags->length%60) );
        QTreeWidgetItem *item = new QTreeWidgetItem( ui.tracksTreeWidget, data );
        PlayerWidget *playerWidget = new PlayerWidget( mediaObject, newTags->track, item, this );
//         playerWidget->hide();
        connect( playerWidget, SIGNAL(startPlayback(int)), this, SLOT(startPlayback(int)) );
        connect( playerWidget, SIGNAL(stopPlayback()), this, SLOT(stopPlayback()) );
        playerWidgets.append( playerWidget );
        ui.tracksTreeWidget->setItemWidget( item, Column_Player, playerWidget );
        item->setCheckState( 0, Qt::Checked );
    }
    ui.tracksTreeWidget->resizeColumnToContents( Column_Rip );
    ui.tracksTreeWidget->resizeColumnToContents( Column_Track );
    ui.tracksTreeWidget->resizeColumnToContents( Column_Length );

    if( ui.tracksTreeWidget->topLevelItem(0) )
        ui.tracksTreeWidget->topLevelItem(0)->setSelected( true );

    ui.albumArtistLineEdit->setText( discTags->artist );
    ui.albumLineEdit->setText( discTags->album );
    ui.discNoSpinBox->setValue( discTags->disc );
    ui.discNoTotalSpinBox->setValue( discTags->discTotal );
    ui.yearSpinBox->setValue( discTags->year );
    ui.genreComboBox->setEditText( discTags->genre );

    artistChanged( ui.albumArtistLineEdit->text() );
    adjustComposerColumn();


    // request cddb data
    requestCddb( true );

    return true;
}

void CDOpener::requestCddb( bool autoRequest )
{
    lOverlayLabel->setText( i18n("Please wait, trying to download CDDB data ...") );

    DiscId *disc = discid_new();

    if( discid_read_sparse(disc, device.toLocal8Bit().constData(), 0) )
    {
        char *discID = discid_get_id(disc);

        MusicBrainz5::CQuery query("soundkonverter/" SOUNDKONVERTER_VERSION_STRING " ( hessijames@gmail.com )");

        try
        {
            MusicBrainz5::CQuery::tParamMap params;
            params["inc"] = "artists";
            // http://musicbrainz.org/ws/2/discid/Pa3lPdWkGajOa5fR3_aDwJ3LoS0-/?inc=artists
            // http://musicbrainz.org/ws/2/discid/7zJyVkBip13wLjk0aSdDcNQrqdM-/?inc=artists
            MusicBrainz5::CMetadata discMetadata = query.Query("discid", discID, "", params);
            if( discMetadata.Disc() && discMetadata.Disc()->ReleaseList() )
            {
                MusicBrainz5::CReleaseList *releaseList = discMetadata.Disc()->ReleaseList();
                std::cout << "Found " << releaseList->NumItems() << " release(s)" << std::endl;

                int releaseNumber = 0;

                if( releaseList->NumItems() > 1 )
                {
                    QStringList list;

                    for( int releaseNumber=0; releaseNumber<releaseList->NumItems(); releaseNumber++ )
                    {
                        MusicBrainz5::CRelease *release = releaseList->Item(releaseNumber);

                        const QString artist = parseNameCredits(release->ArtistCredit()->NameCreditList());
                        const QString title = QString::fromStdString(release->Title());
                        const QString date = QString::fromStdString(release->Date());

                        if( !date.isEmpty() )
                        {
                            list.append( QString("%1 - %2 (%3)").arg(artist).arg(title).arg(date) );
                        }
                        else
                        {
                            list.append( QString("%1 - %2").arg(artist).arg(title) );
                        }
                    }

                    bool ok = false;
                    const QString cddbItem = QInputDialog::getItem( this, i18n("Select CDDB Entry"), i18n("Multiple CDDB entries where found. Please select one:"), list, 0, false, &ok );

                    if( ok )
                    {
                        // The user selected an item and pressed OK
                        releaseNumber = list.indexOf( cddbItem );
                    }
                    else
                    {
                        // user pressed Cancel
                        fadeOut();
                        return;
                    }
                }

                MusicBrainz5::CRelease *release = releaseList->Item(releaseNumber);
                std::cout << "Basic release: " << std::endl << (*release) << std::endl;

                // The releases returned from LookupDiscID don't contain full information
                params["inc"] = "artists recordings discids artist-credits";
                std::string releaseID = release->ID();
                // http://musicbrainz.org/ws/2/release/acc0689e-b777-4628-92ea-4224d7684dbb/?inc=artists+recordings+discids+artist-credits
                // http://musicbrainz.org/ws/2/release/2702fe57-a5b6-4f57-91d1-546b42ab3e06/?inc=artists+recordings+discids+artist-credits
                MusicBrainz5::CMetadata releaseMetadata = query.Query("release", releaseID, "", params);
                if( releaseMetadata.Release() )
                {
                    MusicBrainz5::CRelease *fullRelease = releaseMetadata.Release();

                    discTags->album = QString::fromStdString(fullRelease->Title());
                    discTags->artist = parseNameCredits(fullRelease->ArtistCredit()->NameCreditList());
                    discTags->year = QString::fromStdString(fullRelease->Date()).left(4).toShort();

                    ui.albumArtistLineEdit->setText( discTags->artist );
                    ui.albumLineEdit->setText( discTags->album );
                    ui.yearSpinBox->setValue( discTags->year );

                    artistChanged( ui.albumArtistLineEdit->text() );

                    const QString asin = QString::fromStdString(fullRelease->ASIN());

                    QStringList coverUrls;
                    coverUrls += QString("http://coverartarchive.org/release/%1/front").arg(QString::fromStdString(fullRelease->ID()));
                    coverUrls += amazonCoverUrls(asin);

                    // However, these releases will include information for all media in the release
                    // So we need to filter out the only the media we want.
                    MusicBrainz5::CMediumList mediaList = fullRelease->MediaMatchingDiscID(discID);
                    if( mediaList.NumItems() > 0 )
                    {
                        MusicBrainz5::CMedium *medium = mediaList.Item(0);

                        ui.discNoSpinBox->setValue( discTags->disc );
                        ui.discNoTotalSpinBox->setValue( discTags->discTotal );

                        MusicBrainz5::CTrackList *trackList = medium->TrackList();
                        if( trackList )
                        {
                            for( int trackNumber=0; trackNumber<trackList->NumItems() && trackNumber<trackTags.count(); trackNumber++ )
                            {
                                MusicBrainz5::CTrack *track = trackList->Item(trackNumber);

                                const QString artist = parseNameCredits(track->Recording()->ArtistCredit()->NameCreditList());
                                const QString title = QString::fromStdString(track->Recording()->Title());

                                trackTags.at(trackNumber)->artist = artist;
                                trackTags.at(trackNumber)->title = title;

                                QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem(trackNumber);
                                item->setText( 2, artist );
                                item->setText( 4, title );
                            }
                        }
                    }
                }
            }
        }
        catch( MusicBrainz5::CConnectionError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CConnectionError");

//             std::cout << "LastResult: " << query.LastResult() << std::endl;
//             std::cout << "LastHTTPCode: " << query.LastHTTPCode() << std::endl;
//             std::cout << "LastErrorMessage: " << query.LastErrorMessage() << std::endl;
        }
        catch( MusicBrainz5::CTimeoutError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CTimeoutError");
        }
        catch( MusicBrainz5::CAuthenticationError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CAuthenticationError");
        }
        catch( MusicBrainz5::CFetchError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CFetchError");
        }
        catch( MusicBrainz5::CRequestError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CRequestError");
        }
        catch( MusicBrainz5::CResourceNotFoundError& error )
        {
            QMessageBox::information(this, error.what(), "MusicBrainz5::CResourceNotFoundError");
        }
    }
    else
    {
        discid_get_error_msg(disc);
    }

    discid_free(disc);

    // TODO resize colums up to a certain width

    fadeOut();
}

QString CDOpener::parseNameCredits(const MusicBrainz5::CNameCreditList *names)
{
    QString full;

    for( int i=0; i<names->NumItems(); i++)
    {
        MusicBrainz5::CNameCredit *credit = names->Item(i);
        const QString name = QString::fromStdString(credit->Artist()->Name());
        const QString glue = QString::fromStdString(credit->JoinPhrase());
        full += name + glue;
    }

    return full.trimmed();
}

QStringList CDOpener::amazonCoverUrls(const QString& asin)
{
    if( asin.isEmpty() )
        return QStringList();

    QHash<QString, QStringList> server;

    server.insert("amazon.com",   QStringList("ec1.images-amazon.com") << "01");
    server.insert("amazon.ca",    QStringList("ec1.images-amazon.com") << "01");
    server.insert("amazon.co.uk", QStringList("ec1.images-amazon.com") << "02");
    server.insert("amazon.de",    QStringList("ec2.images-amazon.com") << "03");
    server.insert("amazon.fr",    QStringList("ec1.images-amazon.com") << "08");
    server.insert("amazon.jp",    QStringList("ec1.images-amazon.com") << "09");
    server.insert("amazon.co.jp", QStringList("ec1.images-amazon.com") << "09");

    QStringList sizes;
    // huge size option is only available for items
    // that have a ZOOMing picture on its amazon web page
    // and it doesn't work for all of the domain names
    // sizes += "_SCRM_";       // huge size
    sizes += "LZZZZZZZ";        // large size,   format 1
    // sizes += "_SCLZZZZZZZ_"; // large size,   format 3
    sizes += "MZZZZZZZ";        // default size, format 1
    // sizes += "_SCMZZZZZZZ_"; // medium size,  format 3
    // sizes += "TZZZZZZZ";     // medium size,  format 1
    // sizes += "_SCTZZZZZZZ_"; // small size,   format 3
    // sizes += "THUMBZZZ";     // small size,   format 1

    QStringList urls;

    const QString serverName = server.value("amazon.com").at(0);
    const QString serverId   = server.value("amazon.com").at(1);

    foreach( const QString size, sizes )
    {
        urls += QString("http://%1/images/P/%2.%3.%4.jpg").arg(serverName).arg(asin).arg(serverId).arg(size);
    }

    return urls;
}

void CDOpener::timeout()
{
    fadeOut();
}

void CDOpener::trackUpPressed()
{
    QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.first() - 1 );

    if( !item )
        return;

    disconnect( ui.tracksTreeWidget, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.at(i) );
        if( item )
            item->setSelected( false );
    }

    item->setSelected( true );
    ui.tracksTreeWidget->scrollToItem( item );

    connect( ui.tracksTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );

    trackChanged();
}

void CDOpener::trackDownPressed()
{
    QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.last() + 1 );

    if( !item )
        return;

    disconnect( ui.tracksTreeWidget, SIGNAL(itemSelectionChanged()), 0, 0 ); // avoid backfireing

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.at(i) );
        if( item )
            item->setSelected( false );
    }

    item->setSelected( true );
    ui.tracksTreeWidget->scrollToItem( item );

    connect( ui.tracksTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(trackChanged()) );

    trackChanged();
}

void CDOpener::trackChanged()
{
    // NOTE if no track is selected soundkonverter could use the current item as default item (like qlistview does)

    // rebuild the list of the selected tracks
    selectedTracks.clear();
    QTreeWidgetItem *item;
    for( int i=0; i<ui.tracksTreeWidget->topLevelItemCount(); i++ )
    {
        item = ui.tracksTreeWidget->topLevelItem( i );
        if( item->isSelected() )
        {
            selectedTracks.append( i );
        }
    }

    // insert the new values
    if( selectedTracks.count() < 1 )
    {
        ui.trackUpButton->setEnabled( false );
        ui.trackDownButton->setEnabled( false );

        ui.titleLineEdit->setEnabled( false );
        ui.titleLineEdit->setText( "" );
        ui.editTitleButton->hide();
        ui.artistLineEdit->setEnabled( false );
        ui.artistLineEdit->setText( "" );
        ui.editArtistButton->hide();
        ui.composerLineEdit->setEnabled( false );
        ui.composerLineEdit->setText( "" );
        ui.editComposerButton->hide();
        ui.commentTextEdit->setEnabled( false );
        ui.commentTextEdit->setReadOnly( true );
        ui.commentTextEdit->setText( "" );
        ui.editCommentButton->hide();

        ui.trackUpButton->setEnabled( false );
        ui.trackDownButton->setEnabled( false );

        return;
    }
    else if( selectedTracks.count() > 1 )
    {
        if( selectedTracks.first() > 0 )
            ui.trackUpButton->setEnabled( true );
        else
            ui.trackUpButton->setEnabled( false );

        if( selectedTracks.last() < ui.tracksTreeWidget->topLevelItemCount() - 1 )
            ui.trackDownButton->setEnabled( true );
        else
            ui.trackDownButton->setEnabled( false );

        QString trackGroupBoxTitle = "";
        if( selectedTracks.count() == ui.tracksTreeWidget->topLevelItemCount() )
        {
            trackGroupBoxTitle = i18n("All tracks");
        }
        else
        {
            trackGroupBoxTitle = i18n("Tracks") + QString().sprintf(" %02i", selectedTracks.at(0) + 1);
            for( int i=1; i<selectedTracks.count(); i++ )
            {
                trackGroupBoxTitle += QString().sprintf(", %02i", selectedTracks.at(i) + 1);
            }
        }
        ui.trackGroupBox->setTitle(trackGroupBoxTitle);

        const QString title = trackTags.at(selectedTracks.at(0))->title;
        bool equalTitles = true;
        const QString artist = trackTags.at(selectedTracks.at(0))->artist;
        bool equalArtists = true;
        const QString composer = trackTags.at(selectedTracks.at(0))->composer;
        bool equalComposers = true;
        const QString comment = trackTags.at(selectedTracks.at(0))->comment;
        bool equalComments = true;
        for( int i=1; i<selectedTracks.count(); i++ )
        {
            if( title != trackTags.at(selectedTracks.at(i))->title )
                equalTitles = false;
            if( artist != trackTags.at(selectedTracks.at(i))->artist )
                equalArtists = false;
            if( composer != trackTags.at(selectedTracks.at(i))->composer )
                equalComposers = false;
            if( comment != trackTags.at(selectedTracks.at(i))->comment )
                equalComments = false;
        }

        if( equalTitles )
        {
            ui.titleLineEdit->setEnabled( true );
            ui.titleLineEdit->setText( title );
            ui.editTitleButton->hide();
        }
        else
        {
            ui.titleLineEdit->setEnabled( false );
            ui.titleLineEdit->setText( "" );
            ui.editTitleButton->show();
        }

        if( equalArtists )
        {
            ui.artistLineEdit->setEnabled( true );
            ui.artistLineEdit->setText( artist );
            ui.editArtistButton->hide();
        }
        else
        {
            ui.artistLineEdit->setEnabled( false );
            ui.artistLineEdit->setText( "" );
            ui.editArtistButton->show();
        }

        if( equalComposers )
        {
            ui.composerLineEdit->setEnabled( true );
            ui.composerLineEdit->setText( composer );
            ui.editComposerButton->hide();
        }
        else
        {
            ui.composerLineEdit->setEnabled( false );
            ui.composerLineEdit->setText( "" );
            ui.editComposerButton->show();
        }

        if( equalComments )
        {
            ui.commentTextEdit->setEnabled( true );
            ui.commentTextEdit->setReadOnly( false );
            ui.commentTextEdit->setText( comment );
            ui.editCommentButton->hide();
        }
        else
        {
            ui.commentTextEdit->setEnabled( false );
            ui.commentTextEdit->setReadOnly( true );
            ui.commentTextEdit->setText( "" );
            ui.editCommentButton->show();
        }
    }
    else
    {
        if( selectedTracks.first() > 0 )
            ui.trackUpButton->setEnabled( true );
        else
            ui.trackUpButton->setEnabled( false );

        if( selectedTracks.last() < ui.tracksTreeWidget->topLevelItemCount() - 1 )
            ui.trackDownButton->setEnabled( true );
        else
            ui.trackDownButton->setEnabled( false );

        ui.trackGroupBox->setTitle( i18n("Track") + QString().sprintf(" %02i",selectedTracks.at(0) + 1) );

        ui.titleLineEdit->setEnabled( true );
        ui.titleLineEdit->setText( trackTags.at(selectedTracks.at(0))->title );
        ui.editTitleButton->hide();

        ui.artistLineEdit->setEnabled( true );
        ui.artistLineEdit->setText( trackTags.at(selectedTracks.at(0))->artist );
        ui.editArtistButton->hide();

        ui.composerLineEdit->setEnabled( true );
        ui.composerLineEdit->setText( trackTags.at(selectedTracks.at(0))->composer );
        ui.editComposerButton->hide();

        ui.commentTextEdit->setEnabled( true );
        ui.commentTextEdit->setReadOnly( false );
        ui.commentTextEdit->setText( trackTags.at(selectedTracks.at(0))->comment );
        ui.editCommentButton->hide();
    }
}

void CDOpener::artistChanged( const QString& text )
{
    for( int i=0; i<trackTags.count(); i++ )
    {
        if( trackTags.at(i)->artist == lastAlbumArtist )
        {
            trackTags.at(i)->artist = text;
            if( QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( i ) )
            {
                item->setText( Column_Artist, text );
            }
        }
    }

    discTags->artist = text;

    lastAlbumArtist = text;

    adjustArtistColumn();
    trackChanged();
}

void CDOpener::adjustArtistColumn()
{
    QString albumArtist = discTags->artist;

    for( int i=0; i<trackTags.count(); i++ )
    {
        if( trackTags.at(i)->artist != albumArtist )
        {
            ui.tracksTreeWidget->setColumnHidden( Column_Artist, false );
            return;
        }
    }

    ui.tracksTreeWidget->setColumnHidden( Column_Artist, true );
}

void CDOpener::adjustComposerColumn()
{
    for( int i=0; i<trackTags.count(); i++ )
    {
        if( !trackTags.at(i)->composer.isEmpty() )
        {
            ui.tracksTreeWidget->setColumnHidden( Column_Composer, false );
            return;
        }
    }

    ui.tracksTreeWidget->setColumnHidden( Column_Composer, true );
}

void CDOpener::trackTitleChanged( const QString& text )
{
    if( !ui.titleLineEdit->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.at(i) );
        if( item )
            item->setText( Column_Title, text );
        trackTags.at(selectedTracks.at(i))->title = text;
    }
}

void CDOpener::trackArtistChanged( const QString& text )
{
    if( !ui.artistLineEdit->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.at(i) );
        if( item )
            item->setText( Column_Artist, text );
        trackTags.at(selectedTracks.at(i))->artist = text;
    }

    adjustArtistColumn();
}

void CDOpener::trackComposerChanged( const QString& text )
{
    if( !ui.composerLineEdit->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        QTreeWidgetItem *item = ui.tracksTreeWidget->topLevelItem( selectedTracks.at(i) );
        if( item )
            item->setText( Column_Composer, text );
        trackTags.at(selectedTracks.at(i))->composer = text;
    }

    adjustComposerColumn();
}

void CDOpener::trackCommentChanged()
{
    QString text = ui.commentTextEdit->toPlainText();

    if( !ui.commentTextEdit->isEnabled() )
        return;

    for( int i=0; i<selectedTracks.count(); i++ )
    {
        trackTags.at(selectedTracks.at(i))->comment = text;
    }
}

void CDOpener::editTrackTitleClicked()
{
    ui.titleLineEdit->setEnabled( true );
    ui.titleLineEdit->setFocus();
    ui.editTitleButton->hide();
    trackTitleChanged( ui.titleLineEdit->text() );
}

void CDOpener::editTrackArtistClicked()
{
    ui.artistLineEdit->setEnabled( true );
    ui.artistLineEdit->setFocus();
    ui.editArtistButton->hide();
    trackArtistChanged( ui.artistLineEdit->text() );
}

void CDOpener::editTrackComposerClicked()
{
    ui.composerLineEdit->setEnabled( true );
    ui.composerLineEdit->setFocus();
    ui.editComposerButton->hide();
    trackComposerChanged( ui.composerLineEdit->text() );
}

void CDOpener::editTrackCommentClicked()
{
    ui.commentTextEdit->setEnabled( true );
    ui.commentTextEdit->setReadOnly( false );
    ui.commentTextEdit->setFocus();
    ui.editCommentButton->hide();
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

    for( int i=0; i<ui.tracksTreeWidget->topLevelItemCount(); i++ )
    {
        if( ui.tracksTreeWidget->topLevelItem(i)->checkState(0) == Qt::Checked )
            trackCount++;
    }

    if( trackCount == 0 )
    {
        QMessageBox::information(this, "soundKonverter", i18n("Please select at least one track in order to proceed."));
        return;
    }

    if( ui.options->currentConversionOptions() && ui.options->currentConversionOptions()->outputDirectoryMode == OutputDirectory::Source )
    {
        ui.options->setOutputDirectoryMode((int)OutputDirectory::MetaData);
    }

    ui.stackedWidget->setCurrentIndex(1);

    QFont font;
    ui.step1Label->setFont(font);
    font.setBold(true);
    ui.step2Label->setFont(font);

    ui.saveCueSheetButton->hide();
//     pCDDB->hide();
    ui.ripEntireCdCheckBox->hide();
    ui.proceedButton->hide();
    ui.okButton->show();
}

void CDOpener::addClicked()
{
    ConversionOptions *conversionOptions = ui.options->currentConversionOptions();
    if( conversionOptions )
    {
        QList<int> tracks;
        QList<TagData*> tagList;
        const int trackCount = cdda_audio_tracks( cdDrive );

        if( ui.ripEntireCdCheckBox->isEnabled() && ui.ripEntireCdCheckBox->isChecked() )
        {
            discTags->title = ui.albumLineEdit->text();
            discTags->artist = ui.albumArtistLineEdit->text();
            discTags->albumArtist = ui.albumArtistLineEdit->text();
            discTags->album = ui.albumLineEdit->text();
            discTags->disc = ui.discNoSpinBox->value();
            discTags->discTotal = ui.discNoTotalSpinBox->value();
            discTags->year = ui.yearSpinBox->value();
            discTags->genre = ui.genreComboBox->currentText();
            const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,trackCount)-cdda_track_firstsector(cdDrive,1));
            discTags->length = (8 * size) / (44100 * 2 * 16);

            tagList.append( discTags );
            tracks.append( 0 );
        }
        else
        {
            for( int i=0; i<ui.tracksTreeWidget->topLevelItemCount(); i++ )
            {
                if( ui.tracksTreeWidget->topLevelItem(i)->checkState(0) == Qt::Checked )
                {
                    trackTags.at(i)->albumArtist = ui.albumArtistLineEdit->text();
                    trackTags.at(i)->album = ui.albumLineEdit->text();
                    trackTags.at(i)->disc = ui.discNoSpinBox->value();
                    trackTags.at(i)->discTotal = ui.discNoTotalSpinBox->value();
                    trackTags.at(i)->year = ui.yearSpinBox->value();
                    trackTags.at(i)->genre = ui.genreComboBox->currentText();
                    const long size = CD_FRAMESIZE_RAW * (cdda_track_lastsector(cdDrive,i+1)-cdda_track_firstsector(cdDrive,i+1));
                    trackTags.at(i)->length = (8 * size) / (44100 * 2 * 16);

                    tagList.append( trackTags.at(i) );
                    tracks.append( i+1 );
                }
            }
        }

        ui.options->accepted();

        emit addTracks( device, tracks, trackCount, tagList, conversionOptions, command );

        accept();
    }
    else
    {
        QMessageBox::critical( this, "soundKonverter", i18n("No conversion options selected.") );
    }
}

void CDOpener::saveCuesheetClicked()
{
    const QString filename = QFileDialog::getSaveFileName(this, i18n("Save cuesheet"), "", "*.cue");
    if( filename.isEmpty() )
        return;

    QFile cueFile(filename);
    if( cueFile.exists() )
    {
        const int ret = QMessageBox::question(this, i18n("File already exists"), i18n("A file with this name already exists.\n\nDo you want to overwrite the existing one?"));
        if( ret == QMessageBox::No )
            return;
    }

    if( cueFile.open(QIODevice::WriteOnly) )
    {
        QTextStream ts(&cueFile);

        ts << "REM COMMENT \"soundKonverter " SOUNDKONVERTER_VERSION_STRING "\"\n";
        ts << "TITLE \"" + ui.albumLineEdit->text() + "\"\n";
        ts << "PERFORMER \"" + ui.albumArtistLineEdit->text() + "\"\n";
        ts << "FILE \"\" WAVE\n";

        for( int i=0; i<trackTags.count(); i++ )
        {
            ts << QString().sprintf("  TRACK %02i AUDIO\n", trackTags.at(i)->track);
            ts << "    TITLE \"" + trackTags.at(i)->title + "\"\n";
            ts << "    PERFORMER \"" + trackTags.at(i)->artist + "\"\n";
            ts << "    SONGWRITER \"" + trackTags.at(i)->composer + "\"\n";
            const long size = CD_FRAMESIZE_RAW * cdda_track_firstsector(cdDrive,i+1);
            const long length = (8 * size) / (44100 * 2 * 16);
            const long frames = (8 * size) / (588 * 2 * 16);
            ts << QString().sprintf("    INDEX 01 %02li:%02li:%02li\n", length/60, length%60, frames%75);
        }

        cueFile.close();
    }
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
