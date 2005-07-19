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
#include "configuration.h"

#include <kapplication.h>       // for 'kapp'
#include <kconfig.h>            // for KConfig

const QColor  Configuration::m_defaultColorOfActive = QColor("#FF0000");
const QColor  Configuration::m_defaultColorOfPast = QColor("#C3C3C3");
const QColor  Configuration::m_defaultColorOfFuture = QColor("#E7CF8C");
const QColor  Configuration::m_defaultColorSelected = QColor("#FF0501");
const QColor  Configuration::m_defaultColorText = QColor("#000000");
const int     Configuration::m_defaultBeginIn = 0;
const int     Configuration::m_defaultDaysToGrab = 0;
const int     Configuration::m_defaultScheduleType = 0;
const int     Configuration::m_defaultRowSize = 20;

const QColor  Configuration::m_defaultFontColor = QColor("#55AA00");
const QColor  Configuration::m_defaultBackColor = QColor("#FF0000");
const int     Configuration::m_defaultAlarmMins = 2;
const QString     Configuration::m_defaultCancel = "TRUE";

Configuration::Configuration() {
    read(); // read the settings or set them to the default values
}

void Configuration::read() {
    KConfig *conf=kapp->config();
    // read general options
    conf->setGroup("General");
    m_SettingsFile = conf->readEntry("SettingsFile", "$HOME/.xmltv/tv_grab_fr.conf");
    m_ScheduleFile = conf->readEntry("ChannelFile", "$HOME/.xmltv/channels.xml");
    m_setToCurrentTime = returnBool (conf->readEntry("SetToCurrentTime","FALSE") );
    m_Executable = conf->readEntry("Executable","tv_grab_fr");
    m_FirstRun = returnBool( conf->readEntry("FirstRun", "TRUE") );
    
    m_FullDetails = returnBool (conf->readEntry("FullDetails","FALSE") );
    m_BeginIn = conf->readEntry("BeginIn",QString::number(0) ).toInt() ;
    m_DaysToGrab = conf->readEntry("DaysToGrab",QString::number(0) ).toInt();
    
    m_GrabberToUse = conf->readEntry("GrabberToUse", "tvgrab");
    
    // read style options
    conf->setGroup("Style");
    m_ColorOfActive = conf->readColorEntry("ColorOfActive", &m_defaultColorOfActive );
    m_ColorOfPast = conf->readColorEntry("ColorOfPast", &m_defaultColorOfPast);
    m_ColorOfFuture = conf->readColorEntry("ColorOfFuture", &m_defaultColorOfFuture );
    
    m_ColorText = conf->readColorEntry("ColorText", &m_defaultColorText);
    m_ColorSelected = conf->readColorEntry("ColorSelected", &m_defaultColorSelected);
    m_RowSize   = conf->readEntry("RowSize",QString::number(20) ).toInt();

    //read Schedule options    
    conf->setGroup("Schedule");
    m_ScheduleType = conf->readEntry("ScheduleType", QString::number(0) ).toInt();
    m_fontColor = conf->readColorEntry("fontColor", &m_defaultFontColor);
    m_backColor = conf->readColorEntry("backColor", &m_defaultBackColor);
    m_AlarmMins = conf->readEntry("AlarmMins", QString::number(2) ).toInt();
    m_Cancel    = returnBool (conf->readEntry("Cancel", "TRUE" ) );
    
    conf->setGroup("DisplayCat");
    m_DetailsInt = conf->readEntry("DetailsInt", QString::number(0) ).toInt();

}

