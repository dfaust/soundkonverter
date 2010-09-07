
#include "wavpackcodecglobal.h"

#include "wavpackcodecwidget.h"
#include "../../core/conversionoptions.h"

#include <math.h>

#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <KLineEdit>
#include <KLocale>
#include <KComboBox>


WavPackCodecWidget::WavPackCodecWidget()
    : CodecWidget(),
    currentFormat( "wavpack" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lCompressionLevel = new QLabel( i18n("Compression level")+":", this );
    topBox->addWidget( lCompressionLevel );

    cCompressionLevel = new KComboBox(  this );
    cCompressionLevel->addItem( i18n("Fast") );
    cCompressionLevel->addItem( i18n("Normal") );
    cCompressionLevel->addItem( i18n("High quality") );
    cCompressionLevel->addItem( i18n("Very high quality") );
    topBox->addWidget( cCompressionLevel );

    topBox->addStretch();

    // cmd arguments box
    
    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 1, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments")+":", this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 2, 1 );
    
    cCompressionLevel->setCurrentIndex( 1 );
}

WavPackCodecWidget::~WavPackCodecWidget()
{}

ConversionOptions *WavPackCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    options->compressionLevel = cCompressionLevel->currentIndex();
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";
    return options;
}

bool WavPackCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;
    
    ConversionOptions *options = _options;
    cCompressionLevel->setCurrentIndex( options->compressionLevel );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() ) lCmdArguments->setText( options->cmdArguments );
    return true;
}

void WavPackCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString WavPackCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool WavPackCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

QDomDocument WavPackCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("compressionLevel",cCompressionLevel->currentIndex());
    encodingOptions.setAttribute("cmdArgumentsEnabled",cCmdArguments->isChecked() && cCmdArguments->isEnabled());
    encodingOptions.setAttribute("cmdArguments",lCmdArguments->text());
    root.appendChild(encodingOptions);
    return profile;
}

bool WavPackCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    cCompressionLevel->setCurrentIndex( encodingOptions.attribute("compressionLevel").toInt() );
    cCmdArguments->setChecked( encodingOptions.attribute("cmdArgumentsEnabled").toInt() );
    lCmdArguments->setText( encodingOptions.attribute("cmdArguments") );
    return true;
}

int WavPackCodecWidget::currentDataRate()
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

