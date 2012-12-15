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

#include <QBoxLayout>
#include <KComboBox>
#include <KIcon>
#include <KLocale>
#include <KMessageBox>
#include <KPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QToolButton>


BackendsListWidget::BackendsListWidget( const QString& _name, Config *_config, QWidget *parent )
    : QGroupBox( _name, parent ),
    config( _config ),
    name( _name )
{
    QVBoxLayout *box = new QVBoxLayout();
    setLayout( box );

    lBackends = new QListWidget( this );
    connect( lBackends, SIGNAL(currentRowChanged(int)), this, SLOT(itemSelected(int)) );
    box->addWidget( lBackends );

    QHBoxLayout *arrowBox = new QHBoxLayout();
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
    pUp->setToolTip( "" );
    pDown->setToolTip( "" );
    pConfigure->setToolTip( "" );
    pInfo->setToolTip( "" );
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

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName( item->text() );

    pUp->setEnabled( row > 0 );
    pDown->setEnabled( row < lBackends->count()-1 );

    if( plugin )
    {
        if( name == i18n("Decoder") )
            pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::Decoder,format) );
        else if( name == i18n("Encoder") )
            pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::Encoder,format) );
        else if( name == i18n("Replay Gain") )
            pConfigure->setEnabled( plugin->isConfigSupported(CodecPlugin::ReplayGain,format) );

        pInfo->setEnabled( plugin->hasInfo() );

        const QString pluginName = plugin->name();

        if( pUp->isEnabled() )
            pUp->setToolTip( i18n("Move %1 one position up",pluginName) );
        else
            pUp->setToolTip( "" );

        if( pDown->isEnabled() )
            pDown->setToolTip( i18n("Move %1 one position down",pluginName) );
        else
            pDown->setToolTip( "" );

        if( pInfo->isEnabled() )
            pInfo->setToolTip( i18n("About %1 ...",pluginName) );
        else
            pInfo->setToolTip( "" );

        if( pConfigure->isEnabled() )
            pConfigure->setToolTip( i18n("Configure %1 ...",pluginName) );
        else
            pConfigure->setToolTip( "" );
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

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName( item->text() );

    if( plugin )
    {
        if( name == i18n("Decoder") )
            plugin->showConfigDialog( CodecPlugin::Decoder, format, this );
        else if( name == i18n("Encoder") )
            plugin->showConfigDialog( CodecPlugin::Encoder, format, this );
        else if( name == i18n("Replay Gain") )
            plugin->showConfigDialog( CodecPlugin::ReplayGain, format, this );
    }
}

void BackendsListWidget::info()
{
    const int row = lBackends->currentRow();
    const QListWidgetItem *item = lBackends->item( row );

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName( item->text() );

    if( plugin )
        plugin->showInfo( this );
}

// class ConfigBackendsPage
///////////////////////////

