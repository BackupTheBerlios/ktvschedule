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
#include "tvguidetable.h"
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qcursor.h>
#include <qdir.h>
#include <qregexp.h> 
#include <kmessagebox.h>
#include <kpassivepopup.h> 
#include <kglobal.h>
#include <kiconloader.h>
#include <knotifyclient.h>
#include <klocale.h>

#include <qframe.h>
#include <qfont.h>
// Creates a custom QTable

TvGuideTable::TvGuideTable(QWidget *parent, const char *name)
 : QTable(parent, name)
{

	lastFoundRow=0;
	lastFoundCol=0;
	
	setGeometry( QRect( 10, 10, 630, 300 )); // Geometry of the QTable
	CreateRows(); // Add rows.
	
	// Adds a connection for right-click
	connect (this, SIGNAL (contextMenuRequested( int , int , const QPoint &  ) ), this, SLOT(showDetails(int , int )));
	
	// Tracking of current Shows
	updateTimer = new QTimer();
        connect( updateTimer, SIGNAL(timeout()), this, SLOT( SetToCurrentTime() ) );
        enableTraking();
	
	// Update the table every minute for changes of the current program
	QTimer *timer = new QTimer();
        connect( timer, SIGNAL(timeout()), this, SLOT(UpdateTable() ) );
        timer->start ( (1*60*1000) );
}


TvGuideTable::~TvGuideTable()
{
}

void TvGuideTable::enableTraking()
{

	if ( ( Config().m_setToCurrentTime ) && (!updateTimer->isActive() ) )
	{
		updateTimer->start ( (1*60*1000) );
	}
	if ( !Config().m_setToCurrentTime )
	{
		updateTimer->stop();
	}

}

// Create the rows. We choose a time interval of 5 minutes.
void TvGuideTable::CreateRows()
{
  int TimeInterval=5;
  
  int Intervals=(60/TimeInterval);
  int NumberOfRows=(24*Intervals);
  
  this->insertRows(0,NumberOfRows);
  
  // Adjust row size depending on the size that the user has set
  
  for (int rows=0; rows<NumberOfRows;rows++)
  {
  	this->setRowHeight ( rows, Config().m_RowSize );
  }
  
  QStringList RowHeaders;
  int Hour=0;
  int Minutes=0;
 
   for( int i=0;i<24 ;i++ )
  	{
	Minutes=0;
  	for( int j=0;j<Intervals ;j++ )
		{
		QString RowHeader;
		if (Hour<10)
			{
			RowHeader.append("0");
			}
					
		RowHeader.append(QString::number(Hour));
		RowHeader.append(":");
		
		if (Minutes<10)
			{
			RowHeader.append("0");
			}
			
		RowHeader.append(QString::number(Minutes));

		RowHeaders.append(RowHeader);
		
		Minutes+=TimeInterval;
		}
	Hour++;
	}
	
	this->setRowLabels(RowHeaders);
	this->verticalHeader()->adjustHeaderSize ();
}

// Clears all Columns of the Table. This is needed if we have refreshed our XML Guide
void TvGuideTable::removeAllColumns()
{
	if (this->numCols() !=0)
	{
		int totalCols= this->numCols();
		for( int i=0; i < totalCols ;i++ )
		{
			this->removeColumn( 0 );
		}
	}
	
}

// Creates the Columns from the QMap
void TvGuideTable::CreateColumns(QMap<QString,ChannelNameIcon> Channels)
{
	//QStringList ColumnHeader;
	int NumberOfCols=0;
	
	QHeader *pHeader = this-> horizontalHeader();
	Channelmap::Iterator it;
	
	for ( it = Channels.begin(); it != Channels.end(); ++it ) 
	{
		// Adds also a icon for each channel
		this->insertColumns(NumberOfCols);
		
		QPixmap pic( it.data().ChannelIcon );
		
		QIconSet iconSet( pic );
		
		pHeader->setLabel( NumberOfCols, iconSet, it.data().ChannelName );

		NumberOfCols++;
        }

}

