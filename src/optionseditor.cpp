//
// C++ Implementation: optionseditor
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008 - 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "optionseditor.h"
#include "options.h"
#include "config.h"
#include "filelistitem.h"

#include "global.h"

#include <QApplication>
#include <QBoxLayout>
#include <KComboBox>
#include <KIcon>
#include <KLineEdit>
#include <KLocale>
#include <KNumInput>
#include <KPushButton>
#include <KTextEdit>
#include <QDateTime>
#include <QLabel>


// TODO use QPointer or QSharedPointer

OptionsEditor::OptionsEditor( Config *_config, QWidget *parent )
    : KPageDialog( parent ),
    config( _config )
{
    tagEngine = config->tagEngine();

    setButtons( KDialog::User1 | KDialog::User2 | KDialog::Ok | KDialog::Apply | KDialog::Cancel );
    setButtonText( KDialog::User2, i18n("Previous") );
    setButtonIcon( KDialog::User2, KIcon("go-previous") );
    setButtonText( KDialog::User1, i18n("Next") );
    setButtonIcon( KDialog::User1, KIcon("go-next") );
    connect( this, SIGNAL(applyClicked()), this, SLOT(applyChanges()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(applyChanges()) );

    button(KDialog::User2)->setAutoRepeat(true);
    button(KDialog::User1)->setAutoRepeat(true);

    QWidget *conversionOptionsWidget = new QWidget( this );
    KPageWidgetItem *conversionOptionsPage = addPage( conversionOptionsWidget, i18n("Conversion") );
    conversionOptionsPage->setIcon( KIcon( "view-list-text" ) );

    // the grid for all widgets in the main window
    QGridLayout* conversionOptionsGridLayout = new QGridLayout( conversionOptionsWidget );
    // generate the options input area
    options = new Options( config, i18n("Select your desired output options and click on \"Ok\"."), conversionOptionsWidget );
    conversionOptionsGridLayout->addWidget( options, 0, 0 );
    conversionOptionsGridLayout->setRowStretch( 1, 1 );

    lEditOptions = new QLabel( "", conversionOptionsWidget );
    conversionOptionsGridLayout->addWidget( lEditOptions, 2, 0 );
    lEditOptions->setAlignment( Qt::AlignHCenter );
    lEditOptions->hide();
    pEditOptions = new KPushButton( i18n("Edit conversion options"), conversionOptionsWidget );
    pEditOptions->setFixedWidth( pEditOptions->sizeHint().width() );
    conversionOptionsGridLayout->addWidget( pEditOptions, 3, 0, Qt::AlignHCenter );
    pEditOptions->hide();
    connect( pEditOptions, SIGNAL(clicked()), this, SLOT(editOptionsClicked()) );



    QWidget *tagsWidget = new QWidget( this );
    KPageWidgetItem *tagsPage = addPage( tagsWidget, i18n("Tags") );
    tagsPage->setIcon( KIcon( "feed-subscribe" ) );

    // the grid for all widgets in the main window
    QGridLayout* tagsGridLayout = new QGridLayout( tagsWidget );

    // add the inputs
    // add a horizontal box layout for the covers
    QHBoxLayout *coversBox = new QHBoxLayout();
    tagsGridLayout->addLayout( coversBox, 0, 1 );
    // and fill it up
    lCoversLabel = new QLabel( i18n("Covers:"), tagsWidget );
    tagsGridLayout->addWidget( lCoversLabel, 0, 0 );
    bCovers = new QHBoxLayout();
    coversBox->addLayout( bCovers );
    coversBox->addStretch();

    // add a horizontal box layout for the title and track number
    QHBoxLayout *titleBox = new QHBoxLayout();
    tagsGridLayout->addLayout( titleBox, 1, 1 );
    // and fill it up
    lTitleLabel = new QLabel( i18n("Title:"), tagsWidget );
    tagsGridLayout->addWidget( lTitleLabel, 1, 0 );
    lTitle = new KLineEdit( tagsWidget );
    titleBox->addWidget( lTitle );
    pTitleEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pTitleEdit->setFixedSize( pTitleEdit->sizeHint().height(), lTitle->sizeHint().height() );
    pTitleEdit->setFlat( true );
    pTitleEdit->setToolTip( i18n("Edit") );
    pTitleEdit->hide();
    titleBox->addWidget( pTitleEdit );
    connect( pTitleEdit, SIGNAL(clicked()), this, SLOT(editTitleClicked()) );
    lTrackLabel = new QLabel( i18n("Track No.:"), tagsWidget );
    titleBox->addWidget( lTrackLabel );
    iTrack = new KIntSpinBox( 0, 999, 1, 1, tagsWidget );
    titleBox->addWidget( iTrack );
    pTrackEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pTrackEdit->setFixedSize( pTrackEdit->sizeHint().height(), iTrack->sizeHint().height() );
    pTrackEdit->setFlat( true );
    pTrackEdit->setToolTip( i18n("Edit") );
    pTrackEdit->hide();
    titleBox->addWidget( pTrackEdit );
    connect( pTrackEdit, SIGNAL(clicked()), this, SLOT(editTrackClicked()) );
    lTrackTotalLabel = new QLabel( i18nc("Track/Disc No. x of y","of"), tagsWidget );
    titleBox->addWidget( lTrackTotalLabel );
    iTrackTotal = new KIntSpinBox( 0, 999, 1, 1, tagsWidget );
    titleBox->addWidget( iTrackTotal );
    pTrackTotalEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pTrackTotalEdit->setFixedSize( pTrackTotalEdit->sizeHint().height(), iTrackTotal->sizeHint().height() );
    pTrackTotalEdit->setFlat( true );
    pTrackTotalEdit->setToolTip( i18n("Edit") );
    pTrackTotalEdit->hide();
    titleBox->addWidget( pTrackTotalEdit );
    connect( pTrackTotalEdit, SIGNAL(clicked()), this, SLOT(editTrackTotalClicked()) );

    // add a horizontal box layout for the artist and the composer
    QHBoxLayout *artistBox = new QHBoxLayout();
    tagsGridLayout->addLayout( artistBox, 2, 1 );
    // and fill it up
    lArtistLabel = new QLabel( i18n("Artist:"), tagsWidget );
    tagsGridLayout->addWidget( lArtistLabel, 2, 0 );
    lArtist = new KLineEdit( tagsWidget );
    artistBox->addWidget( lArtist );
    pArtistEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pArtistEdit->setFixedSize( pArtistEdit->sizeHint().height(), lArtist->sizeHint().height() );
    pArtistEdit->setFlat( true );
    pArtistEdit->setToolTip( i18n("Edit") );
    pArtistEdit->hide();
    artistBox->addWidget( pArtistEdit );
    connect( pArtistEdit, SIGNAL(clicked()), this, SLOT(editArtistClicked()) );
    lComposerLabel = new QLabel( i18n("Composer:"), tagsWidget );
    artistBox->addWidget( lComposerLabel );
    lComposer = new KLineEdit( tagsWidget );
    artistBox->addWidget( lComposer );
    pComposerEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pComposerEdit->setFixedSize( pComposerEdit->sizeHint().height(), lComposer->sizeHint().height() );
    pComposerEdit->setFlat( true );
    pComposerEdit->setToolTip( i18n("Edit") );
    pComposerEdit->hide();
    artistBox->addWidget( pComposerEdit );
    connect( pComposerEdit, SIGNAL(clicked()), this, SLOT(editComposerClicked()) );

    // add a horizontal box layout for the album
    QHBoxLayout *albumArtistBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumArtistBox, 3, 1 );
    // and fill it up
    lAlbumArtistLabel = new QLabel( i18n("Album artist:"), tagsWidget );
    tagsGridLayout->addWidget( lAlbumArtistLabel, 3, 0 );
    lAlbumArtist = new KLineEdit( tagsWidget );
    albumArtistBox->addWidget( lAlbumArtist );
    pAlbumArtistEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pAlbumArtistEdit->setFixedSize( pAlbumArtistEdit->sizeHint().height(), lAlbumArtist->sizeHint().height() );
    pAlbumArtistEdit->setFlat( true );
    pAlbumArtistEdit->setToolTip( i18n("Edit") );
    pAlbumArtistEdit->hide();
    albumArtistBox->addWidget( pAlbumArtistEdit );
    connect( pAlbumArtistEdit, SIGNAL(clicked()), this, SLOT(editAlbumArtistClicked()) );

    // add a horizontal box layout for the album
    QHBoxLayout *albumBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumBox, 4, 1 );
    // and fill it up
    lAlbumLabel = new QLabel( i18n("Album:"), tagsWidget );
    tagsGridLayout->addWidget( lAlbumLabel, 4, 0 );
    lAlbum = new KLineEdit( tagsWidget );
    albumBox->addWidget( lAlbum );
    pAlbumEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pAlbumEdit->setFixedSize( pAlbumEdit->sizeHint().height(), lAlbum->sizeHint().height() );
    pAlbumEdit->setFlat( true );
    pAlbumEdit->setToolTip( i18n("Edit") );
    pAlbumEdit->hide();
    albumBox->addWidget( pAlbumEdit );
    connect( pAlbumEdit, SIGNAL(clicked()), this, SLOT(editAlbumClicked()) );

    // add a horizontal box layout for the disc number, year and genre
    QHBoxLayout *albumdataBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumdataBox, 5, 1 );
    // and fill it up
    lDiscLabel = new QLabel( i18n("Disc No.:"), tagsWidget );
    tagsGridLayout->addWidget( lDiscLabel, 5, 0 );
    iDisc = new KIntSpinBox( 0, 99, 1, 1, tagsWidget );
    albumdataBox->addWidget( iDisc );
    pDiscEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pDiscEdit->setFixedSize( pDiscEdit->sizeHint().height(), iDisc->sizeHint().height() );
    pDiscEdit->setFlat( true );
    pDiscEdit->setToolTip( i18n("Edit") );
    pDiscEdit->hide();
    albumdataBox->addWidget( pDiscEdit );
    connect( pDiscEdit, SIGNAL(clicked()), this, SLOT(editDiscClicked()) );
    lDiscTotalLabel = new QLabel( i18nc("Track/Disc No. x of y","of"), tagsWidget );
    albumdataBox->addWidget( lDiscTotalLabel );
    iDiscTotal = new KIntSpinBox( 0, 99, 1, 1, tagsWidget );
    albumdataBox->addWidget( iDiscTotal );
    pDiscTotalEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pDiscTotalEdit->setFixedSize( pDiscTotalEdit->sizeHint().height(), iDiscTotal->sizeHint().height() );
    pDiscTotalEdit->setFlat( true );
    pDiscTotalEdit->setToolTip( i18n("Edit") );
    pDiscTotalEdit->hide();
    albumdataBox->addWidget( pDiscTotalEdit );
    connect( pDiscTotalEdit, SIGNAL(clicked()), this, SLOT(editDiscTotalClicked()) );
    albumdataBox->addStretch();
    lYearLabel = new QLabel( i18n("Year:"), tagsWidget );
    albumdataBox->addWidget( lYearLabel );
    iYear = new KIntSpinBox( 0, 99999, 1, QDate::currentDate().year(), tagsWidget );
    albumdataBox->addWidget( iYear );
    pYearEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pYearEdit->setFixedSize( pYearEdit->sizeHint().height(), iYear->sizeHint().height() );
    pYearEdit->setFlat( true );
    pYearEdit->setToolTip( i18n("Edit") );
    pYearEdit->hide();
    albumdataBox->addWidget( pYearEdit );
    connect( pYearEdit, SIGNAL(clicked()), this, SLOT(editYearClicked()) );
    albumdataBox->addStretch();
    lGenreLabel = new QLabel( i18n("Genre:"), tagsWidget );
    albumdataBox->addWidget( lGenreLabel );
    cGenre = new KComboBox( true, tagsWidget );
    cGenre->addItems( tagEngine->genreList );
    cGenre->clearEditText();
    KCompletion *cGenreCompletion = cGenre->completionObject();
    cGenreCompletion->insertItems( tagEngine->genreList );
    cGenreCompletion->setIgnoreCase( tagsWidget );
    albumdataBox->addWidget( cGenre );
    pGenreEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pGenreEdit->setFixedSize( pGenreEdit->sizeHint().height(), cGenre->sizeHint().height() );
    pGenreEdit->setFlat( true );
    pGenreEdit->setToolTip( i18n("Edit") );
    pGenreEdit->hide();
    albumdataBox->addWidget( pGenreEdit );
    connect( pGenreEdit, SIGNAL(clicked()), this, SLOT(editGenreClicked()) );

    // add a horizontal box layout for the comment
    QHBoxLayout *commentBox = new QHBoxLayout();
    tagsGridLayout->addLayout( commentBox, 6, 1 );
    // and fill it up
    lCommentLabel = new QLabel( i18n("Comment:"), tagsWidget );
    tagsGridLayout->addWidget( lCommentLabel, 6, 0 );
    tComment = new KTextEdit( tagsWidget );
    commentBox->addWidget( tComment );
    pCommentEdit = new KPushButton( KIcon("edit-rename"), " ", tagsWidget );
    pCommentEdit->setFixedSize( pCommentEdit->sizeHint().height(), lTitle->sizeHint().height() );
    pCommentEdit->setFlat( true );
    pCommentEdit->setToolTip( i18n("Edit") );
    pCommentEdit->hide();
    commentBox->addWidget( pCommentEdit );
    connect( pCommentEdit, SIGNAL(clicked()), this, SLOT(editCommentClicked()) );
    tagsGridLayout->setRowStretch( 6, 1 );

    lEditTags = new QLabel( "", tagsWidget );
    tagsGridLayout->addWidget( lEditTags, 7, 1 );
    lEditTags->setAlignment( Qt::AlignHCenter );
    lEditTags->hide();
    pEditTags = new KPushButton( i18n("Edit tags"), tagsWidget );
    pEditTags->setFixedWidth( pEditTags->sizeHint().width() );
    tagsGridLayout->addWidget( pEditTags, 8, 1, Qt::AlignHCenter );
    pEditTags->hide();
    connect( pEditTags, SIGNAL(clicked()), this, SLOT(editTagsClicked()) );
}

