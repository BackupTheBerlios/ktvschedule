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
#include "ktvschedule.h"
#include "channellist.h"

#include <qfile.h>
#include <qmap.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qstringlist.h> 
#include <qstring.h>  
#include <qtable.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <qmessagebox.h> 
#include <qlabel.h>
#include <qprogressdialog.h> 
#include <qthread.h>
#include <qdir.h>
#include <qobject.h>
#include <qurloperator.h> 
#include <qnetwork.h>
#include <qregexp.h>

#include <kaboutdata.h>
#include <kaboutapplication.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kurlrequester.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kmenubar.h> 
#include <ktoolbar.h>
#include <qtimer.h>
#include <qregexp.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <qurloperator.h> 

#include <knotifydialog.h> 
#include <khelpmenu.h>

ktvschedule::ktvschedule() : KMainWindow(), m_prefDialog(0)
{	
	 // Initialise Toolbars, Menus, StatusBar and some other	 
	 init();
	 
	 // Read Configuration file
	 Config().read();
	 
	 // Loads XMLFile, process it and displays contents for current date
	 LoadFileAndData();
	 // Go to the current Grid Time
	 t_TVGrid->SetToCurrentTime();
	 
	 // Connections the Now Playing action with the moving of the table
	 connect (t_TVGrid, SIGNAL(contentsMoving (int, int)), this , SLOT(enableGoHomeButton() ) );
	 actionCollection()->action("Now Playing")->setEnabled( FALSE );
	 
	 if (!Config().m_FirstRun)
	 {
	 	KMessageBox::warningContinueCancel (0, "It seems that it is the first time you run kTvSchedule. Please make sure that you have runned your tv_grab_xx at least once");
		Config().m_FirstRun = TRUE;
		Config().write();
	}
	
	setupSystemTray();
}

ktvschedule::~ktvschedule()
{
}

