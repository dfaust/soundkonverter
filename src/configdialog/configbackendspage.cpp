//
// C++ Implementation: configbackendspage
//
// Description: 
//
//
// Author: Daniel Faust <hessijames@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configbackendspage.h"
#include "../config.h"
#include "../core/codecplugin.h"

#include <QLayout>
#include <QLabel>
#include <KComboBox>
#include <KLocale>
#include <KPushButton>
#include <QCheckBox>
#include <KIcon>
#include <QListWidget>
#include <QToolButton>
#include <KMessageBox>


BackendsListWidget::BackendsListWidget( const QString& _name, Config *_config, QWidget *parent )
    : QGroupBox( _name, parent ),
    config( _config ),
    name( _name )
{
    QVBoxLayout *box = new QVBoxLayout( 0 );
    setLayout( box );
    lBackends = new QListWidget( this );
    connect( lBackends, SIGNAL(currentRowChanged(int)), this, SLOT(itemSelected(int)) );
    box->addWidget( lBackends );
    QHBoxLayout *arrowBox = new QHBoxLayout( 0 );
    box->addLayout( arrowBox );
    pUp = new QToolButton( this );
    pUp->setIcon( KIcon("arrow-up") );
    pUp->setAutoRaise( true );
    pUp->setEnabled( false );
    connect( pUp, SIGNAL(clicked()), this, SLOT(up()) );
    arrowBox->addWidget( pUp );
    pDown = new QToolButton( this );
    pDown->setIcon( KIcon("arrow-down") );
    pDown->setAutoRaise( true );
    pDown->setEnabled( false );
    connect( pDown, SIGNAL(clicked()), this, SLOT(down()) );
    arrowBox->addWidget( pDown );
    pConfigure = new QToolButton( this );
    pConfigure->setIcon( KIcon("configure") );
    pConfigure->setAutoRaise( true );
    pConfigure->setEnabled( false );
    connect( pConfigure, SIGNAL(clicked()), this, SLOT(configure()) );
    arrowBox->addWidget( pConfigure );
    pInfo = new QToolButton( this );
    pInfo->setIcon( KIcon("help-about") );
    pInfo->setAutoRaise( true );
    pInfo->setEnabled( false );
    arrowBox->addWidget( pInfo );
    connect( pInfo, SIGNAL(clicked()), this, SLOT(info()) );
}

BackendsListWidget::~BackendsListWidget()
{}

void BackendsListWidget::setFormat( const QString& _format )
{
    format = _format;
}

void BackendsListWidget::addItem( const QString& item )
{
    lBackends->addItem( item );
}

void BackendsListWidget::clear()
{
    lBackends->clear();
    pUp->setEnabled( false );
    pDown->setEnabled( false );
    pConfigure->setEnabled( false );
    pInfo->setEnabled( false );
    originalOrder.clear();
}

QStringList BackendsListWidget::getList()
{
    QStringList list;

    for( int i=0; i<lBackends->count(); i++ )
    {
        list.append( lBackends->item(i)->text() );
    }

    return list;
}

bool BackendsListWidget::changed()
{
    return originalOrder != getList();
}

void BackendsListWidget::resetOrder()
{
    originalOrder = getList();
}

void BackendsListWidget::itemSelected( int row )
{
    const QListWidgetItem *item = lBackends->item( row );
    if( !item ) return;
    CodecPlugin *plugin = config->pluginLoader()->codecPluginByName( item->text() );

    pUp->setEnabled( row > 0 );
    pDown->setEnabled( row < lBackends->count()-1 );
    if( plugin )
    {
        if( name == i18n("Decoder") ) pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::Decoder,format) );
        else if( name == i18n("Encoder") ) pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::Encoder,format) );
        else if( name == i18n("Replay Gain") ) pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::ReplayGain,format) );
        pInfo->setEnabled( plugin->hasInfo() );
    }
}

void BackendsListWidget::up()
{
    const int row = lBackends->currentRow();
    lBackends->insertItem( row-1, lBackends->takeItem(row) );
    lBackends->setCurrentRow( row-1 );
    emit orderChanged();
}

void BackendsListWidget::down()
{
    const int row = lBackends->currentRow();
    lBackends->insertItem( row+1, lBackends->takeItem(row) );
    lBackends->setCurrentRow( row+1 );
    emit orderChanged();
}

void BackendsListWidget::configure()
{
    const int row = lBackends->currentRow();
    const QListWidgetItem *item = lBackends->item( row );
    if( !item ) return;
    CodecPlugin *plugin = config->pluginLoader()->codecPluginByName( item->text() );

    if( plugin )
    {
        if( name == i18n("Decoder") ) plugin->showConfigDialog( CodecPlugin::Decoder, format, this );
        else if( name == i18n("Encoder") ) plugin->showConfigDialog( CodecPlugin::Encoder, format, this );
        else if( name == i18n("Replay Gain") ) plugin->showConfigDialog( CodecPlugin::ReplayGain, format, this );
    }
}

void BackendsListWidget::info()
{
    const int row = lBackends->currentRow();
    const QListWidgetItem *item = lBackends->item( row );
    if( !item ) return;
    CodecPlugin *plugin = config->pluginLoader()->codecPluginByName( item->text() );

    if( plugin )
    {
        plugin->showInfo( this );
    }
}

// class ConfigBackendsPage
///////////////////////////

