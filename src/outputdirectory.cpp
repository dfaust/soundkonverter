
#include "outputdirectory.h"
// #include "filelist.h"
#include "filelistitem.h"
#include "core/conversionoptions.h"
// #include "tagengine.h"
#include "config.h"

#include <QLayout>
#include <QToolTip>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QRegExp>
#include <QProcess>

#include <KLocale>
#include <KMessageBox>
#include <KFileDialog>
#include <KComboBox>
#include <KLineEdit>
#include <KIcon>
#include <KPushButton>

OutputDirectory::OutputDirectory( Config *_config, QWidget *parent )
    : QWidget( parent ),
    config( _config )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
//     grid->setSpacing( 6 );

    QHBoxLayout *box = new QHBoxLayout( );
    grid->addLayout( box, 0, 0 );

    QLabel *lOutput = new QLabel( i18n("Output")+":", this );
    box->addWidget( lOutput );

    cMode = new KComboBox( this );
    cMode->addItem( i18n("By meta data") );
    cMode->addItem( i18n("Source directory") );
    cMode->addItem( i18n("Specify output directory") );
    cMode->addItem( i18n("Copy directory structure") );
    box->addWidget( cMode );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChangedSlot(int)) );
//     lDir = new KLineEdit( this );
//     box->addWidget( lDir );
//     lDir->setClearButtonShown( true );
//     connect( lDir, SIGNAL(textChanged(const QString&)),  this, SLOT(directoryChangedSlot(const QString&)) );

    cDir = new KComboBox( true, this );
//     cDir->addItems( tagEngine->genreList );
//     cDir->clearEditText();
//     KCompletion *cDirCompletion = cDir->completionObject();
//     cDirCompletion->insertItems( tagEngine->genreList );
    box->addWidget( cDir, 1 );
    connect( cDir, SIGNAL(editTextChanged(const QString&)),  this, SLOT(directoryChangedSlot(const QString&)) );

    pDirSelect = new KPushButton( KIcon("folder"), "", this );
    box->addWidget( pDirSelect );
    pDirSelect->setFixedWidth( pDirSelect->height() );
    pDirSelect->setToolTip( i18n("Choose an output directory") );
    connect( pDirSelect, SIGNAL(clicked()), this, SLOT(selectDir()) );
    pDirGoto = new KPushButton( KIcon("system-file-manager"), "", this );
    box->addWidget( pDirGoto );
    pDirGoto->setFixedWidth( pDirGoto->height() );
    pDirGoto->setToolTip( i18n("Open the output directory with Dolphin") );
    connect( pDirGoto, SIGNAL(clicked()), this, SLOT(gotoDir()) );

    modeJustChanged = false;
    setMode( (OutputDirectory::Mode)config->data.general.lastOutputDirectoryMode );
}

OutputDirectory::~OutputDirectory()
{}

void OutputDirectory::disable()
{
    cMode->setEnabled( false );
    cDir->setEnabled( false );
    pDirSelect->setEnabled( false );
}

void OutputDirectory::enable()
{
    cMode->setEnabled( true );
    modeChangedSlot( cMode->currentIndex() );
}

OutputDirectory::Mode OutputDirectory::mode()
{
    return (Mode)cMode->currentIndex();
}

void OutputDirectory::setMode( OutputDirectory::Mode mode )
{
    cMode->setCurrentIndex( (int)mode );
    updateMode( mode );
}

QString OutputDirectory::directory()
{
    if( (Mode)cMode->currentIndex() != Source ) return cDir->currentText();
    else return "";
}

void OutputDirectory::setDirectory( const QString& directory )
{
    if( (Mode)cMode->currentIndex() != Source ) cDir->setEditText( directory );
}

