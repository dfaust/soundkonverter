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

#include <qlayout.h>
#include <qlabel.h>
#include <qdatetime.h>

#include <kpushbutton.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <knuminput.h>
#include <ktextedit.h>

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
//     connect( pEditOptions, SIGNAL(clicked()), this, SLOT(editOptionsClicked()) );



    QWidget *tagsWidget = new QWidget( this );
    KPageWidgetItem *tagsPage = addPage( tagsWidget, i18n("Tags") );
    tagsPage->setIcon( KIcon( "feed-subscribe" ) );

    // the grid for all widgets in the main window
    QGridLayout* tagsGridLayout = new QGridLayout( tagsWidget );

    // add the inputs
    // add a horizontal box layout for the title and track number
    QHBoxLayout *titleBox = new QHBoxLayout();
    tagsGridLayout->addLayout( titleBox, 0, 1 );
    // and fill it up
    lTitleLabel = new QLabel( i18n("Title:"), tagsWidget );
    tagsGridLayout->addWidget( lTitleLabel, 0, 0 );
    lTitle = new KLineEdit( tagsWidget );
    titleBox->addWidget( lTitle );
//     connect( lTitle, SIGNAL(textChanged(const QString&)), this, SLOT(titleChanged(const QString&)) );
    pTitleEdit = new KPushButton( " ", tagsWidget );
    pTitleEdit->setIcon( KIcon("edit-rename") );
    pTitleEdit->setFixedSize( lTitle->sizeHint().height(), lTitle->sizeHint().height() );
    pTitleEdit->hide();
    titleBox->addWidget( pTitleEdit );
//     connect( pTitleEdit, SIGNAL(clicked()), this, SLOT(editTitleClicked()) );
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
//     connect( pNumberEdit, SIGNAL(clicked()), this, SLOT(editNumberClicked()) );

    // add a horizontal box layout for the artist and the composer
    QHBoxLayout *artistBox = new QHBoxLayout();
    tagsGridLayout->addLayout( artistBox, 1, 1 );
    // and fill it up
    lArtistLabel = new QLabel( i18n("Artist:"), tagsWidget );
    tagsGridLayout->addWidget( lArtistLabel, 1, 0 );
    lArtist = new KLineEdit( tagsWidget );
    artistBox->addWidget( lArtist );
//     connect( lArtist, SIGNAL(textChanged(const QString&)), this, SLOT(artistChanged(const QString&)) );
    pArtistEdit = new KPushButton( " ", tagsWidget );
    pArtistEdit->setIcon( KIcon("edit-rename") );
    pArtistEdit->setFixedSize( lArtist->sizeHint().height(), lArtist->sizeHint().height() );
    pArtistEdit->hide();
    artistBox->addWidget( pArtistEdit );
//     connect( pArtistEdit, SIGNAL(clicked()), this, SLOT(editArtistClicked()) );
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
//     connect( pComposerEdit, SIGNAL(clicked()), this, SLOT(editComposerClicked()) );

    // add a horizontal box layout for the album
    QHBoxLayout *albumBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumBox, 2, 1 );
    // and fill it up
    lAlbumLabel = new QLabel( i18n("Album:"), tagsWidget );
    tagsGridLayout->addWidget( lAlbumLabel, 2, 0 );
    lAlbum = new KLineEdit( tagsWidget );
    albumBox->addWidget( lAlbum );
//     connect( lAlbum, SIGNAL(textChanged(const QString&)), this, SLOT(albumChanged(const QString&)) );
    pAlbumEdit = new KPushButton( " ", tagsWidget );
    pAlbumEdit->setIcon( KIcon("edit-rename") );
    pAlbumEdit->setFixedSize( lAlbum->sizeHint().height(), lAlbum->sizeHint().height() );
    pAlbumEdit->hide();
    albumBox->addWidget( pAlbumEdit );
//     connect( pAlbumEdit, SIGNAL(clicked()), this, SLOT(editAlbumClicked()) );

    // add a horizontal box layout for the disc number, year and genre
    QHBoxLayout *albumdataBox = new QHBoxLayout();
    tagsGridLayout->addLayout( albumdataBox, 3, 1 );
    // and fill it up
    lDiscLabel = new QLabel( i18n("Disc No.:"), tagsWidget );
    tagsGridLayout->addWidget( lDiscLabel, 3, 0 );
    iDisc = new KIntSpinBox( 0, 99, 1, 1, tagsWidget );
    albumdataBox->addWidget( iDisc );
