#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './tvgrabprefslayout.ui'
**
** Created: Sat Apr 9 20:08:25 2005
**      by: The User Interface Compiler ($Id: tvgrabprefslayout.cpp,v 1.1 2005/07/19 20:03:36 abeverat Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "tvgrabprefslayout.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Form1 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Form1::Form1( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Form1" );
    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Form1::~Form1()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Form1::languageChange()
{
    setCaption( tr2i18n( "Form1" ) );
}

#include "tvgrabprefslayout.moc"