KUrl OutputDirectory::calcPath( FileListItem *fileListItem, Config *config, QString extension )
{
    // TODO replace '//' by '/' ???
    // FIXME test fvat names
    ConversionOptions *options = config->conversionOptionsManager()->getConversionOptions(fileListItem->conversionOptionsId);
    if( !options ) return KUrl();
    QString path;
    KUrl url;
    if( extension.isEmpty() ) extension = config->pluginLoader()->codecExtensions(options->codecName).at(0);
    if( extension.isEmpty() ) extension = options->codecName;

    QString fileName;
    if( fileListItem->track == -1 )
        fileName = fileListItem->url.fileName();
    else
        fileName =  QString().sprintf("%02i",fileListItem->tags->track) + " - " + fileListItem->tags->artist + " - " + fileListItem->tags->title + "." + extension;

    // if the user wants to change the output directory/file name per file! TODO
//     if( !fileListItem->options.outputFilePathName.isEmpty() ) {
//         path = uniqueFileName( changeExtension(fileListItem->options.outputFilePathName,extension) );
//         if( config->data.general.useVFATNames ) path = vfatPath( path );
//         return path;
//     }

    if( options->outputDirectoryMode == Specify )
    {
        path = options->outputDirectory+"/"+fileName;
        if( config->data.general.useVFATNames ) path = vfatPath( path );
        url = changeExtension( KUrl(path), extension );
        if( config->data.general.conflictHandling == Config::Data::General::NewFileName ) url = uniqueFileName( url );
        return url;
    }
    else if( options->outputDirectoryMode == MetaData )
    {
        path = options->outputDirectory;

        // TODO a little bit redundant, adding %f if file name wasn't set properly
        if( path.right(1) == "/" ) path += "%f";
        else if( path.lastIndexOf(QRegExp("%[abcdfgnpty]")) < path.lastIndexOf("/") ) path += "/%f";
//         else if( path.lastIndexOf(QRegExp("%[aAbBcCdDfFgGnNpPtTyY]{1,1}")) < path.lastIndexOf("/") ) path += "/%f";
        
        const int fileNameBegin = path.lastIndexOf("/");
        if( fileListItem->tags == 0 || ( path.mid(fileNameBegin).contains("%n") && fileListItem->tags->track == 0 ) || ( path.mid(fileNameBegin).contains("%t") && fileListItem->tags->title.isEmpty() ) )
        {
            path = path.left( fileNameBegin ) + "/%f";
        }

        QRegExp reg( "\\[(.*)%([abcdfgnpty])(.*)\\]" );
        reg.setMinimal( true );
        while( path.indexOf(reg) != -1 )
        {
            if( fileListItem->tags &&
                ( reg.cap(2) == "a" && !fileListItem->tags->artist.isEmpty() ) ||
                ( reg.cap(2) == "b" && !fileListItem->tags->album.isEmpty() ) ||
                ( reg.cap(2) == "c" && !fileListItem->tags->comment.isEmpty() ) ||
                ( reg.cap(2) == "d" && fileListItem->tags->disc != 0 ) ||
                ( reg.cap(2) == "g" && !fileListItem->tags->genre.isEmpty() ) ||
                ( reg.cap(2) == "n" && fileListItem->tags->track != 0 ) ||
                ( reg.cap(2) == "p" && !fileListItem->tags->composer.isEmpty() ) ||
                ( reg.cap(2) == "t" && !fileListItem->tags->title.isEmpty() ) ||
                ( reg.cap(2) == "y" && fileListItem->tags->year != 0 ) )
            {
                path.replace( reg, "\\1%\\2\\3" );
            }
            else
            {
                path.replace( reg, "" );
            }
        }
        
        while( path.contains("//") ) path.replace( "//", "/" );

        path.replace( "%a", "$replace_by_artist$" );
        path.replace( "%b", "$replace_by_album$" );
        path.replace( "%c", "$replace_by_comment$" );
        path.replace( "%d", "$replace_by_disc$" );
        path.replace( "%g", "$replace_by_genre$" );
        path.replace( "%n", "$replace_by_track$" );
        path.replace( "%p", "$replace_by_composer$" );
        path.replace( "%t", "$replace_by_title$" );
        path.replace( "%y", "$replace_by_year$" );
        path.replace( "%f", "$replace_by_filename$" );

        QString artist = ( fileListItem->tags == 0 || fileListItem->tags->artist.isEmpty() ) ? i18n("Unknown Artist") : fileListItem->tags->artist;
        artist.replace("/",",");
        path.replace( "$replace_by_artist$", artist );

        QString album = ( fileListItem->tags == 0 || fileListItem->tags->album.isEmpty() ) ? i18n("Unknown Album") : fileListItem->tags->album;
        album.replace("/",",");
        path.replace( "$replace_by_album$", album );

        QString comment = ( fileListItem->tags == 0 || fileListItem->tags->comment.isEmpty() ) ? i18n("No Comment") : fileListItem->tags->comment;
        comment.replace("/",",");
        path.replace( "$replace_by_comment$", comment );

        QString disc = ( fileListItem->tags == 0 ) ? "0" : QString().sprintf("%i",fileListItem->tags->disc);
        path.replace( "$replace_by_disc$", disc );

        QString genre = ( fileListItem->tags == 0 || fileListItem->tags->genre.isEmpty() ) ? i18n("Unknown Genre") : fileListItem->tags->genre;
        genre.replace("/",",");
        path.replace( "$replace_by_genre$", genre );

        QString track = ( fileListItem->tags == 0 ) ? "00" : QString().sprintf("%02i",fileListItem->tags->track);
        path.replace( "$replace_by_track$", track );

        QString composer = ( fileListItem->tags == 0 || fileListItem->tags->composer.isEmpty() ) ? i18n("Unknown Composer") : fileListItem->tags->composer;
        composer.replace("/",",");
        path.replace( "$replace_by_composer$", composer );

        QString title = ( fileListItem->tags == 0 || fileListItem->tags->title.isEmpty() ) ? i18n("Unknown Title") : fileListItem->tags->title;
        title.replace("/",",");
        path.replace( "$replace_by_title$", title );

        QString year = ( fileListItem->tags == 0 ) ? "0000" : QString().sprintf("%04i",fileListItem->tags->year);
        path.replace( "$replace_by_year$", year );

        QString filename = fileName.left( fileName.lastIndexOf(".") );
        filename.replace("/",",");
        path.replace( "$replace_by_filename$", filename );

        if( config->data.general.useVFATNames ) path = vfatPath( path );
        url = KUrl( path + "." + extension );
        if( config->data.general.conflictHandling == Config::Data::General::NewFileName ) url = uniqueFileName( url );
        return url;
    }
/*    else if( options->outputDirectoryMode == CopyStructure )
    {
        QString inputPath = fileListItem->url.pathOrUrl();
        QString outputPath = options->outputDirectory;
        QStringList inputDirs = inputPath.split( "/" );
        QStringList outputDirs = outputPath.split( "/" );
        QStringList baseDirs;
        int minCount = inputDirs.count() < outputDirs.count() ? inputDirs.count() : outputDirs.count();
        int i=0;
        for( ; i<minCount; i++ )
        {
            baseDirs += outputDirs.at(i);
            if( inputDirs.at(i) != outputDirs.at(i) )
            {
                break;
            }
        }
        baseDirs += inputDirs.mid( i + 1 );
        path = baseDirs.join( "/" );
        if( config->data.general.useVFATNames ) path = vfatPath( path );
        url = uniqueFileName( changeExtension(KUrl(path),extension) );
        return url;
        
        /*
        QString basePath = options->outputDirectory;
        QString originalPath = fileListItem->url.pathOrUrl();
        QString cutted;
        while( basePath.length() > 0 ) {
            if( fileListItem->url.pathOrUrl().indexOf(basePath) == 0 ) {
                originalPath.replace( basePath, "" );
                return uniqueFileName( changeExtension(KUrl(basePath+cutted+originalPath),extension) );
            }
            else {
                cutted = basePath.right( basePath.length() - basePath.lastIndexOf("/") ) + cutted;
                basePath = basePath.left( basePath.lastIndexOf("/") );
            }
        }
        url = uniqueFileName( changeExtension(KUrl(options->outputDirectory+"/"+fileListItem->url.pathOrUrl()),extension) );
//         if( config->data.general.useVFATNames ) path = vfatPath( path );
        return url;
        *
    }*/
    else if( options->outputDirectoryMode == CopyStructure )
    {
        QString basePath = options->outputDirectory;
        QString originalPath = fileListItem->url.pathOrUrl();
        int cutpos = basePath.length();
        while( basePath.left(cutpos) != originalPath.left(cutpos) )
        {
            cutpos = basePath.lastIndexOf( '/', cutpos - 1 );
        }
        // At this point, basePath and originalPath overlap on the left for cutpos characters (which might be 0).
        if( config->data.general.useVFATNames ) path = vfatPath( path );
        url = changeExtension( KUrl(basePath+originalPath.mid(cutpos)), extension );
        if( config->data.general.conflictHandling == Config::Data::General::NewFileName ) url = uniqueFileName( url );
        return url;
    }
    else // SourceDirectory
    {
        path = fileListItem->url.toLocalFile();
        if( config->data.general.useVFATNames ) path = vfatPath( path );
        url = changeExtension( KUrl(path), extension );
        if( config->data.general.conflictHandling == Config::Data::General::NewFileName ) url = uniqueFileName( url );
        return url;
    }
}