//     connect( iDisc, SIGNAL(valueChanged(int)), this, SLOT(discChanged(int)) );
    pDiscEdit = new KPushButton( " ", tagsWidget );
    pDiscEdit->setIcon( KIcon("edit-rename") );
    pDiscEdit->setFixedSize( iDisc->sizeHint().height(), iDisc->sizeHint().height() );
    pDiscEdit->hide();
    albumdataBox->addWidget( pDiscEdit );
//     connect( pDiscEdit, SIGNAL(clicked()), this, SLOT(editDiscClicked()) );
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
//     connect( pYearEdit, SIGNAL(clicked()), this, SLOT(editYearClicked()) );
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
//     connect( pGenreEdit, SIGNAL(clicked()), this, SLOT(editGenreClicked()) );

    // add a horizontal box layout for the comment
    QHBoxLayout *commentBox = new QHBoxLayout();
    tagsGridLayout->addLayout( commentBox, 4, 1 );
    // and fill it up
    lCommentLabel = new QLabel( i18n("Comment:"), tagsWidget );
    tagsGridLayout->addWidget( lCommentLabel, 4, 0 );
    tComment = new KTextEdit( tagsWidget );
    commentBox->addWidget( tComment );
//     connect( tComment, SIGNAL(textChanged()), this, SLOT(commentChanged()) );
    pCommentEdit = new KPushButton( " ", tagsWidget );
    pCommentEdit->setIcon( KIcon("edit-rename") );
    pCommentEdit->setFixedSize( lTitle->sizeHint().height(), lTitle->sizeHint().height() );
    pCommentEdit->hide();
    commentBox->addWidget( pCommentEdit );
//     connect( pCommentEdit, SIGNAL(clicked()), this, SLOT(editCommentClicked()) );
    tagsGridLayout->setRowStretch( 4, 1 );

    lEditTags = new QLabel( "", tagsWidget );
    tagsGridLayout->addWidget( lEditTags, 5, 1 );
    lEditTags->setAlignment( Qt::AlignHCenter );
    lEditTags->hide();
    pEditTags = new KPushButton( i18n("Edit tags"), tagsWidget );
    pEditTags->setFixedWidth( pEditTags->sizeHint().width() );
    tagsGridLayout->addWidget( pEditTags, 6, 1, Qt::AlignHCenter );
    pEditTags->hide();
    connect( pEditTags, SIGNAL(clicked()), this, SLOT(editTagsClicked()) );



    QWidget *infoWidget = new QWidget( this );
    KPageWidgetItem *infoPage = addPage( infoWidget, i18n("Infos") );
    infoPage->setIcon( KIcon( "dialog-information" ) );
    QGridLayout *infoGridLayout = new QGridLayout( infoWidget );
    lPipes = new QLabel( "", infoWidget );
    infoGridLayout->addWidget( lPipes, 0, 0 );
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

    if( !enabled ) {
        lTitle->setText( "" );
        iNumber->setValue( 0 );
        lArtist->setText( "" );
        lComposer->setText( "" );
        lAlbum->setText( "" );
        iDisc->setValue( 0 );
        iYear->setValue( 0 );
        cGenre->setEditText( "" );
        tComment->setText( "" );
    }
}

