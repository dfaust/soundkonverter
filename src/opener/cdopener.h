
#ifndef CDOPENER_H
#define CDOPENER_H

#include <KDialog>
#include <QTimer>

#include <libkcddb/kcddb.h>
#include <libkcddb/client.h>
#include <libkcddb/cdinfo.h>

extern "C"
{
#include <cdda_interface.h>
#include <cdda_paranoia.h>
}

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <Phonon/MediaController>

class TagEngine;
class Config;
class Options;
class ConversionOptions;
class QTreeWidget;
class KPushButton;
class KLineEdit;
class KComboBox;
class KIntSpinBox;
class KTextEdit;
class QGroupBox;
class QTreeWidgetItem;
class QLabel;
class QCheckBox;
class TagData;


class PlayerWidget : public QWidget
{
     Q_OBJECT
public:
    PlayerWidget( Phonon::MediaObject *mediaObject, int _track, QTreeWidgetItem *_treeWidgetItem, QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~PlayerWidget();

    void trackChanged( int track );
    bool isPlaying() { return playing; }
    QTreeWidgetItem *treeWidgetItem() { return m_treeWidgetItem; }

private:
    int track;
    bool playing;
    QTreeWidgetItem *m_treeWidgetItem;

    KPushButton *pStartPlayback;
    KPushButton *pStopPlayback;
    Phonon::SeekSlider *seekSlider;

private slots:
    void startPlaybackClicked();
    void stopPlaybackClicked();

signals:
    void startPlayback( int track );
    void stopPlayback();
};


/**
 * @short Shows a dialog for selecting files from a CD
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 1.0
 */
class CDOpener : public KDialog
{
     Q_OBJECT
public:
    enum DialogPage {
        CdOpenPage,
        ConversionOptionsPage
    };

    enum Columns {
        Column_Rip      = 0,
        Column_Track    = 1,
        Column_Artist   = 2,
        Column_Composer = 3,
        Column_Title    = 4,
        Column_Length   = 5,
        Column_Player   = 6
    };

    /** Constructor */
    CDOpener( Config *_config, const QString& _device, QWidget *parent = 0, Qt::WFlags f=0 );

    /** Destructor */
    ~CDOpener();

    /** true if no CD was found (don't execute the dialog) */
    bool noCdFound;

public slots:
    /** Set the current profile */
    void setProfile( const QString& profile );

    /** Set the current format */
    void setFormat( const QString& format );

    /** Set the current output directory */
    void setOutputDirectory( const QString& directory );

    /** Set the command to execute after the conversion is complete */
    void setCommand( const QString& _command );

private:
    /** returns a list of devices holding audio cds plus a short description (track count) */
    QMap<QString,QString> cdDevices();
    bool openCdDevice( const QString& _device );
    int cdda_audio_tracks( cdrom_drive *cdDrive ) const;

    /** the widget for selecting and editing the cd tracks */
    QWidget *cdOpenerWidget;
    /** the widget for showing the progress of reading the cd / cddb data */
    QWidget *cdOpenerOverlayWidget;
    /** the conversion options editor widget */
    Options *options;
    /** the current page */
    DialogPage page;

    QLabel *lSelector;
    QLabel *lOptions;

    /** A list of all tracks on the CD */
    QTreeWidget *trackList;

    /** A combobox for entering the artist or selecting VA of the whole CD */
    KLineEdit *lArtist;
    /** A lineedit for entering the album name */
    KLineEdit *lAlbum;
    /** A spinbox for entering or selecting the disc number */
    KIntSpinBox *iDisc;
    /** A spinbox for entering or selecting the total disc number */
    KIntSpinBox *iDiscTotal;
    /** A spinbox for entering or selecting the year of the album */
    KIntSpinBox *iYear;
    /** A combobox for entering or selecting the genre of the album */
    KComboBox *cGenre;

    /** The groupbox shows the selected track numbers */
    QGroupBox *tagGroupBox;

    /** Set the focus of the tag editor to the track over it */
    KPushButton *pTrackUp;
    /** Set the focus of the tag editor to the track under it */
    KPushButton *pTrackDown;

    /** A lineedit for entering the title of track */
    KLineEdit *lTrackTitle;
    KPushButton *pTrackTitleEdit;
    /** A lineedit for entering the artist of a track */
    KLineEdit *lTrackArtist;
    KPushButton *pTrackArtistEdit;
    /** A lineedit for entering the composer of a track */
    KLineEdit *lTrackComposer;
    KPushButton *pTrackComposerEdit;
    /** A textedit for entering a comment for a track */
    KTextEdit *tTrackComment;
    KPushButton *pTrackCommentEdit;

    Phonon::AudioOutput *audioOutput;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaController *mediaController;
    Phonon::MediaSource *mediaSource;

    QList<PlayerWidget*> playerWidgets;

    /** Save the tag information to a cue file */
    KPushButton *pSaveCue;
    /** Request CDDB information */
    KPushButton *pCDDB;
    /** Rip enitre CD as one track */
    QCheckBox *cEntireCd;
    /** Add selected tracks to the file list and quit the dialog */
    KPushButton *pAdd;
    /** proceed to select conversion options */
    KPushButton *pProceed;
    /** Quit the dialog */
    KPushButton *pCancel;

    Config *config;

    QString device;

    cdrom_drive *cdDrive;
    cdrom_paranoia *cdParanoia;

//     void *wmHandle;

    KCDDB::Client *cddb;

    QList<TagData*> tags; // @0 disc tags
    bool cdTextFound;
    bool cddbFound;

    QString lastAlbumArtist;

    QTimer timeoutTimer;

    QList<int> selectedTracks;

    /** Show the progress of reading the cd / cddb data */
    QLabel *lOverlayLabel;

    QTimer fadeTimer;
    float fadeAlpha;
    int fadeMode; // 1 = fade in, 2 = fade out

    QString command;

    inline QBrush brushSetAlpha( QBrush brush, const int alpha )
    {
        QColor color = brush.color();
        color.setAlpha( alpha );
        brush.setColor( color );
        return brush;
    }

    void fadeIn();
    void fadeOut();

    void adjustArtistColumn();
    void adjustComposerColumn();

private slots:
    void requestCddb( bool autoRequest = false );
    void lookup_cddb_done( KCDDB::Result result );
    void timeout();

    void trackChanged();
    void trackUpPressed();
    void trackDownPressed();
    void artistChanged( const QString& text );
    void trackTitleChanged( const QString& text );
    void trackArtistChanged( const QString& text );
    void trackComposerChanged( const QString& text );
    void trackCommentChanged();
    void editTrackTitleClicked();
    void editTrackArtistClicked();
    void editTrackComposerClicked();
    void editTrackCommentClicked();
//     void itemHighlighted( QTreeWidgetItem *item, int column );

    void startPlayback( int track );
    void stopPlayback();
    void playbackTitleChanged( int title );
    void playbackStateChanged( Phonon::State newstate, Phonon::State oldstate );

    void proceedClicked();
    void addClicked();
//     void addAsOneTrackClicked();
    void saveCuesheetClicked();

    void fadeAnim();

signals:
    void addTracks( const QString& device, QList<int> trackList, int tracks, QList<TagData*> tagList, ConversionOptions *conversionOptions, const QString& command );
    void addDisc( const QString& device, ConversionOptions *conversionOptions );
    //void openCuesheetEditor( const QString& content );
};

#endif // CDOPENER_H
