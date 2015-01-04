
#include "lamecodecglobal.h"

#include "lamecodecwidget.h"
#include "lameconversionoptions.h"

#include <QApplication>

#include <KLocalizedString>


#include <QProcess>

LameCodecWidget::LameCodecWidget() :
    CodecWidget(),
    currentFormat("mp3")
{
    ui.setupUi(this);

    connect(ui.presetComboBox, SIGNAL(activated(const QString &)), this, SLOT(presetChanged(const QString &)));
    connect(ui.presetBitrateSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(presetBitrateChanged(int)));
    connect(ui.qualitySlider, SIGNAL(valueChanged(int)),           this, SLOT(qualitySliderChanged(int)));
    connect(ui.modeComboBox, SIGNAL(activated(int)),               this, SLOT(modeChanged(int)));
    connect(ui.qualitySpinBox, SIGNAL(valueChanged(int)),          this, SLOT(qualitySpinBoxChanged(int)));

    connect(ui.cmdArgumentsCheckBox, SIGNAL(toggled(bool)),        ui.cmdArgumentsLineEdit, SLOT(setEnabled(bool)));
    connect(ui.manPageButton, SIGNAL(clicked()),                   this, SLOT(showManpage()));

    connect(ui.presetComboBox, SIGNAL(activated(int)),             SIGNAL(optionsChanged()));
    connect(ui.presetBitrateSpinBox, SIGNAL(valueChanged(int)),    SIGNAL(optionsChanged()));
    connect(ui.presetBitrateCbrCheckBox, SIGNAL(toggled(bool)),    SIGNAL(optionsChanged()));
    connect(ui.presetFastCheckBox, SIGNAL(toggled(bool)),          SIGNAL(optionsChanged()));
    connect(ui.modeComboBox, SIGNAL(activated(int)),               SIGNAL(optionsChanged()));
    connect(ui.qualitySlider, SIGNAL(valueChanged(int)),           SIGNAL(optionsChanged()));
    connect(ui.qualitySpinBox, SIGNAL(valueChanged(int)),          SIGNAL(optionsChanged()));
    connect(ui.bitrateModeComboBox, SIGNAL(activated(int)),        SIGNAL(optionsChanged()));

    presetChanged(ui.presetComboBox->currentText());
    modeChanged(0);
}

LameCodecWidget::~LameCodecWidget()
{
}

// TODO optimize
int LameCodecWidget::bitrateForQuality( int quality )
{
    return 320-quality*32;
}

// TODO optimize
int LameCodecWidget::qualityForBitrate( int bitrate )
{
    return (320-bitrate)/32;
}

ConversionOptions *LameCodecWidget::currentConversionOptions()
{
    LameConversionOptions *options = new LameConversionOptions();
    options->data.preset = (LameConversionOptions::Data::Preset)ui.presetComboBox->currentIndex();
    options->data.presetBitrate = ui.presetBitrateSpinBox->value();
    options->data.presetBitrateCbr = ui.presetBitrateCbrCheckBox->isEnabled() && ui.presetBitrateCbrCheckBox->isChecked();
    options->data.presetFast = ui.presetFastCheckBox->isEnabled() && ui.presetFastCheckBox->isChecked();
    if( ui.modeComboBox->currentText() == i18n("Quality") )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = ui.qualitySpinBox->value();
        options->bitrate = bitrateForQuality( options->quality );
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = ui.qualitySpinBox->value();
        options->quality = qualityForBitrate( options->bitrate );
        options->bitrateMode = ( ui.bitrateModeComboBox->currentText()==i18n("Average") ) ? ConversionOptions::Abr : ConversionOptions::Cbr;
    }
    options->cmdArguments = ui.cmdArgumentsCheckBox->isChecked() ? ui.cmdArgumentsLineEdit->text() : "";

    return options;
}

bool LameCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    LameConversionOptions *options = static_cast<LameConversionOptions*>(_options);
    ui.presetComboBox->setCurrentIndex( (int)options->data.preset );
    presetChanged( ui.presetComboBox->currentText() );
    ui.presetBitrateSpinBox->setValue( options->data.presetBitrate );
    ui.presetBitrateCbrCheckBox->setChecked( options->data.presetBitrateCbr );
    ui.presetFastCheckBox->setChecked( options->data.presetFast );
    if( options->qualityMode == ConversionOptions::Quality )
    {
        ui.modeComboBox->setCurrentIndex( ui.modeComboBox->findText(i18n("Quality")) );
        modeChanged( ui.modeComboBox->currentIndex() );
        ui.qualitySpinBox->setValue( options->quality );
        ui.bitrateModeComboBox->setCurrentIndex( ui.bitrateModeComboBox->findText(i18n("Variable")) );
    }
    else
    {
        ui.modeComboBox->setCurrentIndex( ui.modeComboBox->findText(i18n("Bitrate")) );
        modeChanged( ui.modeComboBox->currentIndex() );
        ui.qualitySpinBox->setValue( options->bitrate );
        if( options->bitrateMode == ConversionOptions::Abr )
            ui.bitrateModeComboBox->setCurrentIndex( ui.bitrateModeComboBox->findText(i18n("Average")) );
        else
            ui.bitrateModeComboBox->setCurrentIndex( ui.bitrateModeComboBox->findText(i18n("Constant")) );
    }
    ui.cmdArgumentsCheckBox->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() )
        ui.cmdArgumentsLineEdit->setText( options->cmdArguments );
    else
        ui.cmdArgumentsLineEdit->clear();

    return true;
}

void LameCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString LameCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( ui.presetComboBox->currentIndex() == 5 && ui.modeComboBox->currentIndex() == 0 && ui.qualitySpinBox->value() == 6 )
    {
        return i18n("Very low");
    }
    else if( ui.presetComboBox->currentIndex() == 5 && ui.modeComboBox->currentIndex() == 0 && ui.qualitySpinBox->value() == 5 )
    {
        return i18n("Low");
    }
    else if( ui.presetComboBox->currentIndex() == 5 && ui.modeComboBox->currentIndex() == 0 && ui.qualitySpinBox->value() == 4 )
    {
        return i18n("Medium");
    }
    else if( ui.presetComboBox->currentIndex() == 5 && ui.modeComboBox->currentIndex() == 0 && ui.qualitySpinBox->value() == 3 )
    {
        return i18n("High");
    }
    else if( ui.presetComboBox->currentIndex() == 5 && ui.modeComboBox->currentIndex() == 0 && ui.qualitySpinBox->value() == 2 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

bool LameCodecWidget::setCurrentProfile( const QString& profile )
{
    ui.cmdArgumentsCheckBox->setChecked( false );
    ui.cmdArgumentsLineEdit->clear();

    if( profile == i18n("Very low") )
    {
        ui.presetComboBox->setCurrentIndex( 5 );
        presetChanged( ui.presetComboBox->currentText() );
        ui.modeComboBox->setCurrentIndex( 0 );
        modeChanged( 0 );
        ui.qualitySlider->setValue( 6 );
        ui.qualitySpinBox->setValue( 6 );
        ui.bitrateModeComboBox->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        ui.presetComboBox->setCurrentIndex( 5 );
        presetChanged( ui.presetComboBox->currentText() );
        ui.modeComboBox->setCurrentIndex( 0 );
        modeChanged( 0 );
        ui.qualitySlider->setValue( 5 );
        ui.qualitySpinBox->setValue( 5 );
        ui.bitrateModeComboBox->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        ui.presetComboBox->setCurrentIndex( 5 );
        presetChanged( ui.presetComboBox->currentText() );
        ui.modeComboBox->setCurrentIndex( 0 );
        modeChanged( 0 );
        ui.qualitySlider->setValue( 4 );
        ui.qualitySpinBox->setValue( 4 );
        ui.bitrateModeComboBox->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("High") )
    {
        ui.presetComboBox->setCurrentIndex( 5 );
        presetChanged( ui.presetComboBox->currentText() );
        ui.modeComboBox->setCurrentIndex( 0 );
        modeChanged( 0 );
        ui.qualitySlider->setValue( 3 );
        ui.qualitySpinBox->setValue( 3 );
        ui.bitrateModeComboBox->setCurrentIndex( 0 );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        ui.presetComboBox->setCurrentIndex( 5 );
        presetChanged( ui.presetComboBox->currentText() );
        ui.modeComboBox->setCurrentIndex( 0 );
        modeChanged( 0 );
        ui.qualitySlider->setValue( 2 );
        ui.qualitySpinBox->setValue( 2 );
        ui.bitrateModeComboBox->setCurrentIndex( 0 );
        return true;
    }

    return false;
}

int LameCodecWidget::currentDataRate()
{
    int dataRate = 0;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        if( ui.presetComboBox->currentIndex() == 0 )
        {
            dataRate = 1090000;
        }
        else if( ui.presetComboBox->currentIndex() == 1 )
        {
            dataRate = 1140000;
        }
        else if( ui.presetComboBox->currentIndex() == 2 )
        {
            dataRate = 1400000;
        }
        else if( ui.presetComboBox->currentIndex() == 3 )
        {
            dataRate = 2360000;
        }
        else if( ui.presetComboBox->currentIndex() == 4 )
        {
            dataRate = ui.presetBitrateSpinBox->value()/8*60*1000;
        }
        else if( ui.presetComboBox->currentIndex() == 5 )
        {
            if( ui.modeComboBox->currentIndex() == 0 )
            {
                dataRate = 1500000 - ui.qualitySpinBox->value()*100000;
            }
            else
            {
                dataRate = ui.qualitySpinBox->value()/8*60*1000;
            }
        }
        if( ui.presetFastCheckBox->isEnabled() && ui.presetFastCheckBox->isChecked() )
        {
            dataRate *= 1.1f;
        }
    }

    return dataRate;
}