// Initialise Toolbar, Menus ScrollBar and other
void ktvschedule::init()
{
	// Creates central widgets
	
	tvgridbox = new QGroupBox ( this ); // Group box
	tvgridbox->setInsideMargin ( 15 );
		
	t_TVGrid=new TvGuideTable(this->tvgridbox, "t_TVGrid"); // Add TvGuideTable custom QTable
	
	QGridLayout *tvgridboxLayout = new QGridLayout( tvgridbox->layout() ); //Layout
	tvgridboxLayout->setAlignment ( Qt::AlignTop );
	tvgridboxLayout->addWidget ( t_TVGrid , 0 , 0 );
		
	setCentralWidget( tvgridbox );
  	tvgridbox->show();
	
////////////////////////////////
	//Check for home directory, if it does not exist, create it
	
       QString qstr(QDir::homeDirPath()+"/.kde/share/apps/ktvschedule"); //for some reason this only works with gcc 3.3 and not the previous version
       QDir d(qstr);
       if ( !d.exists() )
	{
		d.mkdir(qstr);
	}
	
	/*QDir d( QString(QDir::homeDirPath()+"/.kde/share/apps/ktvschedule") );
	if ( !d.exists() )
		{
			d.mkdir(QString(QDir::homeDirPath()+"/.kde/share/apps/ktvschedule"));
		}*/ // This worked for gcc 3.4 but not for 3.3

	
	//setRightJustification( TRUE );
	
////////////////////////////////

	this->setIcon( KGlobal::iconLoader()->loadIcon("ktvschedule", KIcon::Desktop) );
	
	// Creates the ToolBar
	KToolBar * TvToolBar = new KToolBar( this, KMainWindow::Top );
	
//////////////////////////////

	// Creates a QDateEdit Widget for future use
	date_show = new QDateEdit( QDate::currentDate() , TvToolBar );
	date_show->setOrder(QDateEdit::DMY);
	
////////////////////////////////
	// Define KActions
	
	 //KAction *actionGoHome = new KAction ( "Now Playing", KGlobal::iconLoader()->loadIcon("gohome", KIcon::NoGroup), CTRL+Key_P , t_TVGrid, SLOT (SetToCurrentTime() ), collector, "Now Playing");
	 
	 KAction *actionGoHome = new KAction ( i18n("Now &Playing"), "gohome", CTRL+Key_P , this, SLOT (SetToCurrentTime() ), actionCollection(), "Now Playing");

	 KAction *actionRefreshGuide = new KAction ( i18n("&Refresh Guide"), "reload", CTRL+Key_R , this, SLOT (RefreshXMLGuide() ), actionCollection(), "Refresh Guide");
	 
	 KAction *actionManageChannels = new KAction ( i18n("&Manage Channels"), "toggle_log", CTRL+Key_M , this, SLOT (channelList() ), actionCollection(), "Manage Channels");
	 
	 KAction *actionConfigure = new KAction ( i18n("&Configure"), "configure", CTRL+Key_C , this, SLOT (runConfig() ), actionCollection(), "Configure");
	 	 
	 KAction *actionExit = KStdAction::quit(this, SLOT(close()), 0);

	 KAction *actionPrevious = new KAction ( i18n("Pre&vious Day"), "previous", CTRL+Key_V , date_show, SLOT ( stepDown() ), actionCollection(), "PreviousDay");
 
	 KAction *actionNext = new KAction ( i18n("&Next Day"), "forward", CTRL+Key_N , date_show, SLOT ( stepUp() ), actionCollection(), "NextDay");
	 
	 KAction *actionEvent = KStdAction::configureNotifications(this, SLOT( notificationsEvent() ), actionCollection(), "settings_notifications" );
	 
	 KAction *actionFindCategory = new KAction (i18n("&Find Next Show"), "find", CTRL+Key_F , this, SLOT( findNextCategory() ), actionCollection(), "findnextshow");

//////////////////////////////////

	// Plug actions to ToolBar
	 
	
	 actionGoHome->plug ( TvToolBar);
	 actionRefreshGuide->plug (TvToolBar);
	 actionExit->plug (TvToolBar);
	 
	 TvToolBar->insertLineSeparator();
	 
	 actionManageChannels->plug (TvToolBar);
	 actionConfigure->plug (TvToolBar);

	 TvToolBar->insertLineSeparator();
	 
	 actionPrevious->plug (TvToolBar);
	 
	 // Add a QDateEdit widget for the ToolBar
	 TvToolBar->insertWidget (actionCollection()->count()+1,13, date_show );
	 
	 actionNext->plug (TvToolBar);
	 
	// Creates the KComboBox and adds it to the Toolbar
	 
	combo_Categories = new KComboBox(TvToolBar);
	combo_Categories->setMinimumWidth( combo_Categories->width ()+60); 
	combo_Categories->insertStringList(Categories);
	
	TvToolBar->insertWidget (actionCollection()->count()+1,15, combo_Categories);
	
	actionFindCategory->plug (TvToolBar);
//////////////////////////////////

	// Create menus
	 
	KPopupMenu *menuGrid = new KPopupMenu( this );
	KPopupMenu *menuConf = new KPopupMenu( this );
	
	menuBar()->insertItem( i18n("&Grid"), menuGrid );
  	menuBar()->insertItem( i18n("&Configure"), menuConf );
	
	// Plug KActions to menus
	
	actionGoHome->plug(menuGrid);
	actionRefreshGuide->plug(menuGrid);
	actionExit->plug(menuGrid);
	
	actionManageChannels->plug(menuConf);
	actionConfigure->plug(menuConf);
        actionEvent->plug(menuConf);
	
	KAboutData *_aboutData = new KAboutData("kTvSchedule", I18N_NOOP("KTvSchedule"), 
              "0.1.8", I18N_NOOP("TV Schedule.\n\n"
              "KDE Front end for TV Schedule programs.\n"
              "created by xmltv (tv_grab_xx)"),
              KAboutData::License_GPL, "(c) 2005, Alexios Beveratos",0,"http://www.kde-apps.org/content/show.php?content=22747","alexios.beveratos@gmail.com");
	_aboutData->addAuthor("Alexios Beveratos", I18N_NOOP("Main Developper"), "alexios.beveratos@gmail.com");
	_aboutData->addAuthor("Benoit Philipps",0,"benoit.philipps@gmail.com");
	//_aboutData->setHomepage("http://www.kde-apps.org/content/show.php?content=22747");
	//_aboutData->setBugAddress("http://developer.berlios.de/projects/ktvschedule/");
	
	 KHelpMenu *HelpMenu = new KHelpMenu( this, _aboutData );
 	 menuBar()->insertItem(i18n("&Help"), HelpMenu->menu() );	
	
//////////////////////////////////

	// Creates StatusBar
			  
	StatusBar = this->statusBar();
	led = new KLed(QColor ("#2EBB16"), KLed::On, KLed::Raised, KLed::Circular);
	StatusBar->addWidget ( led, 0, TRUE );
	
//////////////////////////////////

	// Other Initialisations
	
	date_show->setDate(QDate::currentDate ());
	date_show->setOrder(QDateEdit::DMY);
	
	FirstInput=QDate::currentDate();
	LastInput=QDate::currentDate();

//////////////////////////////////

	//Add connections
	connect( date_show, SIGNAL( valueChanged(const QDate&) ), this, SLOT( PopulateTvGrid() ) );
	//connect( combo_Categories, SIGNAL( activated(const QString &)),t_TVGrid,SLOT( highlightCategory(const QString& )));
	connect( combo_Categories, SIGNAL( activated(const QString &)),this,SLOT( highlightCategory(const QString& )));
	
	// Creates the scheduler class, and connects it. can be made better
	
	tvShowScheduler= new tvscheduler(); 
	connect( t_TVGrid, SIGNAL (infoContext(TvItem*) ), tvShowScheduler, SLOT (doNotMiss(TvItem*) ) );
	connect( t_TVGrid, SIGNAL (recordProgramSignal(TvItem* ) ), tvShowScheduler, SLOT (recordProgram(TvItem* ) ) );
	
/////////////////////////////////
	//If we use nxtvepg then disable some buttons
	if (Config().m_GrabberToUse == "nxtvepg")
	{
		actionRefreshGuide->setEnabled(FALSE);
		actionManageChannels->setEnabled(FALSE);
	}
}

