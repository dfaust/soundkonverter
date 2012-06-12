//
// C++ Implementation: optionseditor
//
// Description:
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "optionseditor.h"
#include "options.h"
#include "config.h"
#include "filelistitem.h"

#include <kicon.h>
#include <klocale.h>

#include <qlabel.h>
#include <qdatetime.h>

#include <kpushbutton.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <knuminput.h>
#include <ktextedit.h>
#include "global.h"


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
    options = new Options( config, i18n("Choose your prefered output options and click on \"Apply\"!"), conversionOptionsWidget );
    conversionOptionsGridLayout->addWidget( options, 0, 0 );
//     connect( options, SIGNAL(optionsChanged()), this, SLOT(optionsChanged()) );
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
//     connect( lTitle, SIGNAL(textChanged(const QString&)), this, SLOT(titleChanged(const QString&)) );
    pTitleEdit = new KPushButton( " ", tagsWidget );
    pTitleEdit->setIcon( KIcon("edit-rename") );
    pTitleEdit->setFixedSize( lTitle->sizeHint().height(), lTitle->sizeHint().height() );
    pTitleEdit->hide();
    titleBox->addWidget( pTitleEdit );
    connect( pTitleEdit, SIGNAL(clicked()), this, SLOT(editTitleClicked()) );
    lNumberLabel = new QLabel( i18n("Track No.:"), tagsWidget );
    titleBox->addWidget( lNumberLabel );
    iNumber = new KIntSpinBox( 0, 999, 1, 1, tagsWidget );
    titleBox->addWidget( iNumber );
//     connect( iNumber, SIGNAL(valueChanged(int)), this, SLOT(numberChanged(int)) );
    pNumberEdit = new KPushButton( " ", tagsWidget );
    pNumberEdit->setIcon( KIcon("edit-rename") );
    pNumberEdit->setFixedSize( iNumber->sizeHint().height(), iNumber->sizeHint().height() );
    pNumberEdit->hide();
    titleBox->addWidget( pNumberEdit );
    connect( pNumberEdit, SIGNAL(clicked()), this, SLOT(editNumberClicked()) );

    // add a horizontal box layout for the artist and the composer
    QHBoxLayout *artistBox = new QHBoxLayout();
    tagsGridLayout->addLayout( artistBox, 2, 1 );
    // and fill it up
    lArtistLabel = new QLabel( i18n("Artist:"), tagsWidget );
    tagsGridLayout->addWidget( lArtistLabel, 2, 0 );
    lArtist = new KLineEdit( tagsWidget );
    artistBox->addWidget( lArtist );
//     connect( lArtist, SIGNAL(textChanged(const QString&)), this, SLOT(artistChanged(const QString&)) );
    pArtistEdit = new KPushButton( " ", tagsWidget );
    pArtistEdit->setIcon( KIcon("edit-rename") );
    pArtistEdit->setFixedSize( lArtist->sizeHint().height(), lArtist->sizeHint().height() );
    pArtistEdit->hide();
    artistBox->addWidget( pArtistEdit );
    connect( pArtistEdit, SIGNAL(clicked()), this, SLOT(editArtistClicked()) );
    lComposerLabel = new QLabel( i18n("Composer:"), tagsWidget );
    artistBox->addWidget( lComposerLabel );
    lComposer = new KLineEdit( tagsWidget );
    artistBox->addWidget( lComposer );
//     connect( lComposer, SIGNAL(textChanged(const QString&)), this, SLOT(composerChanged(const QString&)) );
    pComposerEdit = new KPushButton( " ", tagsWidget );
    pComposerEdit->setIcon( KIcon("edit-rename") );
    pComposerEdit->setFixedSize( lComposer->sizeHint().height(), lComposer->sizeHint().height() );
    pComposerEdit->hide();
    artistBox->addWidget( pComposerEdit );
    connect( pComposerEdit, SIGNAL(clicked()), this, SLOT(editComposerClicked()) );

    // add a horizontal box layout for the album
    QHBoxLayout *albumBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumBox, 3, 1 );
    // and fill it up
    lAlbumLabel = new QLabel( i18n("Album:"), tagsWidget );
    tagsGridLayout->addWidget( lAlbumLabel, 3, 0 );
    lAlbum = new KLineEdit( tagsWidget );
    albumBox->addWidget( lAlbum );
