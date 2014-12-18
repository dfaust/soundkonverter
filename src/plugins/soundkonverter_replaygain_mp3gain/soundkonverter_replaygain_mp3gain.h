
#ifndef SOUNDKONVERTER_REPLAYGAIN_MP3GAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_MP3GAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>

class ConversionOptions;
class QDialog;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;

class Mp3GainPluginItem : public ReplayGainPluginItem
{
    Q_OBJECT
public:
    Mp3GainPluginItem( QObject *parent=0 );
    ~Mp3GainPluginItem();

    QList<QUrl> undoFileList;
};

class soundkonverter_replaygain_mp3gain : public ReplayGainPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.soundkonverter.replaygainplugin.mp3gain" FILE "soundkonverter_replaygain_mp3gain.json")
    Q_INTERFACES(ReplayGainPlugin)

public:
    soundkonverter_replaygain_mp3gain();
    ~soundkonverter_replaygain_mp3gain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int apply( const QList<QUrl>& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

private:
    QDialog* configDialog;
    QComboBox *configDialogTagModeComboBox;
    QCheckBox *configDialogModifyAudioStreamCheckBox;
    QDoubleSpinBox *configDialogGainAdjustmentSpinBox;

    int tagMode;
    bool modifyAudioStream;
    double gainAdjustment;

private slots:
    /** The undo process has exited */
    virtual void undoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );

    void configDialogSave();
    void configDialogDefault();
};

#endif // _SOUNDKONVERTER_REPLAYGAIN_MP3GAIN_H_