//Loads XML file
bool ktvschedule::loadXMLFile(const QString& XMLFile)
{

//Opens XML File
 QFile file( XMLFile );
 
 if( !file.open( IO_ReadOnly ) )
   {
   KMessageBox::error (this, "I was not able to open the XML file. Please check your configuration", "File not found") ;
   qDebug("Could not open file");
   return FALSE;
   }
   else
   {
	//and sets contents to the TvXML. This is loaded into the memory
	 if( !TvXML.setContent( &file ) )
 	{
		KMessageBox::error (this, "There was an error loading the XML file. It is maybe not critical", "XML Error") ;
   		qDebug ("There was an error loading the XML file");;
 	}
    file.close();
    return TRUE;
    }
}
 
void ktvschedule::LoadFileAndData()
{
	//Loads the XML file into memory and appends it to a DomDoc	
	 if ( this->loadXMLFile( Config().m_ScheduleFile ) )  // Continue only if ok
	 {
	 	// Get channel name to channel Id
	 	this->correspondChannels();
	 	// Reads XML File and populates the Tv Grid
	 	this->PopulateTvGrid();
		this->sortTvTable();
	}

}

void ktvschedule::correspondChannels()
{

	// clears all Columns before creating new ones
	t_TVGrid->removeAllColumns();
	Channels.clear();
	//
	     qInitNetworkProtocols ();
	     KStandardDirs *dirs = new KStandardDirs();
	     QString home = dirs->localkdedir () + "/share/apps/ktvschedule/";
	     QUrlOperator *urlOp = new QUrlOperator;
	//
	//Stats XML parsing
        QDomElement docElement = TvXML.documentElement();
	
	QDomNode node = docElement.firstChild();	
	
 	while( !node.isNull() )
 	{
	 QDomNode subnode=node.firstChild();
	 	 
	 QDomElement node_element=node.toElement();

	 ///////////////////////////
	 //If it is a channel, load channel name and Id

	 if (node_element.tagName()=="channel")
	  {
		ChannelNameIcon chanNameandIcon;
		bool Found = FALSE;

	 	while (!subnode.isNull())
		{
		
		QDomElement subnode_element=subnode.toElement();
		if (subnode_element.tagName()=="display-name")
			{
				if (!Found)
				{			
					chanNameandIcon.ChannelName = subnode_element.text();
					Found = TRUE;
				}
			//in fact some XML guides have many channel names. As from xmltv display names are : List the most canonical / common ones first and the most obscure names last. So we can stop.
			}
		
		if (subnode_element.tagName()=="icon")
		{
			chanNameandIcon.ChannelIcon = subnode_element.attribute("src","");

			// if the icon is on a remote location, try to fetch it localy
			if (chanNameandIcon.ChannelIcon.contains("http://")!=0)
			{
			  // if file does not exist localy, then download it
			  if (!QFile::exists(home + chanNameandIcon.ChannelIcon.section('/',-1) ) ) 
			  {
			  	urlOp->copy(chanNameandIcon.ChannelIcon, home); // donwload pixmap
			  }
			  
			   // Ok, we are going to save in the config file the url of the pics. It will be used in the channels to download missing pics.
			  QString temp = chanNameandIcon.ChannelIcon.section('/',-1); // We need a temp string
			  Config().picURL = chanNameandIcon.ChannelIcon.remove ( temp );
			  
			  // Change the location of the pixmap
			  chanNameandIcon.ChannelIcon = home + temp;
			}
			else if (chanNameandIcon.ChannelIcon.contains("file://")!=0)
			{
				// We create a temp string in order to config the picURL. Otherwise it removes usefull info from the channelIcon
				QString temp = chanNameandIcon.ChannelIcon;
				Config().picURL = temp.remove ( temp.section('/',-1) );
				
				QRegExp rx("^[0-9]{1,2}");
				QString dest=home + chanNameandIcon.ChannelName.lower().remove(" ").remove(rx)+"."+chanNameandIcon.ChannelIcon.right(3);
				if (!QFile::exists(dest))
				{
					urlOp->copy(chanNameandIcon.ChannelIcon,dest,FALSE,FALSE);
			 	}
				 // Change the location of the pixmap
			  	chanNameandIcon.ChannelIcon = dest;
				
			}
			else
			{
				qDebug("Pict is of unknown type"); 	 
			}
		}
			
		subnode=subnode.nextSibling();
			}
		
		Channels[node_element.attribute("id","")]=chanNameandIcon;
	  }
	  else
	  {
	  	// NxTvEPG have another field before the channels (about field). Do not stop
		if (node_element.tagName()!="about")
		{
			break; //no need to continue
		}
	  }
	node=node.nextSibling();
	}	
	
	// Creates collumns in the TvGrid. The QMap Channel has the channel name and id
	t_TVGrid->CreateColumns(Channels);
}