// Adds a TvItem to the Cell.
void TvGuideTable::setCell ( TvItem* tv,QMap<QString,ChannelNameIcon> Channels, QDate ShowDate)
{	
	Channelmap::Iterator it;
	it=Channels.begin();
	int colnum=0;
	
	while (it.key()!=tv->ChannelId) //search for the channel name
	{
	it++;
	}
	
	while (colnum != this->numCols ()-1)
	{
	  if (it.data().ChannelName == this->horizontalHeader()->label(colnum) )
	  {
	  	break;
	  }
	  colnum++;
	}

	int StartRow=0;
	
	// If the starttime was yesterday, we start at row 0
	if ( tv->StartTime.date() < ShowDate )
	{
		StartRow = 0;
	}
		else
	{
		StartRow=tv->StartTime.time().hour()*12;
		StartRow=StartRow+(tv->StartTime.time().minute()/5);
	}
	
	int StopRow=0;
	
	// If the stoptime is tomorrow, we stop at the end of the day
	if ( tv->StopTime.date() > ShowDate )
	{
		StopRow=(24*(60/5));
	}
		else
	{
		StopRow=tv->StopTime.time().hour()*12;
		StopRow=StopRow+(tv->StopTime.time().minute()/5);
	}
	
	
	tv->setWordWrap(TRUE);
	tv->setText(tv->Title+"\n"+tv->SubTitle); // Add the Title as text of the cell
	tv->setColor(); // Sets the color the background should be
	
	this-> setItem(StartRow,colnum,tv); // Paints the cell by inserting the item to the table
	
	tv->setSpan ( (StopRow-StartRow),1 ); // Spans the cell over the time it is running
}

// Clears all cells. Usefull if we change date
void TvGuideTable::clearCells()
{
	int imax=this->numCols();
	int jmax=this->numRows();

	for(int i=0; i<imax; i++ )
	{
		for (int j=0;j<jmax;j++)
		{
		this->clearCell ( j, i );
		}
	}
}

