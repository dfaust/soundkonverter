
#ifndef SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H
#define SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H

#include "../../core/replaygainplugin.h"

#include <KUrl>

#include <QWeakPointer>

class ConversionOptions;
class KDialog;
class QComboBox;


class soundkonverter_replaygain_aacgain : public ReplayGainPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_replaygain_aacgain( QObject *parent, const QStringList& args );

    /** Default Destructor */
    virtual ~soundkonverter_replaygain_aacgain();

    QString name();

    QList<ReplayGainPipe> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    int apply( const KUrl::List& fileList, ApplyMode mode = Add );
    float parseOutput( const QString& output );

private:
    KUrl::List undoFileList;

    QWeakPointer<KDialog> configDialog;
    QComboBox *configDialogTagLabelComboBox;

    int tagMode;

private slots:
    /** The undo process has exited */
    virtual void undoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );

    void configDialogSave();
    void configDialogDefault();

};

// K_EXPORT_COMPONENT_FACTORY( soundkonverter_replaygain_aacgain, KGenericFactory<soundkonverter_replaygain_aacgain> );
K_EXPORT_SOUNDKONVERTER_REPLAYGAIN( aacgain, soundkonverter_replaygain_aacgain );


#endif // _SOUNDKONVERTER_REPLAYGAIN_AACGAIN_H_


