
#include "flakecodecglobal.h"

#include "flakecodecwidget.h"
#include "../../core/conversionoptions.h"



#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <KLineEdit>
#include <KLocale>


FlakeCodecWidget::FlakeCodecWidget()
    : CodecWidget(),
    currentFormat( "flac" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lCompressionLevel = new QLabel( i18n("Compression level:"), this );
    topBox->addWidget( lCompressionLevel );

    sCompressionLevel = new QSlider( Qt::Horizontal, this );
    sCompressionLevel->setRange( 0, 12 );
    sCompressionLevel->setSingleStep( 1 );
    sCompressionLevel->setPageStep( 1 );
//     sQuality->setTickPosition( QSlider::TicksBelow );
//     sQuality->setFixedWidth( sQuality->sizeHint().width() );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSliderChanged(int)) );
    connect( sCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( sCompressionLevel );

    iCompressionLevel = new QSpinBox( this );
    iCompressionLevel->setRange( 0, 12 );
    iCompressionLevel->setSingleStep( 1 );
    iCompressionLevel->setFixedWidth( iCompressionLevel->sizeHint().width() );
//     dQuality->setFixedHeight( cMode->minimumSizeHint().height() );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), this, SLOT(compressionLevelSpinBoxChanged(int)) );
    connect( iCompressionLevel, SIGNAL(valueChanged(int)), SIGNAL(optionsChanged()) );
    topBox->addWidget( iCompressionLevel );

    topBox->addStretch();

    // cmd arguments box

    QHBoxLayout *cmdArgumentsBox = new QHBoxLayout();
    grid->addLayout( cmdArgumentsBox, 1, 0 );

    cCmdArguments = new QCheckBox( i18n("Additional encoder arguments:"), this );
    cmdArgumentsBox->addWidget( cCmdArguments );
    lCmdArguments = new KLineEdit( this );
    lCmdArguments->setEnabled( false );
    const QString toolTip =
        "[-p #]\tPadding bytes to put in header (default: 4096)\n"
//         "[-0 ... -12]\tCompression level (default: 5)\n"
//         "\t\t0 = -b 1152 -t 1 -l 2,2 -m 0 -r 4,4 -s 0\n"
//         "\t\t1 = -b 1152 -t 1 -l 3,4 -m 1 -r 2,2 -s 1\n"
//         "\t\t2 = -b 1152 -t 1 -l 2,4 -m 1 -r 3   -s 1\n"
//         "\t\t3 = -b 4608 -t 2 -l 6   -m 1 -r 3   -s 1\n"
//         "\t\t4 = -b 4608 -t 2 -l 8   -m 1 -r 3   -s 1\n"
//         "\t\t5 = -b 4608 -t 2 -l 8   -m 1 -r 6   -s 1\n"
//         "\t\t6 = -b 4608 -t 2 -l 8   -m 2 -r 8   -s 1\n"
//         "\t\t7 = -b 4608 -t 2 -l 8   -m 3 -r 8   -s 1\n"
//         "\t\t8 = -b 4608 -t 2 -l 12  -m 3 -r 8   -s 1\n"
//         "\t\t9 = -b 4608 -t 2 -l 12  -m 6 -r 8   -s 1\n"
//         "\t\t10 = -b 4608 -t 2 -l 12  -m 5 -r 8   -s 1\n"
//         "\t\t11 = -b 4608 -t 2 -l 32  -m 6 -r 8   -s 1\n"
//         "\t\t12 = -b 4608 -t 2 -l 32  -m 5 -r 8   -s 1\n"
        "[-b #]\tBlock size [16 - 65535] (default: 4608)\n"
        "[-t #]\tPrediction type\n"
        "\t\t0 = no prediction / verbatim\n"
        "\t\t1 = fixed prediction\n"
        "\t\t2 = Levinson-Durbin recursion (default)\n"
        "[-l #[,#]]\tPrediction order {max} or {min},{max} (default: 1,8)\n"
        "[-m #]\tPrediction order selection method\n"
        "\t\t0 = maximum\n"
        "\t\t1 = estimate (default)\n"
        "\t\t2 = 2-level\n"
        "\t\t3 = 4-level\n"
        "\t\t4 = 8-level\n"
        "\t\t5 = full search\n"
        "\t\t6 = log search\n"
        "[-r #[,#]]\tRice partition order {max} or {min},{max} (default: 0,6)\n"
        "[-s #]\tStereo decorrelation method\n"
        "\t\t0 = independent L+R channels\n"
        "\t\t1 = mid-side (default)\n"
        "[-v #]\tVariable block size\n"
        "\t\t0 = fixed (default)\n"
        "\t\t1 = variable, method 1\n"
        "\t\t2 = variable, method 2";
    lCmdArguments->setToolTip( toolTip );
    cmdArgumentsBox->addWidget( lCmdArguments );
    connect( cCmdArguments, SIGNAL(toggled(bool)), lCmdArguments, SLOT(setEnabled(bool)) );

    grid->setRowStretch( 2, 1 );

    iCompressionLevel->setValue( 5 );
}

FlakeCodecWidget::~FlakeCodecWidget()
{}

ConversionOptions *FlakeCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();
    options->qualityMode = ConversionOptions::Lossless;
    options->compressionLevel = iCompressionLevel->value();
    if( cCmdArguments->isChecked() ) options->cmdArguments = lCmdArguments->text();
    else options->cmdArguments = "";
    return options;
}

bool FlakeCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name ) return false;

    ConversionOptions *options = _options;
    iCompressionLevel->setValue( options->compressionLevel );
    cCmdArguments->setChecked( !options->cmdArguments.isEmpty() );
    if( !options->cmdArguments.isEmpty() ) lCmdArguments->setText( options->cmdArguments );
    return true;
}

void FlakeCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format ) return;
    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

QString FlakeCodecWidget::currentProfile()
{
    return i18n("Lossless");
}

bool FlakeCodecWidget::setCurrentProfile( const QString& profile )
{
    return profile == i18n("Lossless");
}

int FlakeCodecWidget::currentDataRate()
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

void FlakeCodecWidget::compressionLevelSliderChanged( int quality )
{
    iCompressionLevel->setValue( quality );
}

void FlakeCodecWidget::compressionLevelSpinBoxChanged( int quality )
{
    sCompressionLevel->setValue( quality );
}