OptionsEditor::~OptionsEditor()
{}

void OptionsEditor::setTagInputEnabled( bool enabled )
{
    lTitleLabel->setEnabled( enabled );
    lTitle->setEnabled( enabled );
    pTitleEdit->hide();
    lTrackLabel->setEnabled( enabled );
    iTrack->setEnabled( enabled );
    lTrackTotalLabel->setEnabled( enabled );
    iTrackTotal->setEnabled( enabled );
    pTrackEdit->hide();
    lArtistLabel->setEnabled( enabled );
    lArtist->setEnabled( enabled );
    pArtistEdit->hide();
    lComposerLabel->setEnabled( enabled );
    lComposer->setEnabled( enabled );
    pComposerEdit->hide();
    lAlbumArtistLabel->setEnabled( enabled );
    lAlbumArtist->setEnabled( enabled );
    pAlbumArtistEdit->hide();
    lAlbumLabel->setEnabled( enabled );
    lAlbum->setEnabled( enabled );
    pAlbumEdit->hide();
    lDiscLabel->setEnabled( enabled );
    iDisc->setEnabled( enabled );
    lDiscTotalLabel->setEnabled( enabled );
    iDiscTotal->setEnabled( enabled );
    pDiscEdit->hide();
    lYearLabel->setEnabled( enabled );
    iYear->setEnabled( enabled );
    pYearEdit->hide();
    lGenreLabel->setEnabled( enabled );
    cGenre->setEnabled( enabled );
    pGenreEdit->hide();
    lCommentLabel->setEnabled( enabled );
    tComment->setEnabled( enabled );
    tComment->setReadOnly( !enabled );
    pCommentEdit->hide();

    if( !enabled )
    {
        lTitle->setText( "" );
        iTrack->setValue( 0 );
        iTrackTotal->setValue( 0 );
        lArtist->setText( "" );
        lComposer->setText( "" );
        lAlbumArtist->setText( "" );
        lAlbum->setText( "" );
        iDisc->setValue( 0 );
        iDiscTotal->setValue( 0 );
        iYear->setValue( 0 );
        cGenre->setEditText( "" );
        tComment->setText( "" );
    }
}

