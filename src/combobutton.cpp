
#include "combobutton.h"

#include <QApplication>
#include <QLayout>
#include <QString>
#include <QAbstractItemView>
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
    connect( m_box, SIGNAL(activated(int)), this, SLOT(boxActivated(int)) );
    setFocusProxy( m_box );

    m_button = new KPushButton( QString(), this );
    grid->addWidget( m_button, 0, 0 );
    connect( m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
    m_iconHight = m_button->iconSize().height();

//     m_sizeMode = Max;

    balanceSize();
}

ComboButton::~ComboButton()
{}

void ComboButton::balanceSize()
{
    const int fontHeight = QFontMetrics(QApplication::font()).boundingRect("M").size().height();

    const int width = m_button->sizeHint().width();

    const int height = ( m_box->sizeHint().height() > m_button->sizeHint().height() ) ? m_box->sizeHint().height() : m_button->sizeHint().height();

    m_box->setFixedSize( width+1.8*fontHeight, height+m_increaseHeight );
    m_box->view()->setMinimumWidth( m_box->view()->sizeHintForColumn(0) );
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
    if( text.count() > m_box->minimumContentsLength() ) m_box->setMinimumContentsLength( text.count() );
    repaintButton();
}

void ComboButton::insertItem( const KIcon &icon, const QString &text, int index )
{
    if( index == -1 ) index = m_box->count();
    m_box->insertItem( index, icon, text );
    if( text.count() > m_box->minimumContentsLength() ) m_box->setMinimumContentsLength( text.count() );
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

// void ComboButton::setSizeMode( int mode )
// {
//     m_sizeMode = mode;
//     balanceSize();
// }

// int ComboButton::sizeMode()
// {
//     return m_sizeMode;
// }

void ComboButton::setFont( const QFont& font )
{
    m_button->setFont( font );
    m_box->setFont( font );
}

QFont ComboButton::font()
{
    return m_button->font();
}

