
#include "ffmpegcodecglobal.h"

#include "ffmpegcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>
#include <KComboBox>
#include <KLineEdit>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>


FFmpegCodecWidget::FFmpegCodecWidget()
    : CodecWidget(),
    currentFormat( "ogg vorbis" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    lBitrate = new QLabel( i18n("Bitrate:"), this );
    topBox->addWidget( lBitrate );

    sBitrate = new QSlider( Qt::Horizontal, this );
    sBitrate->setRange( 8, 320 );
    sBitrate->setValue( 160 );
    connect( sBitrate, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sBitrate, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sBitrate );

    iBitrate = new QSpinBox( this );
    iBitrate->setRange( 8, 320 );
    iBitrate->setValue( 160 );
    iBitrate->setSuffix( " kbps" );
    iBitrate->setFixedWidth( iBitrate->sizeHint().width() );
    connect( iBitrate, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( iBitrate, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( iBitrate );

    cBitrate = new KComboBox( this );
    cBitrate->addItem( "32 kbps" );
    cBitrate->addItem( "40 kbps" );
    cBitrate->addItem( "48 kbps" );
    cBitrate->addItem( "56 kbps" );
    cBitrate->addItem( "64 kbps" );
    cBitrate->addItem( "80 kbps" );
    cBitrate->addItem( "96 kbps" );
    cBitrate->addItem( "112 kbps" );
    cBitrate->addItem( "128 kbps" );
    cBitrate->addItem( "160 kbps" );
    cBitrate->addItem( "192 kbps" );
    cBitrate->addItem( "224 kbps" );
    cBitrate->addItem( "256 kbps" );
    cBitrate->addItem( "320 kbps" );
    cBitrate->addItem( "384 kbps" );
    cBitrate->addItem( "448 kbps" );
    cBitrate->addItem( "512 kbps" );
    cBitrate->addItem( "576 kbps" );
    cBitrate->addItem( "640 kbps" );
    cBitrate->setCurrentIndex( 10 );
    cBitrate->hide();
    connect( cBitrate, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( cBitrate );

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
}

FFmpegCodecWidget::~FFmpegCodecWidget()
{}

ConversionOptions *FFmpegCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Bitrate;
    if( currentFormat == "ac3" )
        options->bitrate = cBitrate->currentText().replace(" kbps","").toInt();
    else
        options->bitrate = iBitrate->value();
    options->quality = -1000;
    options->bitrateMode = ConversionOptions::Cbr;
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";

    return options;
}

bool FFmpegCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;

    if( currentFormat == "ac3" )
        cBitrate->setCurrentIndex( cBitrate->findText(QString::number(options->bitrate)+" kbps") );
    else
        iBitrate->setValue( options->bitrate );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() )
        lCmdArguments->setText( options->cmdArguments );

    return true;
}

void FFmpegCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;

    if( currentFormat == "wav" )
    {
        lBitrate->hide();
        sBitrate->hide();
        iBitrate->hide();
        cBitrate->hide();
        cCmdArguments->hide();
        lCmdArguments->hide();
    }
    else if( currentFormat == "flac" || currentFormat == "m4a/alac" )
    {
        lBitrate->hide();
        sBitrate->hide();
        iBitrate->hide();
        cBitrate->hide();
        cCmdArguments->show();
        lCmdArguments->show();
    }
    else if( currentFormat == "ac3" )
    {
        lBitrate->show();
        sBitrate->hide();
        iBitrate->hide();
        cBitrate->show();
        cCmdArguments->show();
        lCmdArguments->show();
    }
    else
    {
        lBitrate->show();
        sBitrate->show();
        iBitrate->show();
        cBitrate->hide();
        cCmdArguments->show();
        lCmdArguments->show();
    }

    if( currentFormat == "mp2" )
    {
        sBitrate->setRange( 32, 384 );
        iBitrate->setRange( 32, 384 );
    }
    else
    {
        sBitrate->setRange( 8, 320 );
        iBitrate->setRange( 8, 320 );
    }
}

QString FFmpegCodecWidget::currentProfile()
{
    if( currentFormat == "wav" || currentFormat == "flac" || currentFormat == "m4a/alac" )
    {
        return i18n("Lossless");
    }
    else if( currentFormat == "amr nb" || currentFormat == "speex" )
    {
        return i18n("User defined");
    }
    else if( currentFormat == "ac3" )
    {
        if( cBitrate->currentText() == "64 kbps" )
        {
            return i18n("Very low");
        }
        else if( cBitrate->currentText() == "128 kbps" )
        {
            return i18n("Low");
        }
        else if( cBitrate->currentText() == "192 kbps" )
        {
            return i18n("Medium");
        }
        else if( cBitrate->currentText() == "320 kbps" )
        {
            return i18n("High");
        }
        else if( cBitrate->currentText() == "640 kbps" )
        {
            return i18n("Very high");
        }
    }
    else
    {
        if( iBitrate->value() == 64 )
        {
            return i18n("Very low");
        }
        else if( iBitrate->value() == 128 )
        {
            return i18n("Low");
        }
        else if( iBitrate->value() == 160 )
        {
            return i18n("Medium");
        }
        else if( iBitrate->value() == 240 )
        {
            return i18n("High");
        }
        else if( iBitrate->value() == 320 )
        {
            return i18n("Very high");
        }
    }

    return i18n("User defined");
}

bool FFmpegCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("64 kbps") );
        }
        else
        {
            sBitrate->setValue( 64 );
            iBitrate->setValue( 64 );
        }
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("128 kbps") );
        }
        else
        {
            sBitrate->setValue( 128 );
            iBitrate->setValue( 128 );
        }
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("192 kbps") );
        }
        else
        {
            sBitrate->setValue( 160 );
            iBitrate->setValue( 160 );
        }
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("320 kbps") );
        }
        else
        {
            sBitrate->setValue( 240 );
            iBitrate->setValue( 240 );
        }
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        if( currentFormat == "ac3" )
        {
            cBitrate->setCurrentIndex( cBitrate->findText("640 kbps") );
        }
        else
        {
            sBitrate->setValue( 320 );
            iBitrate->setValue( 320 );
        }
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

int FFmpegCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else if( currentFormat == "flac" || currentFormat == "m4a/alac" )
    {
        dataRate = 6520000;
    }
    else if( currentFormat == "ac3" )
    {
        dataRate = cBitrate->currentText().replace(" kbps","").toInt()/8*60*1000;
    }
    else
    {
        dataRate = iBitrate->value()/8*60*1000;
    }

    return dataRate;
}

void FFmpegCodecWidget::qualitySliderChanged( int bitrate )
{
    iBitrate->setValue( bitrate );
}

void FFmpegCodecWidget::qualitySpinBoxChanged( int bitrate )
{
    sBitrate->setValue( bitrate );
}