ConfigBackendsPage::ConfigBackendsPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );
    box->setSpacing( 0 );

    QGroupBox *ripperGroup = new QGroupBox( i18n("CD ripper"), this );
    box->addWidget( ripperGroup );
    QHBoxLayout *ripperBox = new QHBoxLayout( 0 );
    ripperGroup->setLayout( ripperBox );
    QLabel *lRipper = new QLabel( i18n("Backend")+":", this );
    ripperBox->addWidget( lRipper );
    cRipper = new KComboBox( this );
    cRipper->addItems( config->data.backends.rippers );
    ripperBox->addWidget( cRipper );
    connect( cRipper, SIGNAL(activated(int)), this, SIGNAL(configChanged()) );

    box->addSpacing( 11 );

    QHBoxLayout *formatSelectorBox = new QHBoxLayout( 0 );
    box->addLayout( formatSelectorBox );
    QLabel *lSelectorFormat = new QLabel( i18n("Format")+":", this );
    formatSelectorBox->addWidget( lSelectorFormat );
    formatSelectorBox->addSpacing( 5 );
    cSelectorFormat = new KComboBox( this );
    cSelectorFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain),PluginLoader::CompressionType(PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    cSelectorFormat->removeItem( cSelectorFormat->findText("wav") );
    cSelectorFormat->removeItem( cSelectorFormat->findText("audio cd") );
    formatSelectorBox->addWidget( cSelectorFormat );
    connect( cSelectorFormat, SIGNAL(activated(const QString&)), this, SLOT(formatChanged(const QString&)) );
    formatSelectorBox->addStretch();

    formatGroup = new QGroupBox( this );
    box->addWidget( formatGroup );
    QHBoxLayout *formatBox = new QHBoxLayout( 0 );
    formatGroup->setLayout( formatBox );
    decoderList = new BackendsListWidget( i18n("Decoder"), config, this );
    formatBox->addWidget( decoderList );
    connect( decoderList, SIGNAL(orderChanged()), this, SIGNAL(configChanged()) );
    encoderList = new BackendsListWidget( i18n("Encoder"), config, this );
    formatBox->addWidget( encoderList );
    connect( encoderList, SIGNAL(orderChanged()), this, SIGNAL(configChanged()) );
    replaygainList = new BackendsListWidget( i18n("Replay Gain"), config, this );
    formatBox->addWidget( replaygainList );
    connect( replaygainList, SIGNAL(orderChanged()), this, SIGNAL(configChanged()) );

    box->addStretch();

    formatChanged( cSelectorFormat->currentText() );
}


ConfigBackendsPage::~ConfigBackendsPage()
{}

void ConfigBackendsPage::formatChanged( const QString& format )
{
    QStringList plugins;

    if( decoderList->changed() || encoderList->changed() || replaygainList->changed() )
    {
        int ret = KMessageBox::questionYesNo( this, i18n("You have changed the current settings.\nDo you want to save them?"), i18n("Settings changed") );
        if( ret == KMessageBox::Yes )
        {
            saveSettings();
            config->save();
        }
    }
    
    decoderList->clear();
    encoderList->clear();
    replaygainList->clear();

    currentFormat = format;
    decoderList->setFormat( format );
    encoderList->setFormat( format );
    replaygainList->setFormat( format );

    formatGroup->setTitle( i18n("%1 settings",format) );
    
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == format )
        {
            for( int j=0; j<config->data.backends.codecs.at(i).decoders.count(); j++ )
            {
                decoderList->addItem( config->data.backends.codecs.at(i).decoders.at(j) );
            }
            for( int j=0; j<config->data.backends.codecs.at(i).encoders.count(); j++ )
            {
                encoderList->addItem( config->data.backends.codecs.at(i).encoders.at(j) );
            }
            for( int j=0; j<config->data.backends.codecs.at(i).replaygain.count(); j++ )
            {
                replaygainList->addItem( config->data.backends.codecs.at(i).replaygain.at(j) );
            }
        }
    }

    decoderList->resetOrder();
    encoderList->resetOrder();
    replaygainList->resetOrder();
}

// void ConfigBackendsPage::configureEncoder()
// {
// //     QString encoder = cEncoder->currentText();
// 
// /*    QList<CodecPlugin*> plugins = config->pluginLoader()->encodersForFormat( cSelectorFormat->currentText() );
//     for( int i = 0; i < plugins.count(); i++ ) {
//         if( plugins.at(i)->name() == encoder )
//         {
//             plugins.at(i)->showConfigDialog( cSelectorFormat->currentText(), this );
//         }
//     }*/
// }


void ConfigBackendsPage::resetDefaults()
{

    emit configChanged( true );
}

void ConfigBackendsPage::saveSettings()
{
    for( int i=0; i<config->data.backends.codecs.count(); i++ )
    {
        if( config->data.backends.codecs.at(i).codecName == currentFormat )
        {
            config->data.backends.codecs[i].decoders = decoderList->getList();
            config->data.backends.codecs[i].encoders = encoderList->getList();
            config->data.backends.codecs[i].replaygain = replaygainList->getList();
        }
    }

    decoderList->resetOrder();
    encoderList->resetOrder();
    replaygainList->resetOrder();
}

// void ConfigBackendsPage::orderChanged()
// {
//     emit configChanged( decoderList->changed() || encoderList->changed() || replaygainList->changed() );
// }