// Shows the details if right clicked on the cell
void TvGuideTable::showDetails(int row, int col)
{
	currentTvItem = dynamic_cast<TvItem*> (item(row, col)); // we get the TvItem of the current cell
	
	if (currentTvItem != NULL)
	 {
	
		QPopupMenu *details = new QPopupMenu; // Creates a QPopupMenu
		details->setFrameShape(QFrame::PopupPanel);
		
		Q_CHECK_PTR( details );
		
		if ( !!( Config().m_DetailsInt & val_Title) ) 
		{ details->insertItem(QString(i18n("Title")+" : %1").arg(currentTvItem->Title)) ; }
		
		if ( !!(Config().m_DetailsInt & val_SubTitle) )
		{
			if (!currentTvItem->SubTitle.isNull() )
			{
				details->insertItem(QString(i18n("SubTitle")+" : %1").arg(currentTvItem->SubTitle));
			}
		}
		
		QPopupMenu *CreditsDetails = new QPopupMenu(details);
		QPopupMenu *ActorDetails = new QPopupMenu(CreditsDetails);
		QPopupMenu *GuestDetails = new QPopupMenu(CreditsDetails);
		QPopupMenu *ProducersDetails = new QPopupMenu(CreditsDetails);
				
		bool Actors=FALSE;
		bool Guests=FALSE;
		bool Producers=FALSE;
		
		int IndexOfActor=0;
		
		for ( QStringList::Iterator it = currentTvItem->Actors.begin(); it != currentTvItem->Actors.end(); ++it ) 
		 {
		 	ActorDetails->insertItem(*it,IndexOfActor);
			Actors=TRUE;
			IndexOfActor++;
		 }
		 
		for ( QStringList::Iterator it = currentTvItem->Guests.begin(); it != currentTvItem->Guests.end(); ++it ) 
		 {
		 	GuestDetails->insertItem(*it);
			Guests=TRUE;
		 }
		 
		for ( QStringList::Iterator it = currentTvItem->Producers.begin(); it != currentTvItem->Producers.end(); ++it ) 
		 {
		 	ProducersDetails->insertItem(*it);
			Producers=TRUE;
		 }
		
		 if (( Actors || Guests || Producers ) && !!( Config().m_DetailsInt & val_Credits ) )
		 {
			details->insertItem( i18n("Credits") ,CreditsDetails );
			if (Actors && !!( Config().m_DetailsInt & val_Actors ) )
			{
				CreditsDetails->insertItem( i18n("Actors"), ActorDetails );
				connect(ActorDetails, SIGNAL(activated (int)), this, SLOT(ImdbDetails(int)));
			}
			if (Guests && !!( Config().m_DetailsInt & val_Guests ) )
			{
				CreditsDetails->insertItem( i18n("Guest"), GuestDetails );
			}
			if (Producers && !!( Config().m_DetailsInt & val_Producers ) )
			{
				CreditsDetails->insertItem( i18n("Producers"), ProducersDetails );
			}
		}
		
		
		if ( (currentTvItem->EpisodeNum_onscreen !=-1) && !!( Config().m_DetailsInt & val_EpisodeNumber) )
		{
			details->insertItem(QString(i18n("Episode number")+" : %1").arg(currentTvItem->EpisodeNum_onscreen));
		}
		
		if ( !! (Config().m_DetailsInt & val_Rating) )
		{
		if (currentTvItem->Stars==-1)
				{
				details->insertItem(i18n("No Rating"));
				}
			else
				{
				QString str;
				str.fill('*',currentTvItem->Stars); // Adds stars to rating
				details->insertItem(QString(i18n("Rating")+" : %1").arg(str) ) ;
				}
		}
		
		if (!currentTvItem->Description.isNull() && !!(Config().m_DetailsInt & val_Description) ) 
		{
			details -> insertItem(new RichCustomMenuItem ( QString(i18n("Description")+" : %1 \n").arg(currentTvItem->Description)));
		}
			
		details->insertSeparator();
		// Calculates the program duration and displays it also
		if (!!(Config().m_DetailsInt & val_TotalPlayTime) )
		{
			QTime ProgramTime(0,0);
			ProgramTime=ProgramTime.addSecs( currentTvItem->StartTime.secsTo(currentTvItem->StopTime) );
			details->insertItem(QString(i18n("Total Play Time")+" : %1").arg(ProgramTime.toString("hh:mm") ) );
		}
		
		//details->insertItem(QString("Category : %1").arg(currentTvItem->Category) ,5);
		if ( !!(Config().m_DetailsInt & val_Categories) )
		{ 
			 QString catItem;
		 	for ( QStringList::Iterator it = currentTvItem->Category.begin(); it != currentTvItem->Category.end(); ++it ) 
		 	{
		 		 catItem+=*it;
				 catItem+=" ";
		 	}
			details->insertItem(QString(i18n("Category")+" : %1").arg(catItem));
		}
		 
		if (!!( Config().m_DetailsInt & val_Language) )
		{ details->insertItem(QString(i18n("Language")+" : %1").arg(currentTvItem->Lang));}
		
		if ( (!currentTvItem->ShowView == -1 ) && !!( Config().m_DetailsInt & val_ShowView) )
		{
		 details->insertItem(QString(i18n("Show view")+" : %1").arg(QString::number(currentTvItem->ShowView)));
		}
		
		if ( (!currentTvItem->RatingSystem.isNull() ) && !!( Config().m_DetailsInt & val_PRRatingsystem))
		{
		  details->insertItem(QString(i18n("PR Rating system")+" : %1, "+i18n("Value")+" : %2").arg(currentTvItem->RatingSystem).arg(currentTvItem->RatingValue));
		}
		
		
		
		if (!!( Config().m_DetailsInt & val_Scheduler) )
		{
			details->insertSeparator();
		
			QPopupMenu *ScheduleMenu= new QPopupMenu (details);
			ScheduleMenu->insertItem(i18n("Schedule Program"),this, SLOT(scheduleReminder() ));
			ScheduleMenu->insertItem(i18n("Record Program (needs Kalva)"), this, SLOT (recordProgram() ) );
		
			details->insertItem(i18n("Scheduler"), ScheduleMenu);
		}
		
		//details->exec( QCursor::pos() );
		details->popup( QCursor::pos() );
	 }
	 
}

