

#ifndef OUTPUTDIRECTORY_H
#define OUTPUTDIRECTORY_H

#include <QWidget>
#include <QUrl>

class FileListItem;

class Config;
class QComboBox;
class QLineEdit;
class QPushButton;

/**
 * @short The input area for the output directory
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class OutputDirectory : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        MetaData = 0,
        Source = 1,
        Specify = 2,
        CopyStructure = 3
    };

    /** Constructor */
    OutputDirectory( Config *_config, QWidget *parent = 0 );

    /** Destructor */
    ~OutputDirectory();

    Mode mode();
    void setMode( Mode mode );
    QString directory();
    void setDirectory( const QString& directory );
    QString filesystem();

    static QUrl calcPath( FileListItem *fileListItem, Config *config, const QStringList& usedOutputNames = QStringList() );
    static QUrl changeExtension( const QUrl& url, const QString& extension );
    static QUrl uniqueFileName( const QUrl& url, const QStringList& usedOutputNames );
    static QUrl makePath( const QUrl& url );
    static QString vfatPath( const QString& path );
    static QString ntfsPath( const QString& path );

public slots:
    //void setActive( bool );
    void enable();
    void disable();

private slots:
    void modeChangedSlot( int );
    void directoryChangedSlot( const QString& );
    void selectDir();
    void gotoDir();

private:
    void updateMode( Mode );

    static QString filesystemForDirectory( const QString& dir = "" );

    QComboBox *cMode;
    QComboBox *cDir;
    QPushButton *pDirSelect;
    QPushButton *pDirGoto;

    Config *config;

signals:
    void modeChanged( int );
    void directoryChanged( const QString& );
};

#endif // OUTPUTDIRECTORY_H