void LameCodecWidget::presetChanged( const QString& preset )
{
    ui.presetComboBox->setToolTip( "" );

    if( preset == i18nc("Backend profile","Medium") )
    {
        ui.presetBitrateSpinBox->setEnabled( false );
        ui.presetFastCheckBox->setEnabled( true );
        ui.presetStackedWidget->setCurrentIndex(0);
        ui.presetComboBox->setToolTip( i18n("This preset should provide near transparency to most people on most music (~150 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Standard") )
    {
        ui.presetBitrateSpinBox->setEnabled( false );
        ui.presetFastCheckBox->setEnabled( true );
        ui.presetStackedWidget->setCurrentIndex(0);
        ui.presetComboBox->setToolTip( i18n("This preset should generally be transparent to most people on most music\nand is already quite high in quality (~230 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Extreme") )
    {
        ui.presetBitrateSpinBox->setEnabled( false );
        ui.presetFastCheckBox->setEnabled( true );
        ui.presetStackedWidget->setCurrentIndex(0);
        ui.presetComboBox->setToolTip( i18n("If you have extremely good hearing and similar equipment, this preset will generally provide\nslightly higher quality than the standard mode (~280 kbps abr).") );
    }
    else if( preset == i18nc("Backend profile","Insane") )
    {
        ui.presetBitrateSpinBox->setEnabled( false );
        ui.presetFastCheckBox->setEnabled( false );
        ui.presetStackedWidget->setCurrentIndex(0);
        ui.presetComboBox->setToolTip( i18n("This preset will usually be overkill for most people and most situations, but if you must have\nthe absolute highest quality with no regard to filesize, this is the way to go (320 kbps cbr).") );
    }
    else if( preset == i18n("Specify bitrate") )
    {
        ui.presetBitrateSpinBox->setEnabled( true );
        ui.presetFastCheckBox->setEnabled( false );
        ui.presetStackedWidget->setCurrentIndex(0);
    }
    else // "User defined"
    {
        ui.presetBitrateSpinBox->setEnabled( false );
        ui.presetFastCheckBox->setEnabled( false );
        ui.presetStackedWidget->setCurrentIndex(1);
    }
    presetBitrateChanged( ui.presetBitrateSpinBox->value() );
}

void LameCodecWidget::presetBitrateChanged( int bitrate )
{
    if( ui.presetBitrateSpinBox->isEnabled() && (
        bitrate == 80 || bitrate == 96 || bitrate == 112 ||
        bitrate == 128 || bitrate == 160 || bitrate == 192 ||
        bitrate == 224 || bitrate == 256 || bitrate == 320 ) )
    {
        ui.presetBitrateCbrCheckBox->setEnabled( true );
    }
    else
    {
        ui.presetBitrateCbrCheckBox->setEnabled( false );
    }
}

void LameCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        ui.qualitySlider->setRange( 0, 9 );
//         ui.qualitySlider->setTickInterval( 100 );
        ui.qualitySlider->setSingleStep( 1 );
        ui.qualitySpinBox->setRange( 0, 9 );
        ui.qualitySpinBox->setSingleStep( 1 );
        ui.qualitySpinBox->setSuffix( "" );
        ui.qualitySlider->setValue( 5 );
        ui.qualitySpinBox->setValue( 5 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        ui.qualitySlider->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 9, 0) );
        ui.qualitySpinBox->setToolTip( i18n("Quality level from %1 to %2 where %2 is the highest quality.\nThe higher the quality, the bigger the file size and vice versa.", 9, 0) );

        ui.bitrateModeComboBox->clear();
        ui.bitrateModeComboBox->addItem( i18n("Variable") );
        ui.bitrateModeComboBox->setEnabled( false );
    }
    else
    {
        ui.qualitySlider->setRange( 8, 320 );
//         ui.qualitySlider->setTickInterval( 800 );
        ui.qualitySlider->setSingleStep( 8 );
        ui.qualitySpinBox->setRange( 8, 320 );
        ui.qualitySpinBox->setSingleStep( 1 );
        ui.qualitySpinBox->setSuffix( " kbps" );
        ui.qualitySlider->setValue( 160 );
        ui.qualitySpinBox->setValue( 160 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
        ui.qualitySlider->setToolTip( "" );
        ui.qualitySpinBox->setToolTip( "" );

        ui.bitrateModeComboBox->clear();
        ui.bitrateModeComboBox->addItem( i18n("Average") );
        ui.bitrateModeComboBox->addItem( i18n("Constant") );
        ui.bitrateModeComboBox->setEnabled( true );
    }
}

void LameCodecWidget::qualitySliderChanged( int quality )
{
    if( ui.modeComboBox->currentText() == i18n("Quality") )
    {
        ui.qualitySpinBox->setValue( 9 - quality );
    }
    else
    {
        ui.qualitySpinBox->setValue( quality );
    }
}

void LameCodecWidget::qualitySpinBoxChanged( int quality )
{
    if( ui.modeComboBox->currentText() == i18n("Quality") )
    {
        ui.qualitySlider->setValue( 9 - quality );
    }
    else
    {
        ui.qualitySlider->setValue( quality );
    }
}

void LameCodecWidget::showManpage()
{
//     QProcess::startDetached("konsole", QStringList() << "-e" << "man" << "lame");
    QProcess::startDetached("konqueror", QStringList("man:/lame"));
}