ConfigBackendsPage::ConfigBackendsPage( Config *_config, QWidget *parent )
    : ConfigPageBase( parent ),
    config( _config )
{
    QVBoxLayout *box = new QVBoxLayout( this );

    QFont groupFont;
    groupFont.setBold( true );

    QLabel *lCdRipper = new QLabel( i18n("CD ripper"), this );
    lCdRipper->setFont( groupFont );
    box->addWidget( lCdRipper );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *ripperBox = new QHBoxLayout();
    ripperBox->addSpacing( ConfigDialogOffset );
    box->addLayout( ripperBox );
    QLabel *lSelectorRipper = new QLabel( i18n("Use plugin:"), this );
    ripperBox->addWidget( lSelectorRipper );
    ripperBox->setStretchFactor( lSelectorRipper, 2 );
    cSelectorRipper = new KComboBox( this );
    cSelectorRipper->addItems( config->data.backends.rippers );
    ripperBox->addWidget( cSelectorRipper );
    ripperBox->setStretchFactor( cSelectorRipper, 1 );
    connect( cSelectorRipper, SIGNAL(activated(int)), this, SLOT(somethingChanged()) );
    connect( cSelectorRipper, SIGNAL(activated(const QString&)), this, SLOT(ripperChanged(const QString&)) );
    pConfigureRipper = new KPushButton( KIcon("configure"), "", this );
    pConfigureRipper->setFixedSize( cSelectorRipper->sizeHint().height(), cSelectorRipper->sizeHint().height() );
    pConfigureRipper->setFlat( true );
    ripperBox->addWidget( pConfigureRipper );
    ripperBox->setStretchFactor( pConfigureRipper, 1 );
    connect( pConfigureRipper, SIGNAL(clicked()), this, SLOT(configureRipper()) );

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lFilters = new QLabel( i18n("Filters"), this );
    lFilters->setFont( groupFont );
    box->addWidget( lFilters );

    box->addSpacing( ConfigDialogSpacingSmall );

    QHBoxLayout *filterBox = new QHBoxLayout();
    filterBox->addSpacing( ConfigDialogOffset );
    box->addLayout( filterBox );
    QGridLayout *filterGrid = new QGridLayout();

    int row = 0;
    foreach( const QString filterPluginName, config->data.backends.filters )
    {
        if( row == 0 )
        {
            QLabel *lSelectorFilter = new QLabel( i18n("Enable plugins:"), this );
            filterGrid->addWidget( lSelectorFilter, row, 0 );
        }

        QCheckBox *newCheckBox = new QCheckBox( filterPluginName, this );
        newCheckBox->setChecked( config->data.backends.enabledFilters.contains(filterPluginName) );
        filterGrid->addWidget( newCheckBox, row, 1 );
        filterCheckBoxes.append( newCheckBox );
        connect( newCheckBox, SIGNAL(stateChanged(int)), this, SLOT(somethingChanged()) );

        KPushButton *newConfigButton = new KPushButton( KIcon("configure"), "", this );
        newConfigButton->setFixedSize( cSelectorRipper->sizeHint().height(), cSelectorRipper->sizeHint().height() );
        newConfigButton->setFlat( true );
        filterGrid->addWidget( newConfigButton, row, 2 );
        connect( newConfigButton, SIGNAL(clicked()), this, SLOT(configureFilter()) );
        filterConfigButtons.append( newConfigButton );

        FilterPlugin *plugin = qobject_cast<FilterPlugin*>(config->pluginLoader()->backendPluginByName(filterPluginName));
        if( plugin )
        {
            newConfigButton->setEnabled( plugin->isConfigSupported(BackendPlugin::General,"") );
        }
        else
        {
            newConfigButton->setEnabled( false );
        }

        if( newConfigButton->isEnabled() )
            newConfigButton->setToolTip( i18n("Configure %1 ...",filterPluginName) );

        row++;
    }

    filterGrid->setColumnStretch( 0, 2 );
    filterGrid->setColumnStretch( 1, 1 );
    filterBox->addLayout( filterGrid );

    box->addSpacing( ConfigDialogSpacingBig );

    QLabel *lPriorities = new QLabel( i18n("Priorities"), this );
    lPriorities->setFont( groupFont );
    box->addWidget( lPriorities );

    box->addSpacing( ConfigDialogSpacingSmall );

    QVBoxLayout *formatBox = new QVBoxLayout();
    box->addLayout( formatBox, 1 );

    QHBoxLayout *formatSelectorBox = new QHBoxLayout();
    formatSelectorBox->addSpacing( ConfigDialogOffset );
    formatBox->addLayout( formatSelectorBox );
    QLabel *lSelectorFormat = new QLabel( i18n("Configure plugin priorities for format:"), this );
    formatSelectorBox->addWidget( lSelectorFormat );
    cSelectorFormat = new KComboBox( this );
    cSelectorFormat->addItems( config->pluginLoader()->formatList(PluginLoader::Possibilities(PluginLoader::Encode|PluginLoader::Decode|PluginLoader::ReplayGain),PluginLoader::CompressionType(PluginLoader::InferiorQuality|PluginLoader::Lossy|PluginLoader::Lossless|PluginLoader::Hybrid)) );
    cSelectorFormat->removeItem( cSelectorFormat->findText("wav") );
    cSelectorFormat->removeItem( cSelectorFormat->findText("audio cd") );
    formatSelectorBox->addWidget( cSelectorFormat );
    connect( cSelectorFormat, SIGNAL(activated(const QString&)), this, SLOT(formatChanged(const QString&)) );
    formatSelectorBox->addStretch();

    QHBoxLayout *formatBackendsBox = new QHBoxLayout();
    formatBackendsBox->addSpacing( ConfigDialogOffset );
    formatBox->addLayout( formatBackendsBox );
    decoderList = new BackendsListWidget( i18n("Decoder"), config, this );
    formatBackendsBox->addWidget( decoderList );
    connect( decoderList, SIGNAL(orderChanged()), this, SLOT(somethingChanged()) );
    encoderList = new BackendsListWidget( i18n("Encoder"), config, this );
    formatBackendsBox->addWidget( encoderList );
    connect( encoderList, SIGNAL(orderChanged()), this, SLOT(somethingChanged()) );
    replaygainList = new BackendsListWidget( i18n("Replay Gain"), config, this );
    formatBackendsBox->addWidget( replaygainList );
    connect( replaygainList, SIGNAL(orderChanged()), this, SLOT(somethingChanged()) );

    QHBoxLayout *optimizationsBox = new QHBoxLayout();
    optimizationsBox->addSpacing( ConfigDialogOffset );
    formatBox->addLayout( optimizationsBox );
    optimizationsBox->addStretch();
    pShowOptimizations = new KPushButton( KIcon("games-solve"), i18n("Show possible optimizations"), this );
    optimizationsBox->addWidget( pShowOptimizations );
    connect( pShowOptimizations, SIGNAL(clicked()), this, SLOT(showOptimizations()) );
    optimizationsBox->addStretch();

    box->addStretch( 2 );

    ripperChanged( cSelectorRipper->currentText() );
    formatChanged( cSelectorFormat->currentText() );
}


