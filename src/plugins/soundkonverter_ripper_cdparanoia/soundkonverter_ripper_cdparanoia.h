/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


#ifndef SOUNDKONVERTER_RIPPER_CDPARANOIA_H
#define SOUNDKONVERTER_RIPPER_CDPARANOIA_H

#include "../../core/ripperplugin.h"

#include <KUrl>
#include <KProcess>
#include <QList>
#include <QWeakPointer>

class KDialog;
class QCheckBox;
class QComboBox;
class QSpinBox;


class soundkonverter_ripper_cdparanoia : public RipperPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_ripper_cdparanoia( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_ripper_cdparanoia();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    unsigned int rip( const QString& device, int track, int tracks, const KUrl& outputFile );
    QStringList ripCommand( const QString& device, int track, int tracks, const KUrl& outputFile );
    float parseOutput( const QString& output, int *fromSector, int *toSector );
    float parseOutput( const QString& output );

private slots:
    /** Get the process' output */
    void processOutput();

private:
    QWeakPointer<KDialog> configDialog;
    QCheckBox *configDialogForceReadSpeedCheckBox;
    QSpinBox *configDialogForceReadSpeedSpinBox;
    QComboBox *configDialogForceEndiannessComboBox;
    QSpinBox *configDialogMaximumRetriesSpinBox;
    QCheckBox *configDialogEnableParanoiaCheckBox;
    QCheckBox *configDialogEnableExtraParanoiaCheckBox;

    int configVersion;
    int forceReadSpeed;
    int forceEndianness;
    int maximumRetries;
    bool enableParanoia;
    bool enableExtraParanoia;

private slots:
    void configDialogForceReadSpeedChanged( int state );
    void configDialogSave();
    void configDialogDefault();
};

K_EXPORT_SOUNDKONVERTER_RIPPER( cdparanoia, soundkonverter_ripper_cdparanoia )


#endif // SOUNDKONVERTER_RIPPER_CDPARANOIA_H


