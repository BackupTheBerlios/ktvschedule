//
// C++ Interface: xmlparser
//
// Description: 
//
//
// Author: Alexios Beveratos <alexios.beveratos@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <qobject.h>
#include <qdom.h>

#include <tvguidetable.h>


/**
@author Alexios Beveratos
*/
class XMLParser : public QObject
{
Q_OBJECT
public:
    XMLParser(QObject *parent = 0, const char *name = 0);

    ~XMLParser();
    void setXMLFile(QString);
    void setTvTable(TvGuideTable*);
    void parseXMLFile();
    QStringList returnCategories();
    
private:
	QFile *XMLFile;
	QDomDocument *TvXML;
	TvGuideTable *TvTable;
	TvItem *Tv;
	QStringList Categories;
	QDate FirstInput;
	QDate LastInput;
	
	void loadXMLFile();
	void xmlExtractor(QDomElement*, TvItem*);
	void recurentXMLParse(QDomElement *,TvItem*);
	void getDateAndTime(QString);
	bool checkExistingCat(QString cat);

signals:
	emitTvItem(TvItem*);
};

#endif