KUrl OutputDirectory::changeExtension( const KUrl& url, const QString& extension )
{
    KUrl newUrl = url;
    
    QString fileName = newUrl.fileName();
    fileName = newUrl.fileName().left( newUrl.fileName().lastIndexOf(".")+1 ) + extension;
    
    newUrl.setFileName( fileName );

    return newUrl;
}

KUrl OutputDirectory::uniqueFileName( const KUrl& url, const QStringList& usedOutputNames )
{
/*    QFileInfo fileInfo( url.toLocalFile() );

    // generate an unique file name
    while( fileInfo.exists() )
    {
//         fileInfo.setFile( fileInfo.filePath().left( fileInfo.filePath().lastIndexOf(".")+1 ) + i18n("new") + fileInfo.filePath().right( fileInfo.filePath().length() - fileInfo.filePath().lastIndexOf(".") ) );
        fileInfo.setFile( fileInfo.filePath().left( fileInfo.filePath().lastIndexOf(".")+1 ) + i18n("new") + fileInfo.filePath().mid( fileInfo.filePath().lastIndexOf(".") ) );
    }
    
    return KUrl( fileInfo.filePath().replace( "//", "/" ) );*/
    
    KUrl uniqueUrl = url;
    
    while( QFile::exists(uniqueUrl.toLocalFile()) || usedOutputNames.contains(uniqueUrl.toLocalFile()) )
    {
        QString fileName = uniqueUrl.fileName();
        fileName = fileName.left( fileName.lastIndexOf(".")+1 ) + i18n("new") + fileName.mid( fileName.lastIndexOf(".") );
        uniqueUrl.setFileName( fileName );
    }

    return uniqueUrl;
}

