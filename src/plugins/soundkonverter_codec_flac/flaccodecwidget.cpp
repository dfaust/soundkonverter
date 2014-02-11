
#include "flaccodecglobal.h"

#include "flaccodecwidget.h"
#include "../../core/conversionoptions.h"



#include <QLayout>
#include <QLabel>
#include <KLocale>
#include <QSpinBox>
#include <QSlider>


FlacCodecWidget::FlacCodecWidget()
    : CodecWidget(),
    currentFormat( "flac" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lCompressionLevel = new QLabel( i18n("Compression level:"), this );
    topBox->addWidget( lCompressionLevel );

    sCompressionLevel = new QSlider( Qt::Horizontal, this );
    sCompressionLevel->setRange( 0, 8 );
    sCompressionLevel->setSingleStep( 1 );
    sCompressionLevel->setPageStep( 1 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sCompressionLevel );
    sCompressionLevel->setToolTip( i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.", 0, 8) );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 0, 8 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
//     dQuality->setFixedHeight( cMode->minimumSizeHint().height() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( iCompressionLevel );
    iCompressionLevel->setToolTip( i18n("Compression level from %1 to %2 where %2 is the best compression.\nThe better the compression, the slower the conversion but the smaller the file size and vice versa.", 0, 8) );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    iCompressionLevel->setValue( 5 );
}

FlacCodecWidget::~FlacCodecWidget()
{}

ConversionOptions *FlacCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    options->compressionLevel = iCompressionLevel->value();
    return options;
}

bool FlacCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    ConversionOptions *options = _options;
    iCompressionLevel->setValue( options->compressionLevel );
    return true;
}

void FlacCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString FlacCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool FlacCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int FlacCodecWidget::currentDataRate()
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

void FlacCodecWidget::compressionLevelSliderChanged( int quality )
{
    iCompressionLevel->setValue( quality );
}

void FlacCodecWidget::compressionLevelSpinBoxChanged( int quality )
{
    sCompressionLevel->setValue( quality );
}


