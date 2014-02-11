
#include "soxfilterglobal.h"

#include "soxcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <QCheckBox>
#include <QLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <KComboBox>
#include <KLocale>
#include <KLineEdit>


// TODO clean up use of modeChanged()

SoxCodecWidget::SoxCodecWidget()
    : CodecWidget(),
    currentFormat( "" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );


    // flac

    lCompressionLevel = new QLabel( i18n("Compression level:"), this );
    topBox->addWidget( lCompressionLevel );

    sCompressionLevel = new QSlider( Qt::Horizontal, this );
    sCompressionLevel->setRange( 0, 8 );
    sCompressionLevel->setSingleStep( 1 );
    sCompressionLevel->setPageStep( 1 );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sCompressionLevel );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 0, 8 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( iCompressionLevel );

    iCompressionLevel->setValue( 5 );


    // mp2, mp3 and ogg vorbis

    lMode = new QLabel( i18n("Mode:"), this );
    topBox->addWidget( lMode );
    cMode = new KComboBox( this );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cMode );

    lQuality = new QLabel( i18n("Quality:"), this );
    topBox->addWidget( lQuality );

    sQuality = new QSlider( Qt::Horizontal, this );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QDoubleSpinBox( this );
    dQuality->setRange( 8, 320 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    topBox->addWidget( dQuality );


    // amr nb and amr wb

    lBitratePreset = new QLabel( i18n("Bitrate:"), this );
    topBox->addWidget( lBitratePreset );
    cBitratePreset = new KComboBox( this );
    cBitratePreset->addItem( "00.00 kbps" );
    topBox->addWidget( cBitratePreset );


    topBox->addStretch();

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 1, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments:"), this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 2, 1 );

    modeChanged( 0 );
}

SoxCodecWidget::~SoxCodecWidget()
{}

ConversionOptions *SoxCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();

    if( currentFormat == "wav" ||
        currentFormat == "aiff" ||
        currentFormat == "flac" )
    {
        options->qualityMode = ConversionOptions::Lossless;
        options->compressionLevel = iCompressionLevel->value();
    }
    else if( currentFormat == "mp2" )
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = dQuality->value();
        options->bitrateMode = ConversionOptions::Cbr;
    }
    else if( currentFormat == "mp3" )
    {
        if( cMode->currentText() == i18n("Quality") )
        {
            options->qualityMode = ConversionOptions::Quality;
            options->quality = dQuality->value();
        }
        else
        {
            options->qualityMode = ConversionOptions::Bitrate;
            options->bitrate = dQuality->value();
            options->bitrateMode = ConversionOptions::Cbr;
        }
    }
    else if( currentFormat == "ogg vorbis" )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = dQuality->value();
    }
    else if( currentFormat == "amr nb" ||
             currentFormat == "amr wb" )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = cBitratePreset->itemData( cBitratePreset->currentIndex() ).toInt();
    }
    options->cmdArguments = cCmdArguments->isChecked() ? lCmdArguments->text() : "";

    return options;
}

bool SoxCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;

    if( currentFormat == "wav" ||
        currentFormat == "aiff" ||
        currentFormat == "flac" )
    {
        iCompressionLevel->setValue( options->compressionLevel );
    }
    else if( currentFormat == "mp2" )
    {
        dQuality->setValue( options->bitrate );
    }
    else if( currentFormat == "mp3" )
    {
        if( options->qualityMode == ConversionOptions::Quality )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            dQuality->setValue( options->quality );
        }
        else
        {
            cMode->setCurrentIndex( 1 );
            modeChanged( 1 );
            dQuality->setValue( options->bitrate );
        }
    }
    else if( currentFormat == "ogg vorbis" )
    {
        dQuality->setValue( options->quality );
    }
    else if( currentFormat == "amr nb" ||
             currentFormat == "amr wb" )
    {
        cBitratePreset->setCurrentIndex( cBitratePreset->findData(options->quality) );
    }

    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() )
        lCmdArguments->setText( options->cmdArguments );
    else
        lCmdArguments->clear();

    return true;
}

void SoxCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;

    if( currentFormat == "flac" )
    {
        lCompressionLevel->show();
        sCompressionLevel->show();
        iCompressionLevel->show();
        sCompressionLevel->setToolTip( i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.", 0, 8) );
        iCompressionLevel->setToolTip( i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.", 0, 8) );

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "mp2" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->show();
        lQuality->setText( i18n("Bitrate:") );
        sQuality->show();
        dQuality->show();
        sQuality->setRange( 32, 384 );
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 32, 384 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        dQuality->setValue( 160 );
        sQuality->setToolTip( "" );
        dQuality->setToolTip( "" );

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "mp3" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->show();
        cMode->show();
        lQuality->hide();
        sQuality->show();
        dQuality->show();
        modeChanged( 0 );

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "ogg vorbis" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->show();
        lQuality->setText( i18n("Quality:") );
        sQuality->show();
        dQuality->show();
        sQuality->setRange( -100, 1000 );
        sQuality->setSingleStep( 50 );
        dQuality->setRange( -1, 10 );
        dQuality->setSingleStep( 0.01 );
        dQuality->setDecimals( 2 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 400 );
        dQuality->setValue( 4.0 );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", -1, 10) );
        dQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", -1, 10) );

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
    else if( currentFormat == "amr nb" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->show();
        cBitratePreset->show();
        cBitratePreset->clear();
        cBitratePreset->addItem( "4.75 kbps", 0 );
        cBitratePreset->addItem( "5.15 kbps", 1 );
        cBitratePreset->addItem( "5.9 kbps", 2 );
        cBitratePreset->addItem( "6.7 kbps", 3 );
        cBitratePreset->addItem( "7.4 kbps", 4 );
        cBitratePreset->addItem( "7.95 kbps", 5 );
        cBitratePreset->addItem( "10.2 kbps", 6 );
        cBitratePreset->addItem( "12.2 kbps", 7 );
    }
    else if( currentFormat == "amr wb" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->show();
        cBitratePreset->show();
        cBitratePreset->clear();
        cBitratePreset->addItem( "6.6 kbps", 0 );
        cBitratePreset->addItem( "8.85 kbps", 1 );
        cBitratePreset->addItem( "12.65 kbps", 2 );
        cBitratePreset->addItem( "14.25 kbps", 3 );
        cBitratePreset->addItem( "15.85 kbps", 4 );
        cBitratePreset->addItem( "18.25 kbps", 5 );
        cBitratePreset->addItem( "19.85 kbps", 6 );
        cBitratePreset->addItem( "23.05 kbps", 7 );
        cBitratePreset->addItem( "23.85 kbps", 8 );
    }
    else if( currentFormat == "wav" ||
             currentFormat == "8svx" ||
             currentFormat == "aiff" )
    {
        lCompressionLevel->hide();
        sCompressionLevel->hide();
        iCompressionLevel->hide();

        lMode->hide();
        cMode->hide();
        lQuality->hide();
        sQuality->hide();
        dQuality->hide();

        lBitratePreset->hide();
        cBitratePreset->hide();
    }
}

QString SoxCodecWidget::currentProfile()
{
    if( currentFormat == "wav" ||
        currentFormat == "aiff" ||
        currentFormat == "flac" )
    {
        return i18n("Lossless");
    }
    else if( currentFormat == "mp2" )
    {
        if( dQuality->value() == 64 )
        {
            return i18n("Very low");
        }
        else if( dQuality->value() == 128 )
        {
            return i18n("Low");
        }
        else if( dQuality->value() == 160 )
        {
            return i18n("Medium");
        }
        else if( dQuality->value() == 240 )
        {
            return i18n("High");
        }
        else if( dQuality->value() == 320 )
        {
            return i18n("Very high");
        }
    }
    else if( currentFormat == "mp3" )
    {
        if( cMode->currentIndex() == 0 && dQuality->value() == 6 )
        {
            return i18n("Very low");
        }
        else if( cMode->currentIndex() == 0 && dQuality->value() == 5 )
        {
            return i18n("Low");
        }
        else if( cMode->currentIndex() == 0 && dQuality->value() == 4 )
        {
            return i18n("Medium");
        }
        else if( cMode->currentIndex() == 0 && dQuality->value() == 3 )
        {
            return i18n("High");
        }
        else if( cMode->currentIndex() == 0 && dQuality->value() == 2 )
        {
            return i18n("Very high");
        }
    }
    else if( currentFormat == "ogg vorbis" )
    {
        if( dQuality->value() == 2 )
        {
            return i18n("Very low");
        }
        else if( dQuality->value() == 3 )
        {
            return i18n("Low");
        }
        else if( dQuality->value() == 4 )
        {
            return i18n("Medium");
        }
        else if( dQuality->value() == 5 )
        {
            return i18n("High");
        }
        else if( dQuality->value() == 6 )
        {
            return i18n("Very high");
        }
    }

    // amr nb, amr wb, 8svx
    return i18n("User defined");
}

bool SoxCodecWidget::setCurrentProfile( const QString& profile )
{
    cCmdArguments->setChecked( false );
    lCmdArguments->clear();

    if( profile == i18n("Very low") )
    {
        if( currentFormat == "mp2" )
        {
            sQuality->setValue( 64 );
            dQuality->setValue( 64 );
            return true;
        }
        else if( currentFormat == "mp3" )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            sQuality->setValue( 6 );
            dQuality->setValue( 6 );
            return true;
        }
        else if( currentFormat == "ogg vorbis" )
        {
            sQuality->setValue( 200 );
            dQuality->setValue( 2 );
            return true;
        }
    }
    else if( profile == i18n("Low") )
    {
        if( currentFormat == "mp2" )
        {
            sQuality->setValue( 128 );
            dQuality->setValue( 128 );
            return true;
        }
        else if( currentFormat == "mp3" )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            sQuality->setValue( 5 );
            dQuality->setValue( 5 );
            return true;
        }
        else if( currentFormat == "ogg vorbis" )
        {
            sQuality->setValue( 300 );
            dQuality->setValue( 3 );
            return true;
        }
    }
    else if( profile == i18n("Medium") )
    {
        if( currentFormat == "mp2" )
        {
            sQuality->setValue( 160 );
            dQuality->setValue( 160 );
            return true;
        }
        else if( currentFormat == "mp3" )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            sQuality->setValue( 4 );
            dQuality->setValue( 4 );
            return true;
        }
        else if( currentFormat == "ogg vorbis" )
        {
            sQuality->setValue( 400 );
            dQuality->setValue( 4 );
            return true;
        }
    }
    else if( profile == i18n("High") )
    {
        if( currentFormat == "mp2" )
        {
            sQuality->setValue( 240 );
            dQuality->setValue( 240 );
            return true;
        }
        else if( currentFormat == "mp3" )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            sQuality->setValue( 5 );
            dQuality->setValue( 5 );
            return true;
        }
        else if( currentFormat == "ogg vorbis" )
        {
            sQuality->setValue( 500 );
            dQuality->setValue( 5 );
            return true;
        }
    }
    else if( profile == i18n("Very high") )
    {
        if( currentFormat == "mp2" )
        {
            sQuality->setValue( 320 );
            dQuality->setValue( 320 );
            return true;
        }
        else if( currentFormat == "mp3" )
        {
            cMode->setCurrentIndex( 0 );
            modeChanged( 0 );
            sQuality->setValue( 2 );
            dQuality->setValue( 2 );
            return true;
        }
        else if( currentFormat == "ogg vorbis" )
        {
            sQuality->setValue( 600 );
            dQuality->setValue( 6 );
            return true;
        }
    }
    else if( profile == i18n("Lossless") )
    {
        if( currentFormat == "wav" ||
            currentFormat == "aiff" ||
            currentFormat == "flac" )
        {
            return true;
        }
    }

    return false;
}