KUrl OutputDirectory::makePath( const KUrl& url )
{
    QFileInfo fileInfo( url.toLocalFile() );

    QStringList dirs = fileInfo.absoluteDir().absolutePath().split( "/" );
    QString mkDir;
    QDir dir;
    for( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
        mkDir += "/" + *it;
        dir.setPath( mkDir );
        if( !dir.exists() ) dir.mkdir( mkDir );
    }

    return url;
}

// from amarok 2.3.0
// copyright            : (C) 2010 by Amarok developers
// web                  : amarok.kde.org
QString OutputDirectory::vfatPath( const QString& path )
{
    QString s = path;

    if( QDir::separator() == '/' ) // we are on *nix, \ is a valid character in file or directory names, NOT the dir separator
        s.replace( '\\', '_' );
    else
        s.replace( '/', '_' ); // on windows we have to replace / instead

        for( int i = 0; i < s.length(); i++ )
        {
            QChar c = s[ i ];
            if( c < QChar(0x20) || c == QChar(0x7F) // 0x7F = 127 = DEL control character
                || c=='*' || c=='?' || c=='<' || c=='>'
                || c=='|' || c=='"' || c==':' )
                c = '_';
            else if( c == '[' )
                c = '(';
            else if ( c == ']' )
                c = ')';
            s[ i ] = c;
        }

    /* beware of reserved device names */
    uint len = s.length();
    if( len == 3 || (len > 3 && s[3] == '.') )
    {
        QString l = s.left(3).toLower();
        if( l=="aux" || l=="con" || l=="nul" || l=="prn" )
            s = '_' + s;
    }
    else if( len == 4 || (len > 4 && s[4] == '.') )
    {
        QString l = s.left(3).toLower();
        QString d = s.mid(3,1);
        if( (l=="com" || l=="lpt") &&
                (d=="0" || d=="1" || d=="2" || d=="3" || d=="4" ||
                    d=="5" || d=="6" || d=="7" || d=="8" || d=="9") )
            s = '_' + s;
    }

    // "clock$" is only allowed WITH extension, according to:
    // http://en.wikipedia.org/w/index.php?title=Filename&oldid=303934888#Comparison_of_file_name_limitations
    if( QString::compare( s, "clock$", Qt::CaseInsensitive ) == 0 )
        s = '_' + s;

    /* max path length of Windows API */
    s = s.left(255);

    /* whitespace at the end of folder/file names or extensions are bad */
    len = s.length();
    if( s[len-1] == ' ' )
        s[len-1] = '_';

    int extensionIndex = s.lastIndexOf( '.' ); // correct trailing spaces in file name itself
    if( ( s.length() > 1 ) &&  ( extensionIndex > 0 ) )
        if( s.at( extensionIndex - 1 ) == ' ' )
            s[extensionIndex - 1] = '_';

    for( int i = 1; i < s.length(); i++ ) // correct trailing whitespace in folder names
    {
        if( ( s.at( i ) == QDir::separator() ) && ( s.at( i - 1 ) == ' ' ) )
            s[i - 1] = '_';
    }

    return s;
}


