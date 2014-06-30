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


#ifndef SOUNDKONVERTER_FILTER_SOX_H
#define SOUNDKONVERTER_FILTER_SOX_H

#include "../../core/filterplugin.h"

#include <QWeakPointer>
#include <QDateTime>

class FilterOptions;
class KDialog;
class KComboBox;


class soundkonverter_filter_sox : public FilterPlugin
{
    Q_OBJECT
public:
    struct SoxCodecData
    {
        QString codecName;
        QString soxCodecName;
        bool external;
        bool experimental;
        bool enabled;
    };

    /** Default Constructor */
    soundkonverter_filter_sox( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_filter_sox();

    QString name();
    int version();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );

    CodecWidget *newCodecWidget();
    FilterWidget *newFilterWidget();

    unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    FilterOptions *filterOptionsFromXml( QDomElement filterOptions );

private:
    QList<SoxCodecData> codecList;
    QWeakPointer<KProcess> infoProcess;
    QString infoProcessOutputData;

    QWeakPointer<KDialog> configDialog;
    KComboBox *configDialogSamplingRateQualityComboBox;

    int configVersion;
    QString samplingRateQuality;
    bool experimentalEffectsEnabled;
    QDateTime soxLastModified;
    QSet<QString> soxCodecList;

    QString soxCodecName( const QString& codecName );

private slots:
    void configDialogSave();
    void configDialogDefault();

    void infoProcessOutput();
    void infoProcessExit( int exitCode, QProcess::ExitStatus exitStatus );
};

K_EXPORT_SOUNDKONVERTER_FILTER( sox, soundkonverter_filter_sox )


#endif // SOUNDKONVERTER_FILTER_SOX_H


