//
// C++ Implementation: xmlparser
//
// Description: 
//
//
// Author: Alexios Beveratos <alexios.beveratos@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent, const char *name)
 : QObject(parent, name)
{
}


XMLParser::~XMLParser()
{
}

XMLParser::setXMLFile(QString fileToSet) ///< Sets the XML file to be parsed
{
	this->XMLFile.setName(fileToSet);
}

XMLParser::setTvTable(TvGuideTable *m_TvTable)
{
	this->TvTable = m_TvTable;
}

void XMLParser::loadXMLFile() ///< Loads the XML file and creates a new QDom document
{

//Opens XML File

 if( !XMLFile->open( IO_ReadOnly ) )
   {
   
   KMessageBox::error (this, "I was not able to open the XML file. Please check your configuration", "File not found") ;
   qDebug("Could not open file");
   return FALSE;
   
   }
   else
   {
	//and sets contents to the TvXML. This is loaded into the memory
	 if( !TvXML->setContent( XMLFile ) )
 	{
		KMessageBox::error (this, "There was an error loading the XML file. It is maybe not critical", "XML Error") ;
   		qDebug ("There was an error loading the XML file");;
 	}
    XMLFile->close();
    return TRUE;
    }
}

void XMLParser::parseXMLFile()
{
	QDomElement docElement = TvXML.documentElement();
	QDomNode node = docElement.firstChild();
	
	while( !node.isNull() )
  	{
 	 TvItem *tv = new TvItem(TvTable,QTableItem::Never); //Create a TvItem which will be inserted to the table
 	 
 	 QDomNode *subnode=node.firstChild();
 	 
	 this->recurentXMLParse(subnode, tv);
	 
	 if (tv->StartTime.date()<FirstInput ) {FirstInput = tv->StartTime.date();}
	 if (tv->StartTime.date()>LastInput ) {LastInput = tv->StartTime.date();}
	 
	if ( (tv->StartTime.date() == date_show->date() ) || (tv->StopTime.date() == date_show->date() ) )
	{
		tv->setCurrentStatus(); // Set the status comparaing it to the current time
		t_TVGrid->setCell(tv, Channels, date_show->date()); //Adds the tv show to the grid.
	}
	 
	 node.nextSibling();
	 
	}
}

void XMLParser::recurentXMLParse(QDomNode *subnode, TvItem tv)
{
	while (!subnode.isNull())
	{
		this->xmlExtractor(subnode.toElement(), tv);
		
		 // stop in the case that the schedule does not fit the current date
		if ( (tv->StartTime.date() == date_show->date() ) || (tv->StopTime.date() == date_show->date() ) )
		{ break };
		
		this->recurentXMLParse(subnode.firstChild(), tv);
		subnode.nextSibling()
	}
}

void XMLParser::xmlExtractor(QDomElement *currentDomElement, TvItem *tv)
{
	switch (currentDomElement->tagName())
	{
	case "programme":
		tv->StartTime = getDateAndTime( currentDomElement->attribute("start","") ); // Add Start time of show 
		tv->StopTime = getDateAndTime( currentDomElement->attribute("stop","") ); // Add Stop time of show
		tv->ChannelId= currentDomElement->attribute("channel","");
		tv->ChannelName=Channels.find(tv->ChannelId).data().ChannelName;
		tv->ShowView=currentDomElement->attribute("showview","").toInt(); // Showview
		break;
	case "title":
		tv->Title=currentDomElement->text();
		break;
	case "sub-title":
		tv->SubTitle=currentDomElement->text(); 
		break;
	case "category":
		if (Config().m_GrabberToUse = "tvgrab")
		{
			tv->Lang=currentDomElement->attribute("lang","");
		
			QString Cat = currentDomElement->text();
			tv->Category << Cat;
			// This enters a new category to the ComboBox. Check for duplicate 
			if (! this->checkExistingCat(Cat.lower() ) ) 
				{
					Categories+=Cat.lower();
				}
		}
		
		if (Config().m_GrabberToUse == "nxtvepg") 
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
		break;
	case "star-rating":
		tv->Stars=currentDomElement->text().left(1).toInt();
		break;
	case "desc":
		tv->Description=currentDomElement->text();
		break;
	case "rating":
		tv->RatingSystem=currentDomElement->attribute("system","");
		tv->RatingValue=currentDomElement->text();
		break;
	case "episode-num":
		if ( currentDomElement->attribute("system","") == "dd_progid")
			{ tv->EpisodeNum_ddProgid=currentDomElement->text(); }
		if ( currentDomElement->attribute("system","") == "onscreen")
			{ tv->EpisodeNum_onscreen=currentDomElement->text().toInt(); }
		break;
	case "actor":
		tv->Actors << currentDomElement->text();
		break;
	case "producer":
		tv->Producers << currentDomElement->text();
		break;
	case "guest":
		tv->Guests << currentDomElement->text();
		break;
	case "timeslot":		
	 	tv->StartTime = QDateTime::fromString(currentDomElement->attribute("start",""),Qt::ISODate); // Add Start time of show
		tv->StopTime = QDateTime::fromString(currentDomElement->attribute("stop",""),Qt::ISODate); // Add Stop time of show
		tv->ChannelId=currentDomElement->attribute("channel",""); //.latin1(); // Channel Id
		tv->ChannelName=Channels.find(tv->ChannelId).data().ChannelName;
		tv->Live=node_element.attribute("liveness","");
		break;
	case "classification":
		if (currentDomElement->attribute("system","") == "age")
		{ tv->Classification_age = currentDomElement->text(); }
		break;
	case "stereo":
		tv->Stereo = currentDomElement->text();
		break;
	
	}

}

bool XMLParser::checkExistingCat(QString cat)
{
	bool exists=FALSE;	

	for ( QStringList::Iterator it = Categories.begin(); it != Categories.end(); ++it )
	{
		if ( cat == *it )
		{ exists = TRUE; }
	}
	
	return exists;
}

QStringList XMLParser::returnCategories()
{
	Categories.sort();
	return Categories;
}

QDateTime XMLParser::getDateAndTime(QString s_tvdate)
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

#include "xmlparser.moc"