//     connect( lAlbum, SIGNAL(textChanged(const QString&)), this, SLOT(albumChanged(const QString&)) );
    pAlbumEdit = new KPushButton( " ", tagsWidget );
    pAlbumEdit->setIcon( KIcon("edit-rename") );
    pAlbumEdit->setFixedSize( lAlbum->sizeHint().height(), lAlbum->sizeHint().height() );
    pAlbumEdit->hide();
    albumBox->addWidget( pAlbumEdit );
    connect( pAlbumEdit, SIGNAL(clicked()), this, SLOT(editAlbumClicked()) );

    // add a horizontal box layout for the disc number, year and genre
    QHBoxLayout *albumdataBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumdataBox, 4, 1 );
    // and fill it up
    lDiscLabel = new QLabel( i18n("Disc No.:"), tagsWidget );
    tagsGridLayout->addWidget( lDiscLabel, 4, 0 );
    iDisc = new KIntSpinBox( 0, 99, 1, 1, tagsWidget );
    albumdataBox->addWidget( iDisc );
//     connect( iDisc, SIGNAL(valueChanged(int)), this, SLOT(discChanged(int)) );
    pDiscEdit = new KPushButton( " ", tagsWidget );
    pDiscEdit->setIcon( KIcon("edit-rename") );
    pDiscEdit->setFixedSize( iDisc->sizeHint().height(), iDisc->sizeHint().height() );
    pDiscEdit->hide();
    albumdataBox->addWidget( pDiscEdit );
    connect( pDiscEdit, SIGNAL(clicked()), this, SLOT(editDiscClicked()) );
    lDiscTotalLabel = new QLabel( i18nc("Disc No. x of y","of"), tagsWidget );
    albumdataBox->addWidget( lDiscTotalLabel );
    iDiscTotal = new KIntSpinBox( 0, 99, 1, 1, tagsWidget );
    albumdataBox->addWidget( iDiscTotal );
//     connect( iDisc, SIGNAL(valueChanged(int)), this, SLOT(discChanged(int)) );
    pDiscTotalEdit = new KPushButton( " ", tagsWidget );
    pDiscTotalEdit->setIcon( KIcon("edit-rename") );
    pDiscTotalEdit->setFixedSize( iDisc->sizeHint().height(), iDisc->sizeHint().height() );
    pDiscTotalEdit->hide();
    albumdataBox->addWidget( pDiscTotalEdit );
    connect( pDiscTotalEdit, SIGNAL(clicked()), this, SLOT(editDiscTotalClicked()) );
    albumdataBox->addStretch();
    lYearLabel = new QLabel( i18n("Year:"), tagsWidget );
    albumdataBox->addWidget( lYearLabel );
    iYear = new KIntSpinBox( 0, 99999, 1, QDate::currentDate().year(), tagsWidget );
    albumdataBox->addWidget( iYear );
//     connect( iYear, SIGNAL(valueChanged(int)), this, SLOT(yearChanged(int)) );
    pYearEdit = new KPushButton( " ", tagsWidget );
    pYearEdit->setIcon( KIcon("edit-rename") );
    pYearEdit->setFixedSize( iYear->sizeHint().height(), iYear->sizeHint().height() );
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
//     connect( cGenre, SIGNAL(textChanged(const QString&)), this, SLOT(genreChanged(const QString&)) );
    pGenreEdit = new KPushButton( " ", tagsWidget );
    pGenreEdit->setIcon( KIcon("edit-rename") );
    pGenreEdit->setFixedSize( cGenre->sizeHint().height(), cGenre->sizeHint().height() );
    pGenreEdit->hide();
    albumdataBox->addWidget( pGenreEdit );
    connect( pGenreEdit, SIGNAL(clicked()), this, SLOT(editGenreClicked()) );

    // add a horizontal box layout for the comment
    QHBoxLayout *commentBox = new QHBoxLayout();
    tagsGridLayout->addLayout( commentBox, 5, 1 );
    // and fill it up
    lCommentLabel = new QLabel( i18n("Comment:"), tagsWidget );
    tagsGridLayout->addWidget( lCommentLabel, 5, 0 );
    tComment = new KTextEdit( tagsWidget );
    commentBox->addWidget( tComment );