void OptionsEditor::itemsSelected( QList<FileListItem*> items )
{
    selectedItems.clear();
    foreach( FileListItem *item, items )
    {
        bool canEdit = false;
        if( item )
        {
            switch( item->state )
            {
                case FileListItem::WaitingForConversion:
                    canEdit = true;
                    break;
                case FileListItem::Ripping:
                    break;
                case FileListItem::Converting:
                    break;
                case FileListItem::ApplyingReplayGain:
                    break;
                case FileListItem::WaitingForAlbumGain:
                    break;
                case FileListItem::ApplyingAlbumGain:
                    break;
                case FileListItem::Stopped:
                    canEdit = true;
                    break;
            }
        }
        if( canEdit )
            selectedItems.append( item );
    }

    // remove all cover widgets
    foreach( QLabel* label, lCovers )
    {
        bCovers->removeWidget( label );
        label->deleteLater();
    }
    lCovers.clear();
    lCoversLabel->setEnabled( false );

    if( selectedItems.count() == 0 )
    {
        setCaption( i18n("No file selected") );
        options->setEnabled( false );
        lEditOptions->hide();
        pEditOptions->hide();
        setTagInputEnabled( false );
        lEditTags->hide();
        pEditTags->hide();

        return;
    }

    options->setEnabled( true );
    lEditOptions->hide();
    pEditOptions->hide();
    setTagInputEnabled( true );
    lEditTags->hide();
    pEditTags->hide();

    if( selectedItems.count() == 1 )
    {
        FileListItem *item = selectedItems.first();

        setCaption( selectedItems.first()->url.fileName() );

        const bool success = options->setCurrentConversionOptions( config->conversionOptionsManager()->getConversionOptions(selectedItems.first()->conversionOptionsId) );
        options->setEnabled( success );
        // TODO show error message if !success

        if( item->tags == 0 && !item->local )
        {
            setTagInputEnabled( false );
            lEditTags->setText( i18n("The tags could not be read, because this is not a local file.\n"
                    "soundKonverter will try to read the tags, when it is about to convert the file.\n"
                    "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                    "to read the tags after downloading.") );
            lEditTags->show();
            pEditTags->show();
        }
        else if( item->tags == 0 )
        {
            setTagInputEnabled( false );
            lEditTags->setText( i18n("Reading the tags of this file failed.\n"
                    "soundKonverter will try to read the tags a second time, when it is about to convert the file.\n"
                    "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                    "to read the tags a second time.") );
            lEditTags->show();
            pEditTags->show();
        }
        else
        {
            const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

            if( !(item->tags->tagsRead & TagData::Covers) )
            {
                item->tags->covers = tagEngine->readCovers( item->url );
                item->tags->tagsRead = TagData::TagsRead(item->tags->tagsRead | TagData::Covers);
            }
            foreach( CoverData* cover, item->tags->covers )
            {
                QPixmap pixmap;
                pixmap.loadFromData( cover->data );

                QLabel *label = new QLabel();
                label->setFrameShape( QFrame::StyledPanel );
                label->setFrameShadow( QFrame::Raised );
                QString toolTip;
                toolTip += "<span style='white-space:pre'><table>";
                toolTip += "<tr><td>" + i18nc("cover tooltip","Cover type:") + "</td><td>" + CoverData::roleName(cover->role) + "</td></tr>";
                if( !cover->description.isEmpty() )
                    toolTip += "<tr><td>" + i18nc("cover tooltip","Description:") + "</td><td>" + cover->description + "</td></tr>";
                toolTip += "<tr><td>" + i18nc("cover tooltip","Size:") + "</td><td>" + i18nc("cover tooltip","%1 x %2 pixels (%3)",pixmap.width(),pixmap.height(),Global::prettyNumber(cover->data.size(),"B")) + "</td></tr>";
                if( !cover->mimeType.isEmpty() )
                    toolTip += "<tr><td>" + i18nc("cover tooltip","Mime type:") + "</td><td>" + cover->mimeType + "</td></tr>";
                toolTip += "</table></span>";
                label->setToolTip( toolTip );
                bCovers->addWidget( label );
                lCovers.append( label );

                label->setPixmap( pixmap.scaledToHeight( 4.5*fontHeight, Qt::SmoothTransformation ) );
            }
            lCoversLabel->setEnabled( item->tags->covers.count() > 0 );
            lTitle->setText( item->tags->title );
            iTrack->setValue( item->tags->track );
            iTrackTotal->setValue( item->tags->trackTotal );
            lArtist->setText( item->tags->artist );
            lComposer->setText( item->tags->composer );
            lAlbumArtist->setText( item->tags->albumArtist );
            lAlbum->setText( item->tags->album );
            iDisc->setValue( item->tags->disc );
            iDiscTotal->setValue( item->tags->discTotal );
            iYear->setValue( item->tags->year );
            cGenre->setEditText( item->tags->genre );
            tComment->setText( item->tags->comment );
        }
    }
    else // selectedItems.count() > 1
    {
        setCaption( i18n("%1 Files").arg(selectedItems.count()) );

        FileListItem *firstItem = selectedItems.first();
        const int     conversionOptionsId = firstItem->conversionOptionsId;
        const QString title               = ( firstItem->tags == 0 ) ? "" : firstItem->tags->title;
        const int     track               = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->track;
        const int     trackTotal          = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->trackTotal;
        const QString artist              = ( firstItem->tags == 0 ) ? "" : firstItem->tags->artist;
        const QString composer            = ( firstItem->tags == 0 ) ? "" : firstItem->tags->composer;
        const QString albumArtist         = ( firstItem->tags == 0 ) ? "" : firstItem->tags->albumArtist;
        const QString album               = ( firstItem->tags == 0 ) ? "" : firstItem->tags->album;
        const int     disc                = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->disc;
        const int     discTotal           = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->discTotal;
        const int     year                = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->year;
        const QString genre               = ( firstItem->tags == 0 ) ? "" : firstItem->tags->genre;
        const QString comment             = ( firstItem->tags == 0 ) ? "" : firstItem->tags->comment;

        foreach( FileListItem *item, selectedItems )
        {
            if( item->conversionOptionsId != conversionOptionsId )
            {
                options->setEnabled( false );
                lEditOptions->setText( i18n("You have selected multiple files with different conversion options.\nYou can change the options of all files by hitting the button below.") );
                lEditOptions->show();
                pEditOptions->show();
            }

            if( item->tags == 0 )
            {
                setTagInputEnabled( false );
                lEditTags->setText( i18n("Reading the tags of one or more files failed.\n"
                        "soundKonverter will try to read the tags a second time, when it is about to convert the files.\n"
                        "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                        "to read the tags a second time.") );
                lEditTags->show();
                pEditTags->show();
                continue;
            }

            if( title != item->tags->title && lTitle->isEnabled() )
            {
                lTitle->setEnabled( false );
                lTitle->setText( "" );
                pTitleEdit->show();
            }
            if( track != item->tags->track && iTrack->isEnabled() )
            {
                iTrack->setEnabled( false );
                iTrack->setValue( 1 );
                pTrackEdit->show();
            }
            if( trackTotal != item->tags->trackTotal && iTrackTotal->isEnabled() )
            {
                iTrackTotal->setEnabled( false );
                iTrackTotal->setValue( 1 );
                pTrackTotalEdit->show();
            }
            if( artist != item->tags->artist && lArtist->isEnabled() )
            {
                lArtist->setEnabled( false );
                lArtist->setText( "" );
                pArtistEdit->show();
            }
            if( composer != item->tags->composer && lComposer->isEnabled() )
            {
                lComposer->setEnabled( false );
                lComposer->setText( "" );
                pComposerEdit->show();
            }
            if( albumArtist != item->tags->albumArtist && lAlbumArtist->isEnabled() )
            {
                lAlbumArtist->setEnabled( false );
                lAlbumArtist->setText( "" );
                pAlbumArtistEdit->show();
            }
            if( album != item->tags->album && lAlbum->isEnabled() )
            {
                lAlbum->setEnabled( false );
                lAlbum->setText( "" );
                pAlbumEdit->show();
            }
            if( disc != item->tags->disc && iDisc->isEnabled() )
            {
                iDisc->setEnabled( false );
                iDisc->setValue( 1 );
                pDiscEdit->show();
            }
            if( discTotal != item->tags->discTotal && iDiscTotal->isEnabled() )
            {
                iDiscTotal->setEnabled( false );
                iDiscTotal->setValue( 1 );
                pDiscTotalEdit->show();
            }
            if( year != item->tags->year && iYear->isEnabled() )
            {
                iYear->setEnabled( false );
                iYear->setValue( QDate::currentDate().year() );
                pYearEdit->show();
            }
            if( genre != item->tags->genre && cGenre->isEnabled() )
            {
                cGenre->setEnabled( false );
                cGenre->setEditText( "" );
                pGenreEdit->show();
            }
            if( comment != item->tags->comment && tComment->isEnabled() )
            {
                tComment->setEnabled( false );
                tComment->setReadOnly( true );
                tComment->setText( "" );
                pCommentEdit->show();
            }
        }

        if( options->isEnabled() )
        {
            const bool success = options->setCurrentConversionOptions( config->conversionOptionsManager()->getConversionOptions(conversionOptionsId) );
            options->setEnabled( success );
            // TODO show error message if !success
        }

        if( lTitle->isEnabled() )
            lTitle->setText( title );

        if( iTrack->isEnabled() )
            iTrack->setValue( track );

        if( iTrackTotal->isEnabled() )
            iTrackTotal->setValue( trackTotal );

        if( lArtist->isEnabled() )
            lArtist->setText( artist );

        if( lComposer->isEnabled() )
            lComposer->setText( composer );

        if( lAlbumArtist->isEnabled() )
            lAlbumArtist->setText( albumArtist );

        if( lAlbum->isEnabled() )
            lAlbum->setText( album );

        if( iDisc->isEnabled() )
            iDisc->setValue( disc );

        if( iDiscTotal->isEnabled() )
            iDiscTotal->setValue( discTotal );

        if( iYear->isEnabled() )
            iYear->setValue( year );

        if( cGenre->isEnabled() )
            cGenre->setEditText( genre );

        if( tComment->isEnabled() )
            tComment->setText( comment );
    }
}

void OptionsEditor::setPreviousEnabled( bool enabled )
{
    enableButton( User2, enabled );
}

void OptionsEditor::setNextEnabled( bool enabled )
{
    enableButton( User1, enabled );
}

void OptionsEditor::applyChanges()
{
    ConversionOptions *newConversionOptions = options->currentConversionOptions();

    for( int i=0; i<selectedItems.count(); i++ )
    {
        if( !selectedItems.at(i) )
            continue;

        if( newConversionOptions && options->isEnabled() )
        {
            selectedItems.at(i)->conversionOptionsId = config->conversionOptionsManager()->updateConversionOptions( selectedItems.at(i)->conversionOptionsId, newConversionOptions );
        }
        else
        {
            // TODO error message
        }

        if( selectedItems.at(i)->tags )
        {
            if( lTitle->isEnabled() )
                selectedItems.at(i)->tags->title = lTitle->text();

            if( iTrack->isEnabled() )
                selectedItems.at(i)->tags->track = iTrack->value();

            if( iTrackTotal->isEnabled() )
                selectedItems.at(i)->tags->trackTotal = iTrackTotal->value();

            if( lArtist->isEnabled() )
                selectedItems.at(i)->tags->artist = lArtist->text();

            if( lComposer->isEnabled() )
                selectedItems.at(i)->tags->composer = lComposer->text();

            if( lAlbumArtist->isEnabled() )
                selectedItems.at(i)->tags->albumArtist = lAlbumArtist->text();

            if( lAlbum->isEnabled() )
                selectedItems.at(i)->tags->album = lAlbum->text();

            if( iDisc->isEnabled() )
                selectedItems.at(i)->tags->disc = iDisc->value();

            if( iDiscTotal->isEnabled() )
                selectedItems.at(i)->tags->discTotal= iDiscTotal->value();

            if( iYear->isEnabled() )
                selectedItems.at(i)->tags->year = iYear->value();

            if( cGenre->isEnabled() )
                selectedItems.at(i)->tags->genre = cGenre->currentText();

            if( tComment->isEnabled() )
                selectedItems.at(i)->tags->comment = tComment->toPlainText();
        }
    }

    options->accepted();

    emit updateFileListItems( selectedItems );
}

void OptionsEditor::editOptionsClicked()
{
    options->setEnabled( true );
    lEditOptions->hide();
    pEditOptions->hide();

    if( selectedItems.count() > 0 && selectedItems.first() )
    {
        ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions( selectedItems.first()->conversionOptionsId );
        if( conversionOptions )
            options->setCurrentConversionOptions( conversionOptions );
    }
}

void OptionsEditor::editTagsClicked()
{
    for( int i=0; i<selectedItems.count(); i++ )
    {
        selectedItems.at(i)->tags = new TagData();
    }

    editTitleClicked();
    editTrackClicked();
    editTrackTotalClicked();
    editArtistClicked();
    editComposerClicked();
    editAlbumClicked();
    editDiscClicked();
    editDiscTotalClicked();
    editYearClicked();
    editGenreClicked();
    editCommentClicked();
}

void OptionsEditor::editTitleClicked()
{
    lTitle->setEnabled( true );
    lTitle->setFocus();
    pTitleEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        lTitle->setText( selectedItems.first()->tags->title );
    }
}

void OptionsEditor::editTrackClicked()
{
    iTrack->setEnabled( true );
    iTrack->setFocus();
    pTrackEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iTrack->setValue( selectedItems.first()->tags->track );
    }
}

void OptionsEditor::editTrackTotalClicked()
{
    iTrackTotal->setEnabled( true );
    iTrackTotal->setFocus();
    pTrackTotalEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iTrackTotal->setValue( selectedItems.first()->tags->trackTotal );
    }
}

void OptionsEditor::editArtistClicked()
{
    lArtist->setEnabled( true );
    lArtist->setFocus();
    pArtistEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        lArtist->setText( selectedItems.first()->tags->artist );
    }
}

void OptionsEditor::editComposerClicked()
{
    lComposer->setEnabled( true );
    lComposer->setFocus();
    pComposerEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        lComposer->setText( selectedItems.first()->tags->composer );
    }
}

void OptionsEditor::editAlbumArtistClicked()
{
    lAlbumArtist->setEnabled( true );
    lAlbumArtist->setFocus();
    pAlbumArtistEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        lAlbumArtist->setText( selectedItems.first()->tags->albumArtist );
    }
}

