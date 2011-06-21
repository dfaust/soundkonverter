
#include "codecoptimizations.h"

#include <KLocale>
#include <KIcon>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>


CodecOptimizations::CodecOptimizations( const QList<Optimization>& optimizationList, QWidget* parent, Qt::WFlags f )
    : KDialog( parent, f )
{
    setCaption( i18n("Solutions for backend problems") );
    setWindowIcon( KIcon("help-about") );
    setButtons( KDialog::Close );
    setButtonFocus( KDialog::Close );
    
    QWidget *widget = new QWidget( this );
    setMainWidget( widget );
    QVBoxLayout *box = new QVBoxLayout( widget );
    
    QLabel *messageLabel = new QLabel( i18n("You have installed or removed backends and your soundKonverter settings can be optimized."), this );
    box->addWidget( messageLabel );

    if( !optimizationList.isEmpty() )
    {
        QStringList messageList;
        for( int i=0; i<optimizationList.count(); i++ )
        {
            const QString codecName = optimizationList.at(i).codecName;
            const Optimization::Mode mode = optimizationList.at(i).mode;
            const QString currentBackend = optimizationList.at(i).currentBackend;
            const QString betterBackend = optimizationList.at(i).betterBackend;
            
            if( mode == Optimization::Encode )
                messageList += i18n( "For encoding %1 files the backend '%2' can be replaced with '%3'", codecName, currentBackend, betterBackend );
            else if( mode == Optimization::Decode )
                messageList += i18n( "For decoding %1 files the backend '%2' can be replaced with '%3'", codecName, currentBackend, betterBackend );
            else if( mode == Optimization::ReplayGain )
                messageList += i18n( "For applying Replay Gain to %1 files the backend '%2' can be replaced with '%3'", codecName, currentBackend, betterBackend );
        }
        QLabel *solutionsLabel = new QLabel( messageList.join("\n\n").replace("\n","<br>"), this );
        solutionsLabel->setMargin( 8 );
        solutionsLabel->setWordWrap( true );
        solutionsLabel->setTextInteractionFlags( Qt::TextSelectableByMouse );
        
        QScrollArea *solutionsScrollArea = new QScrollArea();
        solutionsScrollArea->setWidget( solutionsLabel );
        box->addWidget( solutionsScrollArea );
    }
}

CodecOptimizations::~CodecOptimizations()
{}