//     connect( tComment, SIGNAL(textChanged()), this, SLOT(commentChanged()) );
    pCommentEdit = new KPushButton( " ", tagsWidget );
    pCommentEdit->setIcon( KIcon("edit-rename") );
    pCommentEdit->setFixedSize( lTitle->sizeHint().height(), lTitle->sizeHint().height() );
    pCommentEdit->hide();
    commentBox->addWidget( pCommentEdit );
    connect( pCommentEdit, SIGNAL(clicked()), this, SLOT(editCommentClicked()) );
    tagsGridLayout->setRowStretch( 5, 1 );

    lEditTags = new QLabel( "", tagsWidget );
    tagsGridLayout->addWidget( lEditTags, 6, 1 );
    lEditTags->setAlignment( Qt::AlignHCenter );
    lEditTags->hide();
    pEditTags = new KPushButton( i18n("Edit tags"), tagsWidget );
    pEditTags->setFixedWidth( pEditTags->sizeHint().width() );
    tagsGridLayout->addWidget( pEditTags, 7, 1, Qt::AlignHCenter );
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
    lNumberLabel->setEnabled( enabled );
    iNumber->setEnabled( enabled );
    pNumberEdit->hide();
    lArtistLabel->setEnabled( enabled );
    lArtist->setEnabled( enabled );
    pArtistEdit->hide();
    lComposerLabel->setEnabled( enabled );
    lComposer->setEnabled( enabled );
    pComposerEdit->hide();
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
        iNumber->setValue( 0 );
        lArtist->setText( "" );
        lComposer->setText( "" );
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
                case FileListItem::BackendNeedsConfiguration:
                    canEdit = true;
                    break;
                case FileListItem::DiscFull:
                    canEdit = true;
                    break;
                case FileListItem::Skipped:
                    canEdit = true;
                    break;
                case FileListItem::Failed:
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
            if( !item->tags->coversRead )
            {
                item->tags->covers = tagEngine->readCovers( item->url );
                item->tags->coversRead = true;
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

                label->setPixmap( pixmap.scaledToHeight( 48, Qt::SmoothTransformation ) );
            }
            lCoversLabel->setEnabled( item->tags->covers.count() > 0 );
            lTitle->setText( item->tags->title );
            iNumber->setValue( item->tags->track );
            lArtist->setText( item->tags->artist );
            lComposer->setText( item->tags->composer );
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
        const int     number              = ( firstItem->tags == 0 ) ? 0  : firstItem->tags->track;
        const QString artist              = ( firstItem->tags == 0 ) ? "" : firstItem->tags->artist;
        const QString composer            = ( firstItem->tags == 0 ) ? "" : firstItem->tags->composer;
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
            if( number != item->tags->track && iNumber->isEnabled() )
            {
                iNumber->setEnabled( false );
                iNumber->setValue( 1 );
                pNumberEdit->show();
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

        if( iNumber->isEnabled() )
            iNumber->setValue( number );

        if( lArtist->isEnabled() )
            lArtist->setText( artist );

        if( lComposer->isEnabled() )
            lComposer->setText( composer );

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

            if( iNumber->isEnabled() )
                selectedItems.at(i)->tags->track = iNumber->value();

            if( lArtist->isEnabled() )
                selectedItems.at(i)->tags->artist = lArtist->text();

            if( lComposer->isEnabled() )
                selectedItems.at(i)->tags->composer = lComposer->text();

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
    editNumberClicked();
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

void OptionsEditor::editNumberClicked()
{
    iNumber->setEnabled( true );
    iNumber->setFocus();
    pNumberEdit->hide();

    if( selectedItems.count() > 0 && selectedItems.first() && selectedItems.first()->tags )
    {
        iNumber->setValue( selectedItems.first()->tags->track );
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



