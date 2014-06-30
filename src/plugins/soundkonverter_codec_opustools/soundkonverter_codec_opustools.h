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


#ifndef SOUNDKONVERTER_CODEC_OPUSTOOLS_H
#define SOUNDKONVERTER_CODEC_OPUSTOOLS_H

#include "../../core/codecplugin.h"

#include <QWeakPointer>

class ConversionOptions;
class KDialog;
class QCheckBox;


class soundkonverter_codec_opustools : public CodecPlugin
{
    Q_OBJECT
public:
    /** Default Constructor */
    soundkonverter_codec_opustools( QObject *parent, const QStringList& args );

    /** Default Destructor */
    ~soundkonverter_codec_opustools();

    QString name();

    QList<ConversionPipeTrunk> codecTable();

    bool isConfigSupported( ActionType action, const QString& codecName );
    void showConfigDialog( ActionType action, const QString& codecName, QWidget *parent );
    bool hasInfo();
    void showInfo( QWidget *parent );
    CodecWidget *newCodecWidget();

    unsigned int convert( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    QStringList convertCommand( const KUrl& inputFile, const KUrl& outputFile, const QString& inputCodec, const QString& outputCodec, ConversionOptions *_conversionOptions, TagData *tags = 0, bool replayGain = false );
    float parseOutput( const QString& output );

    ConversionOptions *conversionOptionsFromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements = 0 );

private:
    QWeakPointer<KDialog> configDialog;
    QCheckBox *configDialogUncoupledChannelsCheckBox;

    int configVersion;
    bool uncoupledChannels;

private slots:
    void configDialogSave();
    void configDialogDefault();

};

K_EXPORT_SOUNDKONVERTER_CODEC( opustools, soundkonverter_codec_opustools )


#endif // SOUNDKONVERTER_CODEC_OPUSTOOLS_H


