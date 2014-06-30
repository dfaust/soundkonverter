/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/



#ifndef COMBOBUTTON_H
#define COMBOBUTTON_H

#include <QWidget>

class QString;
class KIcon;
class KPushButton;
class KComboBox;

/**
 * @short ComboButton
 * @author Daniel Faust <hessijames@gmail.com>
 * @version 0.3
 */
class ComboButton : public QWidget
{
    Q_OBJECT
public:
//     enum SizeMode {
//         Min, Max
//     };

    /**
     * Constructor
     * @param parent The parent widget
     * @param name The name of the file list
     */
    ComboButton( QWidget *parent );

    /** Destructor */
    ~ComboButton();

    /** Insert a new item with @p text at position @p index */
    void insertItem( const QString &text, int index = -1 );
    /** Insert a new item with an icon @p pixmap and @p text at position @p index */
    void insertItem( const KIcon &icon, const QString &text, int index = -1 );

    /** Increase the combobutton's height by @p height */
    void increaseHeight( int height );

    /** Sets m_sizeMode to @p mode */
//     void setSizeMode( int mode );

    /** Returns the m_sizeMode */
    int sizeMode();

    /** Sets the font of the combobutton */
    void setFont( const QFont& font );

    /** Returns the font of the button */
    QFont font();

private:
    /** A pointer to the button */
    KPushButton *m_button;
    /** A pointer to the combobox */
    KComboBox *m_box;

    int m_increaseHeight;
    int m_iconHight;

    /** The current size mode */
//     int m_sizeMode;

    /** Recalculate the size of the combobutton */
    void balanceSize();
    /** The button gets a new label, etc. */
    void repaintButton();

//public slots:
    //void setCurrentItem(const QString &item, bool insert=false, int index=-1);
    //void setCurrentItem(int index);

private slots:
    /** Is called when the user selects an item from the popdown menu of the combobox */
    void boxActivated( int index );
    /** Is called when the user clicks the button */
    void buttonClicked();

signals:
    /** The signal clicked is emitted, when the user selects an item */
    void clicked( int index );

};

#endif // COMBOBUTTON_H