void ktvschedule::PopulateTvGrid()
{
        QDomElement docElement = TvXML.documentElement();
	QDomNode node = docElement.firstChild();
	tvgridbox->setTitle(date_show->date().toString("dddd") );
	
	//Clears the Category Combo
	combo_Categories->clear();
	Categories="All";
	
	this-> setCursor( Qt::WaitCursor ); //sets a cursor to WaitCursor
	
	t_TVGrid->clearCells(); // Clear all cells in the table
	
  	while( !node.isNull() )
  	{
 	
 	 TvItem *tv = new TvItem(t_TVGrid,QTableItem::Never); //Create a TvItem which will be inserted to the table
 	 
 	 QDomNode subnode=node.firstChild();
 	 
 	 QDomElement node_element=node.toElement();
 	 
	 if (Config().m_GrabberToUse == "tvgrab") // If we use tvgrab
	 {
	 	 this->tvgrabXML(tv,node_element,subnode); 
	 }
	 
	 if (Config().m_GrabberToUse == "nxtvepg")
	 {
	 	 this->nxtvepgXML(tv,node_element,subnode); 
	 }
	 		
	node=node.nextSibling();
	
	// Add only programs still running at date_show (duplicate of previous, but will keep)
	
		if ( (tv->StartTime.date() == date_show->date() ) || (tv->StopTime.date() == date_show->date() ) )
			{
				tv->setCurrentStatus(); // Set the status comparaing it to the current time
				t_TVGrid->setCell(tv, Channels, date_show->date()); //Adds the tv show to the grid.
			}
	}
	
	// Displays on the StatusBar the validity of the guide
	QString string = "Tv Guide valid from : "+FirstInput.toString("dd.MM.yyyy");
	string +=" to : "+LastInput.toString("dd.MM.yyyy");
	
	StatusBar->message(string);
	
	Categories.sort(); //Sorts the StringList for the ComboBox
	combo_Categories->insertStringList(Categories); // Add the StringList to the ComboBox
	
	this-> setCursor( Qt::ArrowCursor ); //Ok, we change the cursor again
	
	combo_Categories->setCurrentItem(0); // Chooses the All category
	this->highlightCategory("All"); // Since the connection is made by name and not index, we have to call it manually
}