// FIXME maybe buggy
void OptionsEditor::itemsSelected( QList<FileListItem*> items )
{
    applyChanges();
    
    for( QList<FileListItem*>::Iterator it = items.begin(); it != items.end(); ) {
        if( (*it)->converting || (*it) == 0 ) it = items.erase( it );
        else it++;
    }

    selectedItems = items;

    if( selectedItems.count() == 0 ) {
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

    if( selectedItems.count() == 1 ) {
        setCaption( selectedItems.first()->url.fileName() );
        // HACK ...but seems to work...
        // FIXME directory does get set properly
//         disconnect( options, SIGNAL(optionsChanged()), 0, 0 );
//         options->setCurrentOptions( items.first()->options );
//         connect( options, SIGNAL(optionsChanged()), this, SLOT(optionsChanged()) );
        options->setCurrentConversionOptions( config->conversionOptionsManager()->getConversionOptions(selectedItems.first()->conversionOptionsId) );


        // info tab
        ConversionOptions *conversionOptions = config->conversionOptionsManager()->getConversionOptions(items.first()->conversionOptionsId);
        lPipes->setText("");
        if( conversionOptions )
        {
            QString inputCodec = config->pluginLoader()->getCodecFromFile( items.first()->url );
            QList<ConversionPipe> pipes = config->pluginLoader()->getConversionPipes( inputCodec, conversionOptions->codecName, conversionOptions->pluginName );
            for( int i=0; i<pipes.size(); i++ )
            {
                if( pipes.at(i).trunks.count() == 1 )
                {
                    lPipes->setText( lPipes->text() + QString::number(i+1) + ": " + pipes.at(i).trunks.at(0).plugin->name() + " (rating: " + QString::number(pipes.at(i).trunks.at(0).rating) + ")\n" );
                }
                else if( pipes.at(i).trunks.count() == 2 )
                {
                    lPipes->setText( lPipes->text() + QString::number(i+1) + ": " + pipes.at(i).trunks.at(0).plugin->name() + " > " + pipes.at(i).trunks.at(1).plugin->name() + " (rating: " + QString::number(pipes.at(i).trunks.at(0).rating) + " & " + QString::number(pipes.at(i).trunks.at(1).rating) + ")\n" );
                }
            }
        }


        if( items.first()->tags == 0 && !items.first()->local ) {
            setTagInputEnabled( false );
            lEditTags->setText( i18n("The tags could not be read, because this is not a local file.\n"
                    "soundKonverter will try to read the tags, when it is about to convert the file.\n"
                    "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                    "to read the tags after downloading.") );
            lEditTags->show();
            pEditTags->show();
        }
        else if( items.first()->tags == 0 ) {
            setTagInputEnabled( false );
            lEditTags->setText( i18n("Reading the tags of this file failed.\n"
                    "soundKonverter will try to read the tags a second time, when it is about to convert the file.\n"
                    "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                    "to read the tags a second time.") );
            lEditTags->show();
            pEditTags->show();
        }
        else {
            lTitle->setText( items.first()->tags->title );
            iNumber->setValue( items.first()->tags->track );
            lArtist->setText( items.first()->tags->artist );
            lComposer->setText( items.first()->tags->composer );
            lAlbum->setText( items.first()->tags->album );
            iDisc->setValue( items.first()->tags->disc );
            iYear->setValue( items.first()->tags->year );
            cGenre->setEditText( items.first()->tags->genre );
            tComment->setText( items.first()->tags->comment );
        }
    }
    else {
        setCaption( i18n("%1 Files").arg(items.count()) );
        QList<FileListItem*>::Iterator it = items.begin();
//         ConversionOptions cOptions = (*it)->options;
        QString title = ( (*it)->tags == 0 ) ? "" : (*it)->tags->title;
        int number = ( (*it)->tags == 0 ) ? 0 : (*it)->tags->track;
        QString artist = ( (*it)->tags == 0 ) ? "" : (*it)->tags->artist;
        QString composer = ( (*it)->tags == 0 ) ? "" : (*it)->tags->composer;
        QString album = ( (*it)->tags == 0 ) ? "" : (*it)->tags->album;
        int disc = ( (*it)->tags == 0 ) ? 0 : (*it)->tags->disc;
        int year = ( (*it)->tags == 0 ) ? 0 : (*it)->tags->year;
        QString genre = ( (*it)->tags == 0 ) ? "" : (*it)->tags->genre;
        QString comment = ( (*it)->tags == 0 ) ? "" : (*it)->tags->comment;
        while( it != items.end() ) {
/*            if( !cOptions.nearlyEqual((*it)->options) ) {
                options->setEnabled( false );
                lEditOptions->setText( i18n("You have selected multiple files with different conversion options.\nYou can change the options of all files by hitting the button below.") );
                lEditOptions->show();
                pEditOptions->show();
            }*/
            if( (*it)->tags == 0 ) {
                setTagInputEnabled( false );
                lEditTags->setText( i18n("Reading the tags of one or more files failed.\n"
                        "soundKonverter will try to read the tags a second time, when it is about to convert the files.\n"
                        "If you want to edit the tags, you can hit the button below but then soundKonverter will not try\n"
                        "to read the tags a second time.") );
                lEditTags->show();
                pEditTags->show();
                it++;
                continue;
            }
            if( title != (*it)->tags->title && lTitle->isEnabled() ) {
                lTitle->setEnabled( false );
                lTitle->setText( "" );
                pTitleEdit->show();
            }
            if( number != (*it)->tags->track && iNumber->isEnabled() ) {
                iNumber->setEnabled( false );
                iNumber->setValue( 1 );
                pNumberEdit->show();
            }
            if( artist != (*it)->tags->artist && lArtist->isEnabled() ) {
                lArtist->setEnabled( false );
                lArtist->setText( "" );
                pArtistEdit->show();
            }
            if( composer != (*it)->tags->composer && lComposer->isEnabled() ) {
                lComposer->setEnabled( false );
                lComposer->setText( "" );
                pComposerEdit->show();
            }
            if( album != (*it)->tags->album && lAlbum->isEnabled() ) {
                lAlbum->setEnabled( false );
                lAlbum->setText( "" );
                pAlbumEdit->show();
            }
            if( disc != (*it)->tags->disc && iDisc->isEnabled() ) {
                iDisc->setEnabled( false );
                iDisc->setValue( 1 );
                pDiscEdit->show();
            }
            if( year != (*it)->tags->year && iYear->isEnabled() ) {
                iYear->setEnabled( false );
                iYear->setValue( QDate::currentDate().year() );
                pYearEdit->show();
            }
            if( genre != (*it)->tags->genre && cGenre->isEnabled() ) {
                cGenre->setEnabled( false );
                cGenre->setEditText( "" );
                pGenreEdit->show();
            }
            if( comment != (*it)->tags->comment && tComment->isEnabled() ) {
                tComment->setEnabled( false );
                tComment->setReadOnly( true );
                tComment->setText( "" );
                pCommentEdit->show();
            }
            it++;
        }
        if( options->isEnabled() ) {
            // HACK ...but seems to work...
            // FIXME directory does get set properly
//             disconnect( options, SIGNAL(optionsChanged()), 0, 0 );
//             options->setCurrentOptions( items.first()->options );
//             connect( options, SIGNAL(optionsChanged()), this, SLOT(optionsChanged()) );
        }
        if( lTitle->isEnabled() ) {
            lTitle->setText( title );
        }
        if( iNumber->isEnabled() ) {
            iNumber->setValue( number );
        }
        if( lArtist->isEnabled() ) {
            lArtist->setText( artist );
        }
        if( lComposer->isEnabled() ) {
            lComposer->setText( composer );
        }
        if( lAlbum->isEnabled() ) {
            lAlbum->setText( album );
        }
        if( iDisc->isEnabled() ) {
            iDisc->setValue( disc );
        }
        if( iYear->isEnabled() ) {
            iYear->setValue( year );
        }
        if( cGenre->isEnabled() ) {
            cGenre->setEditText( genre );
        }
        if( tComment->isEnabled() ) {
            tComment->setText( comment );
        }
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

void OptionsEditor::itemRemoved( FileListItem *item )
{
    selectedItems.removeAll( item );

    itemsSelected( selectedItems );
}

void OptionsEditor::applyChanges()
{
    for( int i=0; i<selectedItems.count(); i++ )
    {
        selectedItems.at(i)->conversionOptionsId = config->conversionOptionsManager()->updateConversionOptions( selectedItems.at(i)->conversionOptionsId, options->currentConversionOptions() );
        if( selectedItems.at(i)->tags )
        {
            selectedItems.at(i)->tags->title = lTitle->text();
            selectedItems.at(i)->tags->track = iNumber->value();
            selectedItems.at(i)->tags->artist = lArtist->text();
            selectedItems.at(i)->tags->composer = lComposer->text();
            selectedItems.at(i)->tags->album = lAlbum->text();
            selectedItems.at(i)->tags->disc = iDisc->value();
            selectedItems.at(i)->tags->year = iYear->value();
            selectedItems.at(i)->tags->genre = cGenre->currentText();
            selectedItems.at(i)->tags->comment = tComment->toPlainText();
        }
    }
    
    emit updateFileListItems( selectedItems );
}

void OptionsEditor::editTagsClicked()
{
    for( int i=0; i<selectedItems.count(); i++ )
    {
        selectedItems.at(i)->tags = new TagData();
    }

    itemsSelected( selectedItems );
}