void OptionsEditor::editAlbumClicked()
{
    lAlbum->setEnabled( true );
    lAlbum->setFocus();
    pAlbumEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        lAlbum->setText( selectedItems.first()->tags->album );
    }
}

void OptionsEditor::editDiscClicked()
{
    iDisc->setEnabled( true );
    iDisc->setFocus();
    pDiscEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iDisc->setValue( selectedItems.first()->tags->disc );
    }
}

void OptionsEditor::editDiscTotalClicked()
{
    iDiscTotal->setEnabled( true );
    iDiscTotal->setFocus();
    pDiscTotalEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iDiscTotal->setValue( selectedItems.first()->tags->discTotal );
    }
}

void OptionsEditor::editYearClicked()
{
    iYear->setEnabled( true );
    iYear->setFocus();
    pYearEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iYear->setValue( selectedItems.first()->tags->year );
    }
}

void OptionsEditor::editGenreClicked()
{
    cGenre->setEnabled( true );
    cGenre->setFocus();
    pGenreEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        cGenre->setCurrentItem( selectedItems.first()->tags->genre );
    }
}

void OptionsEditor::editCommentClicked()
{
    tComment->setEnabled( true );
    tComment->setReadOnly( false );
    tComment->setFocus();
    pCommentEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        tComment->setPlainText( selectedItems.first()->tags->comment );
    }
}