void ktvschedule::tvgrabXML(TvItem* tv,QDomElement node_element,QDomNode subnode)
{

if (node_element.tagName()=="programme")	
	 {
		
	 	tv->StartTime = getDateAndTime( node_element.attribute("start","") ); // Add Start time of show 
		//qDebug("after"+tv->StartTime.toString("yyyyMMddhhmm"));
		tv->StopTime = getDateAndTime( node_element.attribute("stop","") ); // Add Stop time of show
		
		//qDebug("before"+tv->StartTime.toString("yyyyMMddhhmm"));
		// This permits to see the validity of the XML file the same time we are parsing the XML file
		if (tv->StartTime.date()<FirstInput ) {FirstInput = tv->StartTime.date();}
		if (tv->StartTime.date()>LastInput ) {LastInput = tv->StartTime.date();}
		
		// Only continue if the date of the show is the same as the showing date. This will make it faster
		if ( (tv->StartTime.date() == date_show->date() ) || (tv->StopTime.date() == date_show->date() ) )
		{
		
		tv->ChannelId=node_element.attribute("channel",""); //.latin1(); // Channel Id
		tv->ChannelName=Channels.find(tv->ChannelId).data().ChannelName;		
		
		tv->ShowView=node_element.attribute("showview","").toInt(); // Showview
	 
	 	while (!subnode.isNull())
		{
			QDomElement subnode_element=subnode.toElement();
			
			if (subnode_element.tagName()=="title") // Title of the show
				{
					tv->Title=subnode_element.text(); //.latin1();
				}
			if (subnode_element.tagName()=="sub-title") //SubTitle
				{
					tv->SubTitle=subnode_element.text(); //.latin1();
				}
				
			if (subnode_element.tagName()=="category") // Category
				{
					tv->Lang=subnode_element.attribute("lang",""); //.latin1();
					QString Cat = subnode_element.text();
					
					tv->Category << Cat; //.latin1();
				
					// This enters a new category to the ComboBox. Check for duplicate 
					if (! this->checkExistingCat(Cat.lower() ) ) 
						{
							Categories+=Cat.lower();
						}
				
				}
			if (subnode_element.tagName()=="star-rating") // Rating
				{
					tv->Stars=subnode_element.text().left(1).toInt();
				}
			if (subnode_element.tagName()=="desc") // Description. This exists only if full update has been choosen
				{
					tv->Description=subnode_element.text(); //.latin1();
				}
				
			if (subnode_element.tagName()=="rating") // Rating
				{
					tv->RatingSystem=subnode_element.attribute("system","");
					tv->RatingValue=subnode_element.text();
				}
			
			if (subnode_element.tagName()=="episode-num")
				{
				if ( subnode_element.attribute("system","") == "dd_progid")
					{ tv->EpisodeNum_ddProgid=subnode_element.text(); }
					
				if (subnode_element.attribute("system","") == "onscreen")
					{ tv->EpisodeNum_onscreen=subnode_element.text().toInt(); }
				}
			
			if (subnode_element.tagName()=="credits")
			{
				QDomNode creditnode = subnode.firstChild();
				
				while ( !creditnode.isNull() )
				{
				  QDomElement creditnode_element=creditnode.toElement();
				  	
				 if (creditnode_element.tagName()=="actor")
					{
					tv->Actors << creditnode_element.text();
					}
			
				if (creditnode_element.tagName()=="producer")
					{
					tv->Producers << creditnode_element.text();
					}
				
				if (creditnode_element.tagName()=="guest")
				{
					tv->Guests << creditnode_element.text();
				}
				creditnode = creditnode.nextSibling();
				}
			}

		subnode=subnode.nextSibling();
		}
		
		}		
	 }
}