void TvGuideTable::scheduleReminder() /// Emits signal to the main Window in order to schedule a Reminder
{

	emit infoContext(currentTvItem);

}

void TvGuideTable::recordProgram() /// Emits signal to the main Window in order to schedule a recording using kalva
{

	emit recordProgramSignal(currentTvItem);

}

void TvGuideTable::ImdbDetails(int ActorIndex) /// Opens konqueror window and looks up the name in IMDB
{
	QStringList::Iterator it = currentTvItem->Actors.at(ActorIndex);
	QString ActorURL = *it;
	if(system("kfmclient openURL http://www.imdb.com/find?q="+ActorURL.replace(" ","%20")+";s=all &")!=0)
	{
		KMessageBox::error (this, "Could not start webbrowser", "Browser Error") ;
   		qDebug ("Could not start webbrowser");
	}
}

// Move the grid to the current Time. Current time is displayed in the middle
void TvGuideTable::SetToCurrentTime()
{
	int row = ((QTime::currentTime()).hour()*12+(QTime::currentTime()).minute()/5); //*this->rowHeight(1);
	int cellsVisible=this->visibleHeight ()/this->rowHeight(1); 
	int centralCell;
	
	if (this->currentRow () > row)
		{
		centralCell=row-cellsVisible/2;
		}
	else
		{
		centralCell=row+cellsVisible/2;
		}	
	
	this->ensureCellVisible ( centralCell , currentColumn() );
	this->setCurrentCell ( row, currentColumn() );
}

// Updates the Table due to color changes and every minute
void TvGuideTable::UpdateTable()
{

	int imax=this->numCols();
	int jmax=this->numRows();

	bool showNew = FALSE; // Check if the show just begun to be showed
	QString newShows= ""; // Here we will construct the message to be displayed
	QStringList NowPlaying;
	
	//first we resize the rows.
	for (int i=0; i<jmax; i++)
	{
		this->setRowHeight ( i, Config().m_RowSize );
	}
	
	// Scans every cell for changes in colors
	for(int i=0; i<imax; i++ )
	{
		for (int j=0;j<jmax-1;j++)
		{
			TvItem *tvitem=dynamic_cast<TvItem*> (item(j, i));
			
			if (tvitem !=NULL)
			{
				QString before = tvitem->getStatus();
				
				tvitem->setCurrentStatus();
				tvitem->setColor();
				tvitem->m_crTxt = Config().m_ColorText;
				
				this->updateCell ( j,i );

			if ( (tvitem->getStatus() != before) && (tvitem->getStatus() == "Current" ) )
			{
				newShows += "<br><u><b>";
				newShows += this->horizontalHeader()->label( i ); //Get the Channel Name from the Header
				newShows += "</b></u> : ";
				newShows += tvitem->Title; // Displays the Title
				newShows += "<br>";
				showNew = TRUE;
			}
			
			//Exprimental
			if (tvitem->getStatus() == "Current")
			{
				bool add=TRUE;
				for ( QStringList::Iterator it = NowPlaying.begin(); it != NowPlaying.end();it++)
				{
					if( *it == tvitem->Title)
					{
					add=FALSE;
					break;
					}
				}
				
				if (add)
				{
				NowPlaying += this->horizontalHeader()->label( i ); //Get the Channel Name from the Header
				NowPlaying += tvitem->Title; // Displays the Title
				}
			} 
			
			}
		}
	}
	
	if (showNew) // In case at least 1 program begins now, we display a popup
	{
	KNotifyClient::event(winId(), "new-program",newShows);
	} 
	emit currentPlayingForSysTray( NowPlaying );
}

