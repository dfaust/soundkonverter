
#include "backendsgroupbox.h"

#include "core/backendplugin.h"
#include "config.h"

BackendsGroupBox::BackendsGroupBox(QWidget *parent) :
    QGroupBox(parent)
{
    ui.setupUi(this);

    connect(ui.listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(itemSelected(int)));
    connect(ui.moveUpButton, SIGNAL(clicked()), this, SLOT(up()));
    connect(ui.moveDownButton, SIGNAL(clicked()), this, SLOT(down()));
    connect(ui.configureButton, SIGNAL(clicked()), this, SLOT(configure()));
    connect(ui.aboutButton, SIGNAL(clicked()), this, SLOT(info()));
}

BackendsGroupBox::~BackendsGroupBox()
{
}

void BackendsGroupBox::init(BackendPlugin::ActionType actionType, Config *config)
{
    this->actionType = actionType;
    this->config = config;
}

void BackendsGroupBox::setFormat(const QString& format)
{
    this->format = format;
}

void BackendsGroupBox::addItem(const QString& item)
{
    ui.listWidget->addItem(item);
}

void BackendsGroupBox::clear()
{
    ui.listWidget->clear();
    ui.moveUpButton->setEnabled(false);
    ui.moveDownButton->setEnabled(false);
    ui.configureButton->setEnabled(false);
    ui.aboutButton->setEnabled(false);
    ui.moveUpButton->setToolTip("");
    ui.moveDownButton->setToolTip("");
    ui.configureButton->setToolTip("");
    ui.aboutButton->setToolTip("");

    originalOrder.clear();
}

QStringList BackendsGroupBox::getList()
{
    QStringList list;

    for( int i=0; i<ui.listWidget->count(); i++ )
    {
        list.append(ui.listWidget->item(i)->text());
    }

    return list;
}

bool BackendsGroupBox::changed()
{
    return originalOrder != getList();
}

void BackendsGroupBox::resetOrder()
{
    originalOrder = getList();
}

void BackendsGroupBox::itemSelected(int row)
{
    const QListWidgetItem *item = ui.listWidget->item(row);

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName(item->text());

    ui.moveUpButton->setEnabled(row > 0);
    ui.moveDownButton->setEnabled(row < ui.listWidget->count() - 1);

    if( plugin )
    {
        ui.configureButton->setEnabled(plugin->isConfigSupported(actionType, format));

        ui.aboutButton->setEnabled(plugin->hasInfo());

        const QString pluginName = plugin->name();

        if( ui.moveUpButton->isEnabled() )
            ui.moveUpButton->setToolTip(i18n("Move %1 one position up",pluginName));
        else
            ui.moveUpButton->setToolTip("");

        if( ui.moveDownButton->isEnabled() )
            ui.moveDownButton->setToolTip(i18n("Move %1 one position down", pluginName));
        else
            ui.moveDownButton->setToolTip("");

        if( ui.aboutButton->isEnabled() )
            ui.aboutButton->setToolTip(i18n("About %1 ...", pluginName));
        else
            ui.aboutButton->setToolTip("");

        if( ui.configureButton->isEnabled() )
            ui.configureButton->setToolTip(i18n("Configure %1 ...", pluginName));
        else
            ui.configureButton->setToolTip("");
    }
}

void BackendsGroupBox::up()
{
    const int row = ui.listWidget->currentRow();
    ui.listWidget->insertItem(row - 1, ui.listWidget->takeItem(row));
    ui.listWidget->setCurrentRow(row - 1);
    emit orderChanged();
}

void BackendsGroupBox::down()
{
    const int row = ui.listWidget->currentRow();
    ui.listWidget->insertItem(row + 1, ui.listWidget->takeItem(row));
    ui.listWidget->setCurrentRow(row + 1);
    emit orderChanged();
}

void BackendsGroupBox::configure()
{
    const int row = ui.listWidget->currentRow();
    const QListWidgetItem *item = ui.listWidget->item(row);

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName(item->text());

    if( plugin )
    {
        plugin->showConfigDialog(actionType, format, this);
    }
}

void BackendsGroupBox::info()
{
    const int row = ui.listWidget->currentRow();
    const QListWidgetItem *item = ui.listWidget->item(row);

    if( !item )
        return;

    BackendPlugin *plugin = config->pluginLoader()->backendPluginByName(item->text());

    if( plugin )
        plugin->showInfo(this);
}