void ktvschedule::nxtvepgXML(TvItem* tv,QDomElement node_element,QDomNode subnode)
{

if (node_element.tagName()=="timeslot")	
	 {
		
	 	tv->StartTime = QDateTime::fromString(node_element.attribute("start",""),Qt::ISODate); // Add Start time of show
		tv->StopTime = QDateTime::fromString(node_element.attribute("stop",""),Qt::ISODate); // Add Stop time of show
		
		//qDebug("before"+tv->StartTime.toString("yyyyMMddhhmm"));
		// This permits to see the validity of the XML file the same time we are parsing the XML file
		if (tv->StartTime.date()<FirstInput ) {FirstInput = tv->StartTime.date();}
		if (tv->StartTime.date()>LastInput ) {LastInput = tv->StartTime.date();}
		
		// Only continue if the date of the show is the same as the showing date. This will make it faster
		if ( (tv->StartTime.date() == date_show->date() ) || (tv->StopTime.date() == date_show->date() ) )
		{
		
		tv->ChannelId=node_element.attribute("channel",""); //.latin1(); // Channel Id
		tv->ChannelName=Channels.find(tv->ChannelId).data().ChannelName;		
		
		//tv->ShowView=node_element.attribute("showview","").toInt(); // Showview
		
		tv->Live=node_element.attribute("liveness",""); // Live show
	 
	 	while (!subnode.isNull())
		{
			QDomElement subnode_element=subnode.toElement();
			
			if (subnode_element.tagName()=="programme")
			{

			QDomNode programnode = subnode.firstChild();
				
			while ( !programnode.isNull() )
			{
			
			QDomElement programnode_element=programnode.toElement();
			
			if (programnode_element.tagName()=="title") // Title of the show
				{
					tv->Title=programnode_element.text(); //.latin1();
				}
				
			if (programnode_element.tagName()=="category") // Category
				{
					QString Cat = programnode_element.text();
					// There are many Categories in each node. We are going to break them up
					if ( Cat!="" )
					{
						for (int i= 0; i!=Cat.contains("/")+1;i++)
						{
							tv->Category << Cat.section("/",i,i);
							// This enters a new category to the ComboBox. Check for duplicate 
							if (! this->checkExistingCat(Cat.section("/",i,i).lower() ) ) 
								{
									Categories+=Cat.section("/",i,i).lower();
								}
						}
					}
				}

			if (programnode_element.tagName()=="desc") // Description. This exists only if full update has been choosen
				{
					tv->Description=programnode_element.text(); //.latin1();
				}
			
			programnode=programnode.nextSibling();
			}
			}

		subnode=subnode.nextSibling();
		}
		
		}		
	 }
}

// Checks for duplicate Categories. If it doesn't exists. it returns FALSE
bool ktvschedule::checkExistingCat(QString cat)
{
	bool exists=FALSE;	

	for ( QStringList::Iterator it = Categories.begin(); it != Categories.end(); ++it )
	{
		if ( cat == *it )
		{ exists = TRUE; }
	}
	
	return exists;
}

// Transforms the date and time read from the XML file. It returns a QDateTime
QDateTime ktvschedule::getDateAndTime(QString s_tvdate)
{
	if (!s_tvdate.isNull() )
	{
	int year=s_tvdate.mid( 0, 4 ).toInt(); 
	int month=s_tvdate.mid(4,2).toInt();
	int day=s_tvdate.mid(6,2).toInt();
	int hour=s_tvdate.mid(8,2).toInt();
	int minute=s_tvdate.mid(10,2).toInt();
	
	QDateTime tv_datetime = QDateTime( QDate(year,month,day),QTime(hour,minute) );
	
	return tv_datetime;
	}
}

// Executes the Channel Manager
void ktvschedule::channelList()
{
	channellist *channels = new channellist(this);
	connect (channels, SIGNAL ( channelsChanged() ), this, SLOT ( doNewSorting() ) );
	channels->setChannelMap(Channels);
	channels->show();
}

// Refreshes the XML Guide
void ktvschedule::RefreshXMLGuide()
{
	 QDate FirstInput=QDate::currentDate();
	 QDate LastInput=QDate::currentDate();
	
	 UpdateGuide.clearArguments();
	 
	 UpdateGuide << Config().m_Executable;
	 UpdateGuide << " --output ";
	 UpdateGuide << Config().m_ScheduleFile;
	 
	 if (Config().m_DaysToGrab!=0)
	 {
	 	UpdateGuide << " --days ";
	 	UpdateGuide << QString::number( Config().m_DaysToGrab );
	 }
	 
	 if (Config().m_BeginIn!=0)
	 {
	 	UpdateGuide << " --offset ";
	 	UpdateGuide << QString::number( Config().m_BeginIn );
	 }
	 
	 if (Config().m_FullDetails)
	 {
	 	UpdateGuide << " --slow";
	 }	 
			
	 UpdateGuide << ">>out.txt";
    	switch( QMessageBox::warning( this, "Warning",
     	   "Do you want to update your Tv Guide ?",
   	     "Continue",
  	      "Quit", 0, 0, 1 ) ) 
	{
    	case 0: 
    		if (UpdateGuide.start(KProcess::NotifyOnExit,KProcess::AllOutput) )
		{
		actionCollection()->action("Now Playing")->setEnabled( FALSE );
		
	connect(&UpdateGuide,SIGNAL(receivedStdout(KProcess*,char*,int)),this,SLOT(slotScanStdout(KProcess*,char*,int)));
	connect(&UpdateGuide, SIGNAL(processExited(KProcess *)),this, SLOT(UpdateFinish() ));
	
		StatusBar->message("Please wait until the guide is updated. This can take very long");
		
		actionCollection()->action("Manage Channels")->setEnabled( FALSE );
		
		QTimer *timer = new QTimer( this, "timer" );
        	connect( timer, SIGNAL(timeout()), led, SLOT(toggle()) );
        	timer->start( 100 ); 
 		
		}
        	break;
    	case 1: 
        	break;
    	}
	 
}  

