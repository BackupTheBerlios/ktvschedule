/***************************************************************************
 *   Copyright (C) 2005 by Alexios Beveratos                               *
 *   alexios@beveratos.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "systemtray.h"
#include <qtooltip.h>

SystemTray::SystemTray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
    m_appPix = loadIcon("ktvschedule");
    setPixmap(m_appPix);
    setToolTip();
}

SystemTray::~SystemTray()
{
}


////////////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////////////

void SystemTray::setToolTip(const QString &tip)
{
    QToolTip::remove(this);

    if(tip.isNull())
        QToolTip::add(this, "ktvschedule");
    else
        QToolTip::add(this, tip);
}

void SystemTray::addPopUpMenu(QStringList nowPlaying)
{
	QString tooltip="";
	for ( QStringList::Iterator it = nowPlaying.begin(); it != nowPlaying.end();it++)
	{
		//details->insertItem(*it);
		tooltip+="<u>";
		tooltip+=*it;
		tooltip+="</u> : <i>";
		it++;
		tooltip+=*it;
		tooltip+="</i><br>";
	}
	QToolTip::add(this,tooltip);

}
//#include "systemray.moc"


