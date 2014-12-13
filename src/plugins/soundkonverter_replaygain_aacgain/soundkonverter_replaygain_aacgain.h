
#ifndef SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H

#include "../../core/replaygainplugin.h"

#include <QUrl>
// #include <QWeakPointer>

class ConversionOptions;
class QDialog;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;


class AacGainPluginItem : public ReplayGainPluginItem
{
    Q_OBJECT
public:
    AacGainPluginItem( QObject *parent=0 );
    ~AacGainPluginItem();

    QList<QUrl> undoFileList;
};


class soundkonverter_replaygain_aacgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_aacgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_replaygain_aacgain();

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

K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( aacgain, soundkonverter_replaygain_aacgain )


#endif // _SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H_