// Slot for StdOut of the tv_grab_?? . Not used yet
void ktvschedule::slotScanStdout(KProcess* proc, char* buffer, int buflen)
{
	QString out(buffer);
	
	out.truncate(buflen);
	out = out.stripWhiteSpace();
	if(!out.isEmpty())
		{
		qDebug(out);
		}
}

// Slot when the tv_grab_?? finishes.
void ktvschedule::UpdateFinish()
{

	dynamic_cast<QTimer *>(child("timer"))->disconnect() ;
	dynamic_cast<QTimer *>(child("timer"))->stop();
/*	delete child("timer");
	//tt->dumpObjectInfo ();
	
	//if (tt->isA("QTimer"))
	qDebug("test");*/
	actionCollection()->action("Refresh Guide")->setEnabled( TRUE );
	actionCollection()->action("Manage Channels")->setEnabled( TRUE );
	this->LoadFileAndData(); //refresh the guide
	
}

// Enables the Go Current Time button. Slot
void ktvschedule::enableGoHomeButton()
{

actionCollection()->action("Now Playing")->setEnabled( TRUE );

}

void ktvschedule::SetToCurrentTime()
{

	if (QDate::currentDate() != date_show->date())
	{
		 date_show->setDate ( QDate::currentDate() );
	}
	
	t_TVGrid->SetToCurrentTime();
	actionCollection()->action("Now Playing")->setEnabled( FALSE );

}

// Runs the configuration window
void ktvschedule::runConfig()
{
   if (m_prefDialog==0) 
   {
        m_prefDialog=new PrefsDialog(this);          // create dialog on demand
        // connect to the "settingsChanged" signal
        connect(m_prefDialog, SIGNAL( settingsChanged() ), this, SLOT( applyPreferences() )  );
    }
    
    m_prefDialog->updateDialog();                   // update the dialog widgets
    if (m_prefDialog->exec()==QDialog::Accepted) // execute the dialog
    {  
        m_prefDialog->updateConfiguration();        // store settings in config object
        applyPreferences();                         // let settings take effect
    }

}

// Apply Preferences if the have changed
void ktvschedule::applyPreferences()
{
	Config().write(); // Writes configuration file
	doNewSorting(); // Reparses the xml file. Many things may have changed
	t_TVGrid->UpdateTable(); // Updates the Tv Grid because colors may have changed
	t_TVGrid->enableTraking(); // Check if we still track the time
	
	if (Config().m_GrabberToUse == "nxtvepg")
	{
		actionCollection()->action("Refresh Guide")->setEnabled(FALSE);
		actionCollection()->action("Manage Channels")->setEnabled(FALSE);
	}
	if (Config().m_GrabberToUse == "tvgrab")
	{
		actionCollection()->action("Refresh Guide")->setEnabled(TRUE);
		actionCollection()->action("Manage Channels")->setEnabled(TRUE);
	}
}

void ktvschedule::notificationsEvent()
{

	(void) KNotifyDialog::configure( );

}

void ktvschedule::findNextCategory()
{

		t_TVGrid->findNextCategory(combo_Categories->currentText () );

}

void ktvschedule::highlightCategory(const QString& cat)
{
	if (cat=="All")
	{
		actionCollection()->action("findnextshow")->setEnabled( FALSE );
	}
	else
	{
		actionCollection()->action("findnextshow")->setEnabled( TRUE );
	}
	
	t_TVGrid->highlightCategory(cat);
	

}

