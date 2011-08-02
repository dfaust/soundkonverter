
#include "maccodecglobal.h"

#include "maccodecwidget.h"
#include "../../core/conversionoptions.h"



#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <KLineEdit>
#include <KLocale>


MacCodecWidget::MacCodecWidget()
    : CodecWidget(),
    currentFormat( "ape" )
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
    sCompressionLevel->setRange( 1, 5 );
    sCompressionLevel->setSingleStep( 10 );
    sCompressionLevel->setPageStep( 1 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sCompressionLevel );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 1, 5 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setSuffix( "000" );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
//     dQuality->setFixedHeight( cMode->minimumSizeHint().height() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( iCompressionLevel );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    iCompressionLevel->setValue( 2 );
}

MacCodecWidget::~MacCodecWidget()
{}

ConversionOptions *MacCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    options->compressionLevel = iCompressionLevel->value()*1000;
    return options;
}

bool MacCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    ConversionOptions *options = _options;
    iCompressionLevel->setValue( options->compressionLevel/1000 );
    return true;
}

void MacCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString MacCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool MacCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

QDomDocument MacCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("compressionLevel",sCompressionLevel->value()*1000);
    root.appendChild(encodingOptions);
    return profile;
}

bool MacCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    Q_UNUSED(profile)

    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    sCompressionLevel->setValue( encodingOptions.attribute("compressionLevel").toInt()/1000 );
    return true;
}

int MacCodecWidget::currentDataRate()
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

void MacCodecWidget::compressionLevelSliderChanged( int quality )
{
    iCompressionLevel->setValue( quality );
}

void MacCodecWidget::compressionLevelSpinBoxChanged( int quality )
{
    sCompressionLevel->setValue( quality );
}