ConfigBackendsPage::~ConfigBackendsPage()
{}

void ConfigBackendsPage::ripperChanged( const QString& pluginName )
{
    RipperPlugin *plugin = qobject_cast<RipperPlugin*>(config->pluginLoader()->backendPluginByName(pluginName));

    if( plugin )
    {
        pConfigureRipper->setEnabled( plugin->isConfigSupported(BackendPlugin::General,"") );
    }
    else
    {
        pConfigureRipper->setEnabled( false );
    }

    if( pConfigureRipper->isEnabled() )
        pConfigureRipper->setToolTip( i18n("Configure %1 ...",pluginName) );
    else
        pConfigureRipper->setToolTip( "" );
}

void ConfigBackendsPage::formatChanged( const QString& format, bool ignoreChanges )
{
    QStringList plugins;

    if( !ignoreChanges && ( decoderList->changed() || encoderList->changed() || replaygainList->changed() ) )
    {
        const int ret = KMessageBox::questionYesNo( this, i18n("You have changed the current settings.\nDo you want to save them?"), i18n("Settings changed") );
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

    foreach( const Config::CodecData codec, config->data.backends.codecs )
    {
        if( codec.codecName == format )
        {
            foreach( const QString decoder, codec.decoders )
            {
                decoderList->addItem( decoder );
            }
            foreach( const QString encoder, codec.encoders )
            {
                encoderList->addItem( encoder );
            }
            foreach( const QString replaygain, codec.replaygain )
            {
                replaygainList->addItem( replaygain );
            }
        }
    }

    decoderList->resetOrder();
    encoderList->resetOrder();
    replaygainList->resetOrder();

    emit configChanged( false );
}

void ConfigBackendsPage::resetDefaults()
{
    // TODO reset rippers

    int i = 0;
    foreach( QCheckBox *checkBox, filterCheckBoxes )
    {
        const QString filterPluginName = config->data.backends.filters.at(i);

        checkBox->setChecked( i == 0 );

        i++;
    }

    const int answer = KMessageBox::questionYesNo( this, i18n("This will choose the best backends for all formats and save the new preferences immediately.\n\nDo you want to continue?") );

    if( answer == KMessageBox::Yes )
    {
        QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations( true );
        for( int i=0; i<optimizationList.count(); i++ )
        {
            optimizationList[i].solution = CodecOptimizations::Optimization::Fix;
        }
        config->doOptimizations( optimizationList );

        formatChanged( cSelectorFormat->currentText(), true );

        emit configChanged( false );
    }
}

void ConfigBackendsPage::saveSettings()
{
    // TODO save rippers

    config->data.backends.enabledFilters.clear();
    int i = 0;
    foreach( QCheckBox *checkBox, filterCheckBoxes )
    {
        const QString filterPluginName = config->data.backends.filters.at(i);

        if( checkBox->isChecked() )
            config->data.backends.enabledFilters.append( filterPluginName );

        i++;
    }

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

    // ensure that the user won't get bothered by an optimization message at the next start
    QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations( true );
    for( int i=0; i<optimizationList.count(); i++ )
    {
        optimizationList[i].solution = CodecOptimizations::Optimization::Ignore;
    }
    config->data.backendOptimizationsIgnoreList.optimizationList = optimizationList;
}

void ConfigBackendsPage::somethingChanged()
{
    bool changed = false;

    int i = 0;
    foreach( QCheckBox *checkBox, filterCheckBoxes )
    {
        if( checkBox == QObject::sender() )
        {
            const QString filterPluginName = config->data.backends.filters.at(i);

            if( checkBox->isChecked() != config->data.backends.enabledFilters.contains(filterPluginName) )
            {
                changed = true;
                break;
            }
        }
        i++;
    }

    if( decoderList->changed() || encoderList->changed() || replaygainList->changed() )
        changed = true;

    emit configChanged( changed );
}

void ConfigBackendsPage::configureRipper()
{
    RipperPlugin *plugin = qobject_cast<RipperPlugin*>(config->pluginLoader()->backendPluginByName(cSelectorRipper->currentText()));

    if( plugin )
    {
        plugin->showConfigDialog( BackendPlugin::General, "", this );
    }
}

void ConfigBackendsPage::configureFilter()
{
    int i = 0;
    foreach( KPushButton *configButton, filterConfigButtons )
    {
        if( configButton == QObject::sender() )
        {
            const QString filterPluginName = config->data.backends.filters.at(i);

            FilterPlugin *plugin = qobject_cast<FilterPlugin*>(config->pluginLoader()->backendPluginByName(filterPluginName));
            if( plugin )
            {
                plugin->showConfigDialog( BackendPlugin::General, "", this );
            }
        }
        i++;
    }
}

void ConfigBackendsPage::showOptimizations()
{
    QList<CodecOptimizations::Optimization> optimizationList = config->getOptimizations( true );
    if( !optimizationList.isEmpty() )
    {
        CodecOptimizations *optimizationsDialog = new CodecOptimizations( optimizationList, this );
        connect( optimizationsDialog, SIGNAL(solutions(const QList<CodecOptimizations::Optimization>&)), config, SLOT(doOptimizations(const QList<CodecOptimizations::Optimization>&)) );
        optimizationsDialog->exec();
    }
    else
    {
        KMessageBox::information( this, i18n("All backend settings seem to be optimal, there is nothing to do.") );
    }

    formatChanged( cSelectorFormat->currentText(), true );
}