void ktvschedule::sortTvTable()
{
  // Only sort if we use tvgrabber at least in this version
  if (Config().m_GrabberToUse == "tvgrab")
  {
  qDebug(Config().m_GrabberToUse);

  if (t_TVGrid->numCols() != 0 )
  {

    QStringList orderList;
    Q_ULONG maxlenght=80;
    QFile channelFile(Config().m_SettingsFile);
    bool okForSorting = FALSE;
    
    if (channelFile.open(IO_ReadOnly) )
    {
    	
	QString currentChannel;
	
    	while ( !channelFile.atEnd() )
	{
		channelFile.readLine(currentChannel, maxlenght);
		
		if ( currentChannel.contains("channel") !=0 ) //some files have some extra info
		{
			// Norvegian config file does not have channel names. We should abort sorting. This is a very ugly way of doing it
			if ( currentChannel.section(" ",2,2) == QString::null )
			{
				okForSorting = FALSE;
				break;	
			}
			
			
			if (!currentChannel.startsWith( "#" ) )
			{
				QRegExp sep("[\\s ;]");
				if (!currentChannel.section(sep,2,2).startsWith("#")) //German xmltv has in 1st position the channel namespace
				{
					orderList+=currentChannel.section(sep, 2, 2);
					okForSorting = TRUE;
				}
				
				if (currentChannel.section(sep,2,2).startsWith("#")) //German xmltv has in 1st position the channel namespace
				{
					// but we have to use this in order to make it works
					
					int pos = currentChannel.find("#",0) + 1; 
					qDebug(currentChannel.stripWhiteSpace().mid( pos,currentChannel.length()-3 ) );
					
					qDebug("%d",currentChannel.length() );
					orderList+=currentChannel.stripWhiteSpace().mid( pos,currentChannel.length()-2 );
					//orderList+=currentChannel.section(sep, 1, 1).replace("%20"," ");
					okForSorting = TRUE;
				}
				
			}
		}
	}
    }

    channelFile.close();
    
    if (okForSorting)
    {
    	int flag = 0;
	int i = 0;	
	QStringList::Iterator it = orderList.begin();
	
        while( flag != t_TVGrid->numCols ()-1 )
	{
		bool found=FALSE;
		for (int i=flag; i !=t_TVGrid->numCols (); i++)
		{		
			//if (t_TVGrid->horizontalHeader()->label(i) == *it)
			
			QString stripedChannelName = t_TVGrid->horizontalHeader()->label(i);
			QString ChannelNameToCompare;
			bool ok;
			stripedChannelName.section (" ",0,0).toInt(&ok);
			
			if (ok) // Some channels have a number before them. But in the way we read the conf file, they do not appear anymore. One has to strip them.
			{
				ChannelNameToCompare=stripedChannelName.section (" ",1,1);
			}
			else
			{
				ChannelNameToCompare=stripedChannelName;
			}
			
			//if (t_TVGrid->horizontalHeader()->label(i) == *it)
			if (ChannelNameToCompare == *it)
			{
				t_TVGrid->swapColumns ( i,flag, TRUE);
				flag++;
				it++;
				found=TRUE;
				break;
			}
		}
	 if (!found) flag++;
        }
    }
 }
 }
}

void ktvschedule::doNewSorting()
{
	
	this->LoadFileAndData();
	//this->PopulateTvGrid();
	//this->sortTvTable();
}

void ktvschedule::setupSystemTray()
{
  // if(m_toggleSystemTrayAction && m_toggleSystemTrayAction->isChecked()) {
      m_systemTray = new SystemTray(this, "systemTray");
      m_systemTray->show();
     // m_toggleDockOnCloseAction->setEnabled(true);
     connect(m_systemTray, SIGNAL(quitSelected()), this, SLOT(slotQuitFromSystray() ) );
     connect (t_TVGrid, SIGNAL (currentPlayingForSysTray(QStringList )), m_systemTray, SLOT (addPopUpMenu(QStringList ) ));
  /* }
   else {
      m_systemTray = 0;
      m_toggleDockOnCloseAction->setEnabled(false);
   }*/
}

void ktvschedule::slotQuitFromSystray()
{
	// Check if tv_grab is running. If yes, kill it
	if ( UpdateGuide.isRunning () )
	{
		UpdateGuide.kill();
	}
	kapp->quit();
}

void ktvschedule::closeEvent(QCloseEvent *event)
{
      hide();
}

#include "ktvschedule.moc"
/*$SPECIALIZATION$*/




