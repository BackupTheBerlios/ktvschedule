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
#include "tvscheduler.h"
#include <dcopclient.h>

#include <qcstring.h>
#include <qdatastream.h>
#include <qstring.h>
#include <kapplication.h>
#include <kurl.h>
#include <klocale.h>


class Configuration;

tvscheduler::tvscheduler(QObject *parent, const char *name)
 : QObject(parent, name)
{
}


tvscheduler::~tvscheduler()
{
}


void tvscheduler::doNotMiss(TvItem* toScheduleShow)
{

	m_toScheduleShow = toScheduleShow;
	
	if (Config().m_ScheduleType == 0)
	{
		addToKOrganizer();
	}

	if (Config().m_ScheduleType == 1)
	{
		addToKAlarm();
	}
}

void tvscheduler::addToKOrganizer()
{

		QString konsolekalendar="";
		konsolekalendar += "konsolekalendar --add --date ";
		konsolekalendar += m_toScheduleShow->StartTime.date().toString("yyyy-MM-dd");
		konsolekalendar += " --time ";
		konsolekalendar += m_toScheduleShow->StartTime.time().toString("hh:mm");
		konsolekalendar += " --end-date ";
		konsolekalendar += m_toScheduleShow->StopTime.date().toString("yyyy-MM-dd");
		konsolekalendar += " --end-time ";
		konsolekalendar += m_toScheduleShow->StopTime.time().toString("hh:mm");
		konsolekalendar += " --summary ";
		konsolekalendar += "\""+m_toScheduleShow->Title+"\"";
		konsolekalendar += " --description ";
		konsolekalendar += "\"on Channel " + m_toScheduleShow->ChannelName+"\"";
		
		//KShellProcess shellkonsolekalendar ("/bin/bash");
		shellProcess.clearArguments();
		shellProcess << konsolekalendar;
		
		if (!shellProcess.start(KProcess::NotifyOnExit,KProcess::AllOutput) )
		 {
		 qDebug("Is konsolekalendar installed ?");
		 }

}

void tvscheduler::addToKAlarm()
{

	//DCOPClient *client  = new DCOPClient();
	 DCOPClient *client = KApplication::dcopClient();
	// client->attach();
	
	QByteArray data,replyData;
	QCString replyType;
	QDataStream arg(data, IO_WriteOnly);
	
	unsigned int flag = 0x02;
	
	QString Title = m_toScheduleShow->Title+" "+i18n("on channel")+" "+m_toScheduleShow->ChannelName;
	
	arg << Title; //message
	arg << m_toScheduleShow->StartTime.toString(Qt::ISODate); //dateTime
	
	if (Config().m_Cancel)
		{ arg << 1; } else { arg << 0; } //late cancel
	arg << flag; //flag
	
	arg << Config().m_backColor.name(); //bgcolor
	arg << Config().m_fontColor.name(); //fgcolor
	arg << QString::null; //font
	arg << KURL(); //audioURL; //audioURL
	arg << Config().m_AlarmMins; //remindermins
	// arg << ; //repeat type
	//arg << ""; //recurence
	arg << 1; //recurType
	arg << 1; //recurInterval
	arg << 0; //recurCount
	//arg << ""; //endDateTime
	//arg << 1 ; //RepeatInterval
	//arg << 1 ; //RepeateCount
	
	bool worked = client->send("kalarm", "request", "scheduleMessage(QString ,QString , int ,unsigned int ,QString , QString ,QString ,KURL , int , int , int , int )", data); 
	//bool worked = client->call("kalarm", "request", "scheduleMessage(QString , QString , int ,int ,QString , QString , QString , KURL , int , int , int , int )", data,replyType, replyData );
}

void tvscheduler::recordProgram(TvItem* toScheduleShow)
{
	 /*	This are the informations needed by kalva in order to begin recording show
	 
	 const QString & yourStation,
                const QString & yourTitel,
                const QString & yourLength,        // HH:MM (optional :SS)
                const QString & yourStartTime,     // HH:MM
                const QString & yourStartDate      // YYYY-MM-DD (ISO 8601) */
		
	DCOPClient *client = KApplication::dcopClient();
	
	QByteArray data,replyData;
	QCString replyType;
	QDataStream arg(data, IO_WriteOnly);
	
	arg << toScheduleShow->ChannelName;
	arg << toScheduleShow->Title;
	
	QTime ProgramTime(0,0);
	ProgramTime=ProgramTime.addSecs( toScheduleShow->StartTime.secsTo(toScheduleShow->StopTime) );
	
	arg << ProgramTime.toString("hh:mm");
	arg << toScheduleShow->StartTime.toString("hh:mm");
	arg << toScheduleShow->StartTime.toString("yyyy-MM-dd");
		
	bool worked = client->send("kalva", "DCOP_kalva", "DcopRecordMovie(QString ,QString , QString, QString, QString)", data); 
	
	
}

#include "tvscheduler.moc"
