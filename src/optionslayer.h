
#ifndef OPTIONSLAYER_H
#define OPTIONSLAYER_H

#include "layer.h"
#include "ui_optionslayer.h"

#include <QUrl>

class Config;
class ConversionOptions;

class OptionsLayer : public Layer
{
    Q_OBJECT

public:
    OptionsLayer(Config *config, QWidget *parent);
    ~OptionsLayer();

    void fadeIn();
    void fadeOut();

    void addUrls(const QList<QUrl>& urls);

private:
    Ui::OptionsLayer ui;

    QList<QUrl> urls;
    QString command;

public slots:
    /** Set the current profile */
    void setProfile(const QString& profile);

    /** Set the current format */
    void setFormat(const QString& format);

    /** Set the current output directory */
    void setOutputDirectory(const QString& directory);

    /** Set the command to execute after the conversion is complete */
    void setCommand(const QString& command);

    /** Set the current conversion options */
    void setCurrentConversionOptions(ConversionOptions *options);

private slots:
    void abort();
    void ok();

signals:
    void done(const QList<QUrl>& urls, ConversionOptions *options, const QString& command);
    void saveFileList();
};

#endif
