
#include "musepackcodecglobal.h"

#include "musepackcodecwidget.h"
#include "musepackconversionoptions.h"

#include <math.h>

#include <KLocale>
#include <KComboBox>
#include <KLineEdit>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QSlider>
#include <QCheckBox>


MusePackCodecWidget::MusePackCodecWidget()
    : CodecWidget(),
    currentFormat( "musepack" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up preset selection

    QHBoxLayout *presetBox = new QHBoxLayout();
    grid->addLayout( presetBox, 0, 0 );

    QLabel *lPreset = new QLabel( i18n("Preset:"), this );
    presetBox->addWidget( lPreset );
    cPreset = new KComboBox( this );
    cPreset->addItem( i18nc("Backend profile","Telephone") );
    cPreset->addItem( i18nc("Backend profile","Thumb") );
    cPreset->addItem( i18nc("Backend profile","Radio") );
    cPreset->addItem( i18nc("Backend profile","Standard") );
    cPreset->addItem( i18nc("Backend profile","Extreme") );
    cPreset->addItem( i18nc("Backend profile","Insane") );
    cPreset->addItem( i18nc("Backend profile","Braindead") );
    cPreset->addItem( i18n("User defined") );
    cPreset->setCurrentIndex( 3 );
    cPreset->setToolTip( i18n("Either use one of MusePacks's presets or your own settings.") );
    connect( cPreset, SIGNAL(activated(const QString&)), this, SLOT(presetChanged(const QString&)) );
    connect( cPreset, SIGNAL(activated(int)), SIGNAL(optionsChanged()) );
    presetBox->addWidget( cPreset );

    presetBox->addStretch();

    // set up user defined options

    userdefinedBox = new QGroupBox( /*i18n("User defined"),*/ this );
    grid->addWidget( userdefinedBox, 1, 0 );

    QVBoxLayout *userdefinedBoxLayout = new QVBoxLayout();
    userdefinedBox->setLayout( userdefinedBoxLayout );

    // top box ----------------------------------------

    QHBoxLayout *userdefinedTopBox = new QHBoxLayout();
    userdefinedBoxLayout->addLayout( userdefinedTopBox );

    QLabel *lQuality = new QLabel( i18n("Quality"), userdefinedBox );
    userdefinedTopBox->addWidget( lQuality );

    userdefinedTopBox->addSpacing( 5 );

    sQuality = new QSlider( Qt::Horizontal, userdefinedBox );
    sQuality->setRange( 0, 1000 );
    sQuality->setSingleStep( 100 );
    sQuality->setValue( 500 );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    userdefinedTopBox->addWidget( sQuality );

    dQuality = new QDoubleSpinBox( userdefinedBox );
    dQuality->setRange( 0, 10 );
    dQuality->setSingleStep( 1 );
    dQuality->setValue( 5 );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(double)), this, SLOT(qualitySpinBoxChanged(double)) );
    connect( dQuality, SIGNAL(valueChanged(double)), SIGNAL(optionsChanged()) );
    userdefinedTopBox->addWidget( dQuality );

    userdefinedTopBox->addStretch();

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 2, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments:"), this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 3, 1 );

    presetChanged( cPreset->currentText() );
}

MusePackCodecWidget::~MusePackCodecWidget()
{}

// TODO optimize
int MusePackCodecWidget::bitrateForQuality( double quality )
{
    return quality*100/3;
}

// TODO optimize
double MusePackCodecWidget::qualityForBitrate( int bitrate )
{
    return (double)bitrate*3/100;
}

ConversionOptions *MusePackCodecWidget::currentConversionOptions()
{
    MusePackConversionOptions *options = new MusePackConversionOptions();
    options->data.preset = (MusePackConversionOptions::Data::Preset)cPreset->currentIndex();
    options->qualityMode = ConversionOptions::Quality;
    options->quality = dQuality->value();
    options->bitrate = bitrateForQuality( options->quality );
    options->bitrateMode = ConversionOptions::Vbr;

    return options;
}

bool MusePackCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    MusePackConversionOptions *options = static_cast<MusePackConversionOptions*>(_options);
    cPreset->setCurrentIndex( (int)options->data.preset );
    presetChanged( cPreset->currentText() );
    dQuality->setValue( options->quality );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() ) lCmdArguments->setText( options->cmdArguments );

    return true;
}

void MusePackCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString MusePackCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cPreset->currentIndex() == 7 && dQuality->value() == 3.0 )
    {
        return i18n("Very low");
    }
    else if( cPreset->currentIndex() == 7 && dQuality->value() == 4.0 )
    {
        return i18n("Low");
    }
    else if( cPreset->currentIndex() == 7 && dQuality->value() == 5.0 )
    {
        return i18n("Medium");
    }
    else if( cPreset->currentIndex() == 7 && dQuality->value() == 6.0 )
    {
        return i18n("High");
    }
    else if( cPreset->currentIndex() == 7 && dQuality->value() == 7.0 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool MusePackCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cPreset->setCurrentIndex( 7 );
        presetChanged( cPreset->currentText() );
        sQuality->setValue( 300 );
        dQuality->setValue( 3.0 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cPreset->setCurrentIndex( 7 );
        presetChanged( cPreset->currentText() );
        sQuality->setValue( 400 );
        dQuality->setValue( 4.0 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cPreset->setCurrentIndex( 7 );
        presetChanged( cPreset->currentText() );
        sQuality->setValue( 500 );
        dQuality->setValue( 5.0 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cPreset->setCurrentIndex( 7 );
        presetChanged( cPreset->currentText() );
        sQuality->setValue( 600 );
        dQuality->setValue( 6.0 );
        cCmdArguments->setChecked( false );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cPreset->setCurrentIndex( 7 );
        presetChanged( cPreset->currentText() );
        sQuality->setValue( 700 );
        dQuality->setValue( 7.0 );
        cCmdArguments->setChecked( false );
        return true;
    }

    return false;
}

int MusePackCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
//         dataRate = 500000 + dQuality->value()*150000;
//         if( dQuality->value() > 7 ) dataRate += (dQuality->value()-7)*250000;
//         if( dQuality->value() > 9 ) dataRate += (dQuality->value()-9)*800000;
        dataRate = 0;
    }

    return dataRate;
}

void MusePackCodecWidget::presetChanged( const QString& preset )
{
    cPreset->setToolTip( "" );

    if( preset == i18nc("Backend profile","Telephone") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("low quality (~60 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Thumb") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("low/medium quality (~90 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Radio") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("medium quality (~130 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Standard") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("high quality (~180 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Extreme") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("excellent quality (~210 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Insane") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("excellent quality (~240 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Braindead") )
    {
        userdefinedBox->setEnabled( false );
        cPreset->setToolTip( i18n("excellent quality (~270 kbps abr).") );
    }
    else // "User defined"
    {
        userdefinedBox->setEnabled( true );
    }
}

void MusePackCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( double(quality)/100.0 );
}

void MusePackCodecWidget::qualitySpinBoxChanged( double quality )
{
    sQuality->setValue( round(quality*100.0) );
}


