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
#ifndef TVSCHEDULER_H
#define TVSCHEDULER_H

#include <qobject.h>
#include <kprocess.h>
#include "tvguidetable.h"


/**
@author Alexios Beveratos
*/


class tvscheduler : public QObject
{
Q_OBJECT
public:
    tvscheduler(QObject *parent = 0, const char *name = 0);

    ~tvscheduler();
    
public slots:
	void doNotMiss(TvItem*);
	void recordProgram(TvItem*);
	
protected:
	
	TvItem* m_toScheduleShow;
	
	void addToKOrganizer();
	void addToKAlarm();
	
	KShellProcess shellProcess;

};

#endif