int SoxCodecWidget::currentDataRate() // TODO
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        dataRate = 6400000;
    }

    return dataRate;
}

void SoxCodecWidget::compressionLevelSliderChanged( int quality )
{
    iCompressionLevel->setValue( quality );
}

void SoxCodecWidget::compressionLevelSpinBoxChanged( int quality )
{
    sCompressionLevel->setValue( quality );
}

void SoxCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 9 );
        sQuality->setSingleStep( 1 );
        dQuality->setRange( 0, 9 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 5 );
        dQuality->setValue( 5 );
        //dQuality->setValue( qualityForBitrate(dQuality->value()) );
        //qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 9, 0) );
        dQuality->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 9, 0) );
    }
    else
    {
        sQuality->setRange( 8, 320 );
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 8, 320 );
        dQuality->setSingleStep( 1 );
        dQuality->setDecimals( 0 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 160 );
        dQuality->setValue( 160 );
        //dQuality->setValue( bitrateForQuality(dQuality->value()) );
        //qualitySpinBoxChanged( dQuality->value() );
        sQuality->setToolTip( "" );
        dQuality->setToolTip( "" );
    }
}

void SoxCodecWidget::qualitySliderChanged( int quality )
{
    double value;
    if( currentFormat == "ogg vorbis" )
        value = double(quality)/100.0;
    else if( currentFormat == "mp3" && cMode->currentIndex() == 0 )
        value = 9 - quality;
    else
        value = quality;

    dQuality->setValue( value );
}

void SoxCodecWidget::qualitySpinBoxChanged( double quality )
{
    int value;
    if( currentFormat == "ogg vorbis" )
        value = round(quality*100.0);
    else if( currentFormat == "mp3" && cMode->currentIndex() == 0 )
        value = 9 - quality;
    else
        value = quality;

    sQuality->setValue( value );
}