// copyright            : (C) 2002 by Mark Kretschmann
// email                : markey@web.de
// modified             : 2006 Daniel Faust <hessijames@gmail.com>
// QString OutputDirectory::vfatPath( const QString& path )
// {
//     QString s = path.right( path.length() - path.findRev("/") - 1 );
//     QString p = path.left( path.findRev("/") + 1 );
// 
//     for( uint i = 0; i < s.length(); i++ )
//     {
//         QChar c = s.ref( i );
//         if( c < QChar(0x20)
//                 || c=='*' || c=='?' || c=='<' || c=='>'
//                 || c=='|' || c=='"' || c==':' || c=='/'
//                 || c=='\\' )
//             c = '_';
//         s.ref( i ) = c;
//     }
// 
//     uint len = s.length();
//     if( len == 3 || (len > 3 && s[3] == '.') )
//     {
//         QString l = s.left(3).lower();
//         if( l=="aux" || l=="con" || l=="nul" || l=="prn" )
//             s = "_" + s;
//     }
//     else if( len == 4 || (len > 4 && s[4] == '.') )
//     {
//         QString l = s.left(3).lower();
//         QString d = s.mid(3,1);
//         if( (l=="com" || l=="lpt") &&
//                 (d=="0" || d=="1" || d=="2" || d=="3" || d=="4" ||
//                     d=="5" || d=="6" || d=="7" || d=="8" || d=="9") )
//             s = "_" + s;
//     }
// 
//     while( s.startsWith( "." ) )
//         s = s.mid(1);
// 
//     while( s.endsWith( "." ) )
//         s = s.left( s.length()-1 );
// 
//     s = s.left(255);
//     len = s.length();
//     if( s[len-1] == ' ' )
//         s[len-1] = '_';
// 
//     return p + s;
// }

void OutputDirectory::selectDir()
{
    QString dir = cDir->currentText();
    QString startDir = dir;
    QString params;
    int i = dir.indexOf( QRegExp("%[aAbBcCdDfFgGnNpPtTyY]{1,1}") );
    if( i != -1 && cMode->currentIndex() == 0 )
    {
        i = dir.lastIndexOf( "/", i );
        startDir = dir.left( i );
        params = dir.mid( i );
    }

    QString directory = KFileDialog::getExistingDirectory( startDir, this, i18n("Choose an output directory") );
    if( !directory.isEmpty() )
    {
        if( i != -1 && cMode->currentIndex() == 0 )
        {
            cDir->setEditText( directory + params );
        }
        else
        {
            cDir->setEditText( directory );
        }
        emit directoryChanged( cDir->currentText() );
    }
}

void OutputDirectory::gotoDir()
{
    QString startDir = cDir->currentText();
    int i = startDir.indexOf( QRegExp("%[aAbBcCdDfFgGnNpPtTyY]{1,1}") );
    if( i != -1 )
    {
        i = startDir.lastIndexOf( "/", i );
        startDir = startDir.left( i );
    }
    
    QProcess::startDetached( "dolphin", QStringList(startDir) );
}

void OutputDirectory::modeChangedSlot( int mode )
{
    modeJustChanged = true;

    updateMode( (Mode)mode );

    emit modeChanged( mode );

    modeJustChanged = false;
}

