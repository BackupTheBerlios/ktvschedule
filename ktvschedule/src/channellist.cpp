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

#include <qfile.h>
#include <qstyle.h>
#include <qpoint.h>
#include <qdir.h>
#include <qtextstream.h>
#include <kurl.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qregexp.h> 
#include <qurloperator.h> 
#include <qnetwork.h>
#include <qdom.h>

#include <kmessagebox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include "configuration.h"
#include "channellist.h"

class TvStation;
class KStandardDirs;

channellist::channellist(QWidget* parent, const char* name, bool modal, WFlags fl)
: ChannelSelect_ui(parent,name, modal,fl)
{
	init();
	
	KStandardDirs *dirs = new KStandardDirs();
	
	home = dirs->localkdedir () + "/share/apps/ktvschedule/";
	
	openFile();
}

channellist::~channellist()
{
}

void channellist::init()
{
	b_add_channel->setPixmap(KGlobal::iconLoader()->loadIcon("2rightarrow", KIcon::Toolbar));
	b_remove_channel->setPixmap(KGlobal::iconLoader()->loadIcon("2leftarrow", KIcon::Toolbar));
	
	b_Channel_Up->setPixmap(KGlobal::iconLoader()->loadIcon("up", KIcon::Toolbar));
	b_Channel_Down->setPixmap(KGlobal::iconLoader()->loadIcon("down", KIcon::Toolbar));
}

void channellist::removeChannel()
{
  	new TvStation(list_inactive,dynamic_cast<TvStation*> (list_active->item(list_active->currentItem()))->fullPath);
	
	list_active-> removeItem (list_active->currentItem());
}

void channellist::addChannel()
{
	new TvStation(list_active,dynamic_cast<TvStation*> (list_inactive->item(list_inactive->currentItem()))->fullPath);
  
 	list_inactive-> removeItem (list_inactive->currentItem());
}

void channellist::moveChannelUp()
{
      int i = list_active->currentItem();
      
      if (i!=0)
      {
      	new TvStation(list_active,dynamic_cast<TvStation*> (list_active->item(i-1))->fullPath, dynamic_cast<TvStation*>(list_active->item(i) ));
      
      	list_active->removeItem(i-1);
      	list_active->setSelected(i-1,TRUE);
      	list_active->setSelected(i,FALSE);
      }
}

void channellist::moveChannelDown()
{
      int i=list_active->currentItem () ;
      
      if (i!=list_active->numRows ()-1 )
      {
      	new TvStation(list_active,dynamic_cast<TvStation*> (list_active->item(i))->fullPath, dynamic_cast<TvStation*>(list_active->item(i+1) ));
      
      	list_active->removeItem(i);
      	list_active->setSelected(i,FALSE);
      	list_active->setSelected(i+1,TRUE);	
      }
}

void channellist::openFile()
{
    Q_ULONG maxlenght=80;
    QFile channelFile(Config().m_SettingsFile);
    
    if (channelFile.open(IO_ReadOnly) )
    {
    	
	QString currentChannel;
	
    	while ( !channelFile.atEnd() )
	{
		
		channelFile.readLine(currentChannel, maxlenght);
		
		if ( currentChannel.contains("channel") !=0 ) // some config files have extra info
		{
			
			if (currentChannel.startsWith( "#" ) )
			{
				(void) new TvStation(list_inactive,currentChannel);
			} else
			{
				(void) new TvStation(list_active,currentChannel);
			}
		}
		else
		{
		extraFileInfo << currentChannel;
		}
		
	}
    }
    else
    {
    KMessageBox::error (this, "I was not able to open the configuration file. Please check your configuration. This file is created by your tv_grab_xx", "File not found") ;
    qDebug ("Error no such file");
    b_add_channel->setEnabled(false);
    b_remove_channel->setEnabled(false);
    b_Submit->setEnabled(false);
    b_Channel_Up->setEnabled(false);
    b_Channel_Down->setEnabled(false);
    }
	channelFile.close();
}