// Highlights all shows corresponding to the selected category
void TvGuideTable::highlightCategory(const QString& cat)
{

	// Init last found col and row back to 0
	lastFoundRow=0;
	lastFoundCol=0;
	
	//Now continue
	int imax=this->numCols();
	int jmax=this->numRows();

	for(int i=0; i<imax; i++ )
	{
		for (int j=0;j<jmax-1;j++)
		{
			TvItem *tvitem=dynamic_cast<TvItem*> (item(j, i));
			
			if (tvitem !=NULL)
			{
			
			for ( QStringList::Iterator it = tvitem->Category.begin(); it != tvitem->Category.end(); ++it ) 
			{
				QString CatItem=*it;
				if (CatItem.lower() == cat )
				{
					tvitem->m_crTxt = Config().m_ColorSelected;
					break;
				}
				else
				{
					tvitem->m_crTxt = Config().m_ColorText;
				}
			}	
				
				/*if (tvitem->Category.lower() == cat )
				{
					tvitem->m_crTxt = Config().m_ColorSelected;
				}
				else
				{
					tvitem->m_crTxt = Config().m_ColorText;
				}*/
				this->updateCell ( j,i );
			}
		}
	}

}

void TvGuideTable::findNextCategory(QString cat)
{
	int imax=this->numCols();
	int jmax=this->numRows();
	
	bool foundShow=FALSE;
	int i=lastFoundCol;
	int j=lastFoundRow;
	
	while ( !foundShow )
	{
	
	TvItem *tvitem=dynamic_cast<TvItem*> (item(j, i));
	
	if (tvitem !=NULL )
		{
		
		for ( QStringList::Iterator it = tvitem->Category.begin(); it != tvitem->Category.end(); ++it ) 
		{
		QString CatItem=*it;
		if (CatItem.lower() == cat )
				{
					this->setCurrentCell ( (j+tvitem->rowSpan()/2), i );
					this-> ensureCellVisible ( (j+tvitem->rowSpan()/2) , i );
					lastFoundRow = j+tvitem->rowSpan ()+1;
					lastFoundCol = i;
					foundShow=TRUE;
					break;
				}
		}
		
		}
		
	j+=1;
	
	if ( (i > imax -1) && (j > jmax-1) ) //if at end of table, begin from beginning again
		{
			i=0;
			j=0;
		}
	
	if (j > jmax-1) //increase column if we have done all rows
		{
			j=0;
			i+=1;
		}	
	}
}

/* TvItem class
A Custom QTableItem containing all the info on each show
*/


