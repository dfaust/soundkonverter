
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
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lCompressionLevel = new QLabel( i18n("Compression level")+":", this );
    topBox->addWidget( lCompressionLevel );

    sCompressionLevel = new QSlider( Qt::Horizontal, this );
    sCompressionLevel->setRange( 0, 8 );
    sCompressionLevel->setSingleStep( 1 );
    sCompressionLevel->setPageStep( 1 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sCompressionLevel );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 0, 8 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
//     dQuality->setFixedHeight( cMode->minimumSizeHint().height() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( iCompressionLevel );

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

QDomDocument FlacCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("compressionLevel",sCompressionLevel->value());
    root.appendChild(encodingOptions);
    return profile;
}

bool FlacCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    Q_UNUSED(profile)

    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    sCompressionLevel->setValue( encodingOptions.attribute("compressionLevel").toInt() );
    return true;
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


