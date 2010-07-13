
#include "aboutplugins.h"

#include <KLocale>
#include <KIcon>
#include <QLayout>
#include <QLabel>
#include <QListView>

#include "config.h"


AboutPlugins::AboutPlugins( Config *_config, QWidget* parent, Qt::WFlags f )
    : KDialog( parent, f ),
    config( _config )
{
    setCaption( i18n("About plugins") );
    setWindowIcon( KIcon("preferences-plugin") );
    setButtons( KDialog::Close );
    setButtonFocus( KDialog::Close );
//     resize( 600, 390 );
    
    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );
    
    QLabel *installedPlugins = new QLabel( i18n("Installed plugins"), this );
    box->addWidget( installedPlugins );
    
    QListView *pluginsList = new QListView( this );
    box->addWidget( pluginsList );
    QList<CodecPlugin*> codecPLugins = config->pluginLoader()->getAllCodecPlugins();

    QLabel *aboutPlugin = new QLabel( "about plugin vorbis tools", this );
    box->addWidget( aboutPlugin );

}

AboutPlugins::~AboutPlugins()
{}


