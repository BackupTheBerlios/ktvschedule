/****************************************************************************
** Form interface generated from reading ui file './tvgrabprefslayout.ui'
**
** Created: Sat Apr 9 20:08:01 2005
**      by: The User Interface Compiler ($Id: tvgrabprefslayout.h,v 1.1 2005/07/19 20:03:34 abeverat Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FORM1_H
#define FORM1_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;

class Form1 : public QWidget
{
    Q_OBJECT

public:
    Form1( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Form1();


protected:

protected slots:
    virtual void languageChange();

};

#endif // FORM1_H
