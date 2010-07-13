
#include "combobutton.h"

#include <QLayout>
#include <QString>
#include <KIcon>
#include <KPushButton>
#include <KComboBox>

ComboButton::ComboButton( QWidget *parent )
    : QWidget( parent )
{
    m_increaseHeight = 0;

    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );

    m_box = new KComboBox( this );
    grid->addWidget( m_box, 0, 0 );
//     m_box->setSizeAdjustPolicy( QComboBox::AdjustToContents ); // default
    connect( m_box, SIGNAL(activated(int)), this, SLOT(boxActivated(int)) );
    setFocusProxy( m_box );

    m_button = new KPushButton( QString(), this );
    grid->addWidget( m_button, 0, 0 );
    connect( m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
    m_iconHight = m_button->iconSize().height();

    m_sizeMode = Max;

    balanceSize();
}

ComboButton::~ComboButton()
{}

void ComboButton::balanceSize()
{
    const int width = m_button->sizeHint().width();

    const int height = ( m_box->sizeHint().height() > m_button->sizeHint().height() ) ? m_box->sizeHint().height() : m_button->sizeHint().height();

    m_box->setFixedSize( width+19, height+m_increaseHeight );
    m_button->setFixedSize( width, height+m_increaseHeight );
    m_button->setIconSize( QSize(m_iconHight+m_increaseHeight,m_iconHight+m_increaseHeight) );
}

void ComboButton::repaintButton()
{
    m_button->setText( m_box->currentText() );
    m_button->setIcon( KIcon(m_box->itemIcon(m_box->currentIndex())) );
    balanceSize();
}

void ComboButton::insertItem( const QString &text, int index )
{
    if( index == -1 ) index = m_box->count();
    m_box->insertItem( index, text );
    repaintButton();
}

void ComboButton::insertItem( const KIcon &icon, const QString &text, int index )
{
    if( index == -1 ) index = m_box->count();
    m_box->insertItem( index, icon, text );
    repaintButton();
}

void ComboButton::increaseHeight( int height )
{
    m_increaseHeight = height;
    balanceSize();
}

void ComboButton::boxActivated( int index )
{
    repaintButton();
    emit clicked( index );
}

void ComboButton::buttonClicked()
{
    emit clicked( m_box->currentIndex() );
}

void ComboButton::setSizeMode( int mode )
{
    m_sizeMode = mode;
    balanceSize();
}

int ComboButton::sizeMode()
{
    return m_sizeMode;
}

void ComboButton::setFont( const QFont& font )
{
    m_button->setFont( font );
    m_box->setFont( font );
}

QFont ComboButton::font()
{
    return m_button->font();
}