void channellist::UpdateConfigFile()
{
	QFile channelFile(Config().m_SettingsFile);
	KURL url=Config().m_SettingsFile;
	QDir dir(url.path());
	dir.remove ( url.fileName(), TRUE );
	
	channelFile.open( IO_WriteOnly );
	QTextStream ts( &channelFile );
	
	for ( QStringList::Iterator it = extraFileInfo.begin(); it != extraFileInfo.end(); ++it ) 
	{
           ts << *it;
        }
	
	TvStation * tvitem = dynamic_cast<TvStation*>(list_inactive->firstItem() );

	while (tvitem !=0)
	{
		QString channelName=tvitem->fullPath;
	
		if (channelName.startsWith("#"))
		{
			ts << channelName;
		}else
		{
			ts <<"#" <<channelName;
		}
		
	tvitem = dynamic_cast<TvStation*>(tvitem->next() );
	}
	
	tvitem = dynamic_cast<TvStation*>(list_active->firstItem() );
	
	while (tvitem !=0)
	{
		QString channelName=tvitem->fullPath;
		
		if (!channelName.startsWith("#"))
		{
			ts << channelName;
		}else
		{
			ts << channelName.mid(1,(channelName.length()-1));
		}
		
	tvitem = dynamic_cast<TvStation*>(tvitem->next() );
	}

	channelFile.close();
	emit channelsChanged();
	
	
 this->close();
}

void channellist::setChannelMap(Channelmap tempChannels)
{
	Channels = tempChannels;
}

// Class for working with the channles. Get name, pict etc

TvStation::TvStation(QListBox * listbox, QString station ):QListBoxItem(listbox)

{
	fullPath=station;
	getName();
	setText(Name);
	
	pictExists = TRUE;
	getPict();
}

TvStation::TvStation ( QListBox* listbox, QString station, TvStation* after):QListBoxItem(listbox, after)

{
	fullPath=station;
	getName();
	setText(Name);
	
	getPict();

}

TvStation::~ TvStation()
{
}

void TvStation::getName()
{
	QRegExp sep("[\\s ;]");
	
	//German xmltv has in 1st position the channel names and the second starts with # So if it not a german config, continue
	if (!fullPath.section(sep,2,2).startsWith("#")) 
		{
	 	Name=fullPath.section(sep, 2, 2);		
		
		// Norviegian xmltv does not have any channel name. Please complaints to the tv_grab_no maintainer
		if (Name==QString::null)
			{
			Name= "Channel No : "+fullPath.section(sep,1,1);
			// There is no Pict to show
			pictExists = FALSE;
			}
		}
		
	if (fullPath.section(sep,2,2).startsWith("#")) // This is a German channel
	{
		Name=fullPath.section(sep,1,1).replace("%20"," ");
	}
}

void TvStation::getPict()
{
	if (pictExists) // only continue if it exists
	{
		KStandardDirs *dirs = new KStandardDirs();
		QString	home = dirs->localkdedir () + "/share/apps/ktvschedule/";
		QString pictpath = home + Name.lower().remove(" ")+".gif";
	
  		if (!Pict.load( pictpath ))
  		{ 
			// if pixmap not loaded then it is probably a jpg
			pictpath.replace("gif","jpg");
			
		}
	
		if (!Pict.load( pictpath ) )
		{ 
			qDebug("Loading Pict from remote location");
			// Still didn't find the pix we will get it using the config info stored before
			if (Config().picURL.contains("http://")!=0) // it is in a remote location
			{
				QUrlOperator *urlOp = new QUrlOperator;
				pictpath = Config().picURL + Name.remove(" ")+".gif";
				urlOp->copy(pictpath, home);

			// Try also a .jpg
				pictpath = Config().picURL + Name.remove(" ")+".jpg";
				urlOp->copy(pictpath, home);
			}
			else if(Config().picURL.contains("file:///"))
			{ 	 
				QUrlOperator *urlOp = new QUrlOperator;
				pictpath = Config().picURL + Name.remove(" ")+".gif";
				urlOp->copy(pictpath, home+Name.lower().remove(" ")+".gif",FALSE,FALSE);
				
				// Try also a .jpg 	 
				pictpath = Config().picURL + Name.remove(" ")+".jpg";
				urlOp->copy(pictpath, home+Name.lower().remove(" ")+".jpg",FALSE,FALSE);
			}
			else
			{
				qDebug("Pict is of unknown type"); 	 
			}
		}
	}
}

void TvStation::paint( QPainter *painter )
{
	int w = width(listBox()) ;
	int h = height(listBox()) ;
	   if ( isSelected() )
	   { 
	   painter->fillRect(QRect(0, 0, w, h), listBox()->colorGroup().highlight()) ;
	   painter->setPen(listBox()->colorGroup().highlightedText()) ;
	   }
	   else
	   { 
	   painter->fillRect(QRect(0, 0, w, h), listBox()->colorGroup().base()) ;
	   painter->setPen(listBox()->colorGroup().text()) ;
	   }

	painter->setPen( Qt::blue );
       // painter->drawText( rect(), AlignCenter, "The Text" );
       int yPos = listBox()->itemHeight()/2 - Pict.height()/2;
       painter->drawPixmap(0, yPos, Pict);
	painter->drawText(55,listBox()->itemHeight()/2,text());
}



#include "channellist.moc"