void OutputDirectory::updateMode( Mode mode )
{
    if( mode == MetaData ) {
//         if( config->data.general.metaDataOutputDirectory.isEmpty() ) config->data.general.metaDataOutputDirectory = QDir::homeDirPath() + "/soundKonverter/%b/%d - %n - %a - %t";
        cDir->setEditText( config->data.general.metaDataOutputDirectory );
        cDir->setEnabled( true );
        pDirSelect->setEnabled( true );
        pDirGoto->setEnabled( true );
        cMode->setToolTip( i18n("Name all converted files according to the specified pattern") );
//         cDir->setToolTip( i18n("<p>The following strings are wildcards, that will be replaced by the information in the meta data:</p><p>%a - Artist<br>%b - Album<br>%c - Comment<br>%d - Disc number<br>%g - Genre<br>%n - Track number<br>%p - Composer<br>%t - Title<br>%y - Year<br>%f - Original file name<p>") );
        cDir->setToolTip( i18n("The following strings are wildcards, that will be replaced\nby the information in the meta data:\n\n%a - Artist\n%b - Album\n%c - Comment\n%d - Disc number\n%g - Genre\n%n - Track number\n%p - Composer\n%t - Title\n%y - Year\n%f - Original file name") );
    }
    else if( mode == Source ) {
        cDir->setEditText( "" );
        cDir->setEnabled( false );
        pDirSelect->setEnabled( false );
        pDirGoto->setEnabled( false );
        cMode->setToolTip( i18n("Output all converted files into the same directory as the original files") );
        cDir->setToolTip("");
    }
    else if( mode == Specify ) {
//         if( config->data.general.specifyOutputDirectory.isEmpty() ) config->data.general.specifyOutputDirectory = QDir::homeDirPath() + "/soundKonverter";
        cDir->setEditText( config->data.general.specifyOutputDirectory );
        cDir->setEnabled( true );
        pDirSelect->setEnabled( true );
        pDirGoto->setEnabled( true );
        cMode->setToolTip( i18n("Output all converted files into the specified output directory") );
        cDir->setToolTip("");
    }
    else if( mode == CopyStructure ) {
//         if( config->data.general.copyStructureOutputDirectory.isEmpty() ) config->data.general.copyStructureOutputDirectory = QDir::homeDirPath() + "/soundKonverter";
        cDir->setEditText( config->data.general.copyStructureOutputDirectory );
        cDir->setEnabled( true );
        pDirSelect->setEnabled( true );
        pDirGoto->setEnabled( true );
        cMode->setToolTip( i18n("Copy the whole directory structure for all converted files") );
        cDir->setToolTip("");
    }
}

void OutputDirectory::directoryChangedSlot( const QString& directory )
{
    if( modeJustChanged ) {
        modeJustChanged = false;
        return;
    }

    Mode mode = (Mode)cMode->currentIndex();

    if( mode == MetaData ) {
        config->data.general.metaDataOutputDirectory = directory;
    }
    else if( mode == Specify ) {
        config->data.general.specifyOutputDirectory = directory;
    }
    else if( mode == CopyStructure ) {
        config->data.general.copyStructureOutputDirectory = directory;
    }

    emit directoryChanged( directory );
}

/*void OutputDirectory::modeInfo()
{
    int mode = cMode->currentItem();
    QString sModeString = cMode->currentText();

    if( (Mode)mode == Default ) {
        KMessageBox::information( this,
            i18n("This will output each file into the soundKonverter default directory."),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
    else if( (Mode)mode == Source ) {
        KMessageBox::information( this,
            i18n("This will output each file into the same directory as the original file."),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
    else if( (Mode)mode == Specify ) {
        KMessageBox::information( this,
            i18n("This will output each file into the directory specified in the editbox behind."),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
    else if( (Mode)mode == MetaData ) {
        KMessageBox::information( this,
            i18n("This will output each file into a directory, which is created based on the metadata in the audio files. Select a directory, where the new directories should be created."),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
    else if( (Mode)mode == CopyStructure ) {
        KMessageBox::information( this,
            i18n("This will output each file into a directory, which is created based on the name of the original directory. So you can copy a whole directory structure, in one you have the original files, in the other the converted."),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
    else {
        KMessageBox::error( this,
            i18n("This mode (%s) doesn't exist.", sModeString),
            QString(i18n("Mode")+": ").append(sModeString) );
    }
}*/

/*void OutputDirectory::dirInfo()
{
    KMessageBox::information( this,
        i18n("<p>The following strings are space holders, that will be replaced by the information in the metatags.</p><p>%a - Artist<br>%b - Album<br>%c - Comment<br>%d - Disc number<br>%g - Genre<br>%n - Track number<br>%p - Composer<br>%t - Title<br>%y - Year<br>%f - Original file name<p>"),
        QString(i18n("Legend")) );
}*/

