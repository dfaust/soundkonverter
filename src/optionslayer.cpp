
#include "optionslayer.h"
#include "options.h"
#include "config.h"

#include <KLocalizedString>

#include <QIcon>
#include <QMessageBox>

OptionsLayer::OptionsLayer(Config *config, QWidget *parent) :
    Layer(parent)
{
    ui.setupUi(this);

    ui.options->init(config, i18n("Select your desired output ui.options and click on \"Ok\"."));

    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(abort()));

    init(ui.frame);
}

OptionsLayer::~OptionsLayer()
{
}

void OptionsLayer::fadeIn()
{
    ui.okButton->setEnabled(true);
    ui.cancelButton->setEnabled(true);

    Layer::fadeIn();
}

void OptionsLayer::fadeOut()
{
    urls.clear();

    Layer::fadeOut();
}

void OptionsLayer::addUrls(const QList<QUrl> &urls)
{
    this->urls += urls;
}

void OptionsLayer::abort()
{
    fadeOut();
}

void OptionsLayer::ok()
{
    ConversionOptions *conversionOptions = ui.options->currentConversionOptions();

    if( conversionOptions )
    {
        ui.options->accepted();
        ui.okButton->setEnabled(false);
        ui.cancelButton->setEnabled(false);
        qApp->processEvents();
        emit done(urls, conversionOptions, command);
        emit saveFileList();
        fadeOut();
    }
    else
    {
        QMessageBox::critical(this, "soundKonverter", i18n("No conversion options selected."));   // possibly unneeded i18n string
    }
}

void OptionsLayer::setProfile(const QString &profile)
{
    ui.options->setProfile(profile);
}

void OptionsLayer::setFormat(const QString &format)
{
    ui.options->setFormat(format);
}

void OptionsLayer::setOutputDirectory(const QString &directory)
{
    ui.options->setOutputDirectory(directory);
}

void OptionsLayer::setCommand(const QString &command)
{
    this->command = command;
}

void OptionsLayer::setCurrentConversionOptions(ConversionOptions *conversionOptions)
{
    ui.options->setCurrentConversionOptions(conversionOptions);
}