void Configuration::write() { //const
    KConfig *conf=kapp->config();
    // write general options
    conf->setGroup("General");
    conf->writeEntry("SettingsFile", m_SettingsFile);
    conf->writeEntry("ChannelFile", m_ScheduleFile);
    conf->writeEntry("SetToCurrentTime", returnString( m_setToCurrentTime) );
    conf->writeEntry("Executable", m_Executable);
    conf->writeEntry("FirstRun", returnString( m_FirstRun) );
    
    conf->writeEntry("FullDetails", returnString (m_FullDetails) );
    conf->writeEntry("BeginIn", QString::number(m_BeginIn) );
    conf->writeEntry("DaysToGrab", QString::number(m_DaysToGrab) );
    conf->writeEntry("GrabberToUse", m_GrabberToUse);
    
    // write style options
    conf->setGroup("Style");

    conf->writeEntry("ColorOfActive",  m_ColorOfActive);
    conf->writeEntry("ColorOfPast", m_ColorOfPast);
    conf->writeEntry("ColorOfFuture", m_ColorOfFuture);
    
    conf->writeEntry("ColorText", m_ColorText);
    conf->writeEntry("ColorSelected", m_ColorSelected);
    conf->writeEntry("RowSize",QString::number(m_RowSize) );
    
    // write notification options
    conf->setGroup("Schedule");

    conf->writeEntry("ScheduleType", QString::number(m_ScheduleType) );
    conf->writeEntry("fontColor", m_fontColor);
    conf->writeEntry("backColor", m_backColor);
    conf->writeEntry("AlarmMins", QString::number(m_AlarmMins) );
    conf->writeEntry("Cancel", returnString( m_Cancel) );
    
    conf->setGroup("DisplayCat");
    conf->writeEntry("DetailsInt", QString::number(m_DetailsInt) );
}

bool Configuration::returnBool(QString StringBool)
{
	if (StringBool == "TRUE")
	{
		return TRUE;
	}
	
	if (StringBool == "FALSE")
	{
		return FALSE;
	}
}

QString Configuration::returnString( bool StringBool)
{
	if (StringBool)
	{
		return "TRUE";
	}
	
	if (!StringBool)
	{
		return "FALSE";
	}
}

GrabberConfig::GrabberConfig()
{
}

GrabberConfig::~GrabberConfig()
{
}

QString * Configuration::getListOfGrabbers(int * nb_grabbers)
{	
	*nb_grabbers=17;
	QString * List_Grabbers_Found=new QString[* nb_grabbers];
	List_Grabbers_Found[0]="tv_grab_de_tvtoday";
	List_Grabbers_Found[1]="tv_grab_dk";
	List_Grabbers_Found[2]="tv_grab_es";
	List_Grabbers_Found[3]="tv_grab_es_digital";
	List_Grabbers_Found[4]="tv_grab_fi";
	List_Grabbers_Found[5]="tv_grab_fr";
	List_Grabbers_Found[6]="tv_grab_huro";
	List_Grabbers_Found[7]="tv_grab_jp";
	List_Grabbers_Found[8]="tv_grab_na_dd";
	List_Grabbers_Found[9]="tv_grab_nl";
	List_Grabbers_Found[10]="tv_grab_nl_wolf";
	List_Grabbers_Found[11]="tv_grab_no";
	List_Grabbers_Found[12]="tv_grab_pt";
	List_Grabbers_Found[13]="tv_grab_se";
	List_Grabbers_Found[14]="tv_grab_se_swedb";
	List_Grabbers_Found[15]="tv_grab_uk_bleb";
	List_Grabbers_Found[16]="tv_grab_uk_rt";
	return List_Grabbers_Found;
}

GrabberConfig * Configuration::getGrabberConfig(QString GrabberName)
{
	if(GrabberName=="tv_grab_de_tvtoday")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=true;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_dk")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_es")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_es_digital")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_fi")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_fr")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=true;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=true;
		G->offset_linked_with_slow=true;
		return G;
	}
	else if(GrabberName=="tv_grab_huro")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_jp")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_na_dd")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_nl")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=true;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_nl_wolf")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=false;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_no")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_pt")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=false;
		G->offset=false;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_se")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_se_swedb")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_uk_bleb")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else if(GrabberName=="tv_grab_uk_rt")
	{
		GrabberConfig * G=new GrabberConfig();
		G->config_file=true;
		G->slow=false;
		G->days=true;
		G->offset=true;
		G->days_linked_with_slow=false;
		G->offset_linked_with_slow=false;
		return G;
	}
	else
	{
		qDebug("No such grabber in list: "+GrabberName);
		return NULL;
	}
}


Configuration& Config() {
    static Configuration conf;
    return conf;
}


