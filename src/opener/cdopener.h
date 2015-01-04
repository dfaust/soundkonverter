
#ifndef CDOPENER_H
#define CDOPENER_H

#include "ui_cdopener.h"

#include <QDialog>
#include <QTimer>

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
#include <phonon/mediacontroller.h>

#include <musicbrainz5/NameCreditList.h>

class TagEngine;
class Config;
class ConversionOptions;
class QTreeWidgetItem;
class TagData;
class QLabel;

namespace Ui {
    class CDOpener;
}

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

    QPushButton *pStartPlayback;
    QPushButton *pStopPlayback;
    Phonon::SeekSlider *seekSlider;

private slots:
    void startPlaybackClicked();
    void stopPlaybackClicked();

signals:
    void startPlayback( int track );
    void stopPlayback();
};


/** Shows a dialog for selecting files from a CD */
class CDOpener : public QDialog
{
     Q_OBJECT
public:
    enum Columns {
        Column_Rip      = 0,
        Column_Track    = 1,
        Column_Artist   = 2,
        Column_Composer = 3,
        Column_Title    = 4,
        Column_Length   = 5,
        Column_Player   = 6
    };

    CDOpener( Config *_config, const QString& _device, QWidget *parent = 0, Qt::WindowFlags f=0 );
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
    Ui::CDOpener ui;

    /** returns a list of devices holding audio cds plus a short description (track count) */
    QMap<QString,QString> cdDevices();
    bool openCdDevice( const QString& _device );
    int cdda_audio_tracks( cdrom_drive *cdDrive ) const;

    QString parseNameCredits(const MusicBrainz5::CNameCreditList *names);

    QStringList amazonCoverUrls(const QString& asin);

    /** the widget for showing the progress of reading the cd / cddb data */
    QWidget *cdOpenerOverlayWidget;
    /** Show the progress of reading the cd / cddb data */
    QLabel *lOverlayLabel;

    Phonon::AudioOutput *audioOutput;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaController *mediaController;
    Phonon::MediaSource *mediaSource;

    QList<PlayerWidget*> playerWidgets;

    Config *config;

    QString device;

    cdrom_drive *cdDrive;
    cdrom_paranoia *cdParanoia;

    TagData *discTags;
    QList<TagData*> trackTags;
    bool cddbFound;

    QString lastAlbumArtist;

    QList<int> selectedTracks;

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