TvItem::TvItem(QTable *table,EditType et)
  : QTableItem(table,et)
 {
 
 	 this->Stars=-1;
	 this->ShowView=-1;
	 this->Description=QString::null;
	 this->SubTitle=QString::null;
	 this->RatingSystem=QString::null;
	 this->RatingValue=QString::null;
	 this->ChannelId=QString::null;
	 this->ChannelName=QString::null;
	 this->Title=QString::null;
	 this->EpisodeNum_ddProgid=QString::null;
	 this->EpisodeNum_onscreen=-1;
	 this->Live=QString::null;
 }
 
 TvItem::~TvItem()
 {
 }

 void TvItem::setColor()
 {
 
  if ( currentStatus == Past )
  	{
	m_crBkg = Config().m_ColorOfPast;
	}
  if ( currentStatus == Current )
  	{
	m_crBkg = Config().m_ColorOfActive;
	}
  if ( currentStatus == Futur )
  	{
	m_crBkg = Config().m_ColorOfFuture;
	} 
	
 }
 
 void TvItem::setCurrentStatus()
 {
 
  if ( StopTime < QDateTime::currentDateTime() )
  	{
	currentStatus = Past;
	}
  if (StartTime > QDateTime::currentDateTime() )
  	{
	currentStatus = Futur;
	}
  if ( (StartTime <= QDateTime::currentDateTime() ) && (StopTime >= QDateTime::currentDateTime() ) )
  	{
	currentStatus = Current;
	} 
	
 }
 
 void TvItem::paint(QPainter* p,const QColorGroup& cg,const QRect& cr,bool selected)
 {
 
  int w = cr.width();
  int h = cr.height();
  m_HorzAlign=Qt::AlignHCenter;
  m_VertAlign=Qt::AlignVCenter;

  if (selected && colSpan() == 1 )
  {
   //p->fillRect( 0 , 0 , w , h , cg.brush( QColorGroup::Highlight ) );
   //p->fillRect( 0 , 0 , w/4 , h , m_crBkg );
   p->setPen( cg.highlightedText() );
  }
  else
  {
   
   int Left=0;
   
   if (this->currentStatus!=Current)
   {
      for (int i=100; i<200; i=i+5)
   	{
   		p->fillRect(Left, 0, 2, h, m_crBkg.light(i) );
		Left+=2;
   	}
	p->fillRect(Left, 0 , w-Left, h, QColor("#FFFFFF") );	
   } else
   {
   	p->fillRect(1,1, w, h, m_crBkg.light(190) );
	p->setPen( m_crBkg );
	p->drawRect ( 0, 0, w-1, h-1);
	
	// Draw a line of current time
	if ( (this->StartTime.daysTo(this->StopTime) != 0) && (this->StartTime.date().day() != QDate::currentDate().day()) )
	{
	  int currentPosition = -this->StopTime.secsTo(QDateTime::currentDateTime() )/60*Config().m_RowSize/5;
	  p->fillRect(0, h-currentPosition, w, 1, QColor("#517D36"));
	}
	else
	{
	  int currentPosition = this->StartTime.secsTo(QDateTime::currentDateTime() )/60*Config().m_RowSize/5;
	  p->fillRect(0, currentPosition, w, 1, QColor("#517D36"));
	}
   }
   
   p->setPen( m_crTxt );
  }
 
  QFont font;
  
  // If the time is less than 10mins, then use smaller font
  
  if (this->StartTime.secsTo(this->StopTime) < (60*10) )
  {
  	font.setPointSize ( font.pointSize ()-2 );
  }
  
  font.setBold(TRUE);
  p->setFont(font);
  p->setPen ( m_crTxt );

  if (this->SubTitle == QString::null )
  	{ 
		p->drawText( 2 , 0, w - 4 , h , wordWrap() ? ( m_HorzAlign| Qt::AlignVCenter |WordBreak) : m_HorzAlign|Qt::AlignBottom , this->Title );
	} else 
	{ 
	        p->drawText( 2 , 0, w - 4 , h/2 , wordWrap() ? ( m_HorzAlign| Qt::AlignBottom |WordBreak) : m_HorzAlign|Qt::AlignBottom , this->Title );
  		
		font.setBold(FALSE);
  		p->setFont(font);
  		p->drawText( 2 , h/2 , w - 4 , h , wordWrap() ? ( m_HorzAlign | Qt::AlignTop |WordBreak) : m_HorzAlign|Qt::AlignTop , this->SubTitle );
	}
	
	if (this->Live =="live" ) // There is a live information
	{
		// If yes, add live Icon at top left. ! The ktvschedule-live icon does not install by it self :(
  		p->drawPixmap ( 0, 0, UserIcon("live_show", KIcon::Toolbar) );
	}
 }
 
 QString TvItem::getStatus()
 {
 
  	switch (this->currentStatus)
  	{
 	case Past:
		return "Past";
		break;
	case Current:
		return "Current";
		break;
	case Futur:
		return "Futur";
		break;
	}
 }
  
#include "tvguidetable.moc"
