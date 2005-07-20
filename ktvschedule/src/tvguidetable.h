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
#ifndef TVGUIDETABLE_H
#define TVGUIDETABLE_H

#define val_Title 1
#define val_SubTitle 2
#define val_Rating 4
#define val_Description 8
#define val_TotalPlayTime 16
#define val_Language 32
#define val_Scheduler 64
#define val_Credits 128
#define val_Actors 256
#define val_Guests 512
#define val_Producers 1024
#define val_Categories 2048
#define val_EpisodeNumber 4096
#define val_ShowView 8192
#define val_PRRatingsystem 16384

#include <qobject.h>
#include <qtable.h>
#include <qmap.h>
#include <qdatetime.h>
#include "configuration.h"
#include <qtimer.h>
#include <qpainter.h>
#include <qfont.h>

#include <qsimplerichtext.h>
#include <qmenudata.h>

/**
@author Alexios Beveratos
*/
class TVProgram;
class TvItem;
class QString;

struct ChannelNameIcon
	{ 
	QString ChannelName;
	QString ChannelIcon;
	};

class TvGuideTable : public QTable
{
Q_OBJECT

typedef QMap<QString,ChannelNameIcon> Channelmap;

public:
    TvGuideTable(QWidget *parent = 0, const char *name = 0);

    ~TvGuideTable();
    
    void CreateColumns(QMap<QString,ChannelNameIcon> );
    void setCell ( TvItem*, QMap<QString,ChannelNameIcon>, QDate );
    void clearCells();
    void removeAllColumns();
    void enableTraking();
    void findNextCategory(QString );
      
public slots:

    void SetToCurrentTime();
    void UpdateTable();
    void highlightCategory(const QString& );
    void ImdbDetails(int);
    
    
signals:
	void infoContext(TvItem*);
	void currentPlayingForSysTray( QStringList );
	void recordProgramSignal(TvItem*);
    
protected:
	void CreateRows();
	QTimer* updateTimer;
	TvItem* currentTvItem;
	int lastFoundRow;
	int lastFoundCol;
	
protected slots:
	void showDetails(int, int);
	void scheduleReminder();
	void recordProgram();
	
};

 class TvItem : public QTableItem
{

enum Status
	{
		Past,
		Current,
		Futur
	};

public:

	TvItem(QTable *,EditType);
	
	~TvItem();
	
	void setColor();
	void setCurrentStatus();
	QString getStatus();

 	QDateTime StartTime;
	QDateTime StopTime;
	int ShowView;
	QString ChannelId;
	QString ChannelName;
	QString Title;
	QString SubTitle;
	QString Live;
	QString Lang;
	QStringList Category;
	QString Description;
	QString RatingSystem;
	QString RatingValue;
	QString EpisodeNum_ddProgid;
	QStringList Actors;
	QStringList Producers;
	QStringList Guests;
	QString Classification_age;
	QString Stereo;
	//QDate DateofShow;
	int EpisodeNum_onscreen;
	int Stars;
	
	 QColor m_crBkg;
 	 QColor m_crTxt;
 	 Qt::AlignmentFlags m_HorzAlign;
 	 Qt::AlignmentFlags m_VertAlign;

private:
	
	Status currentStatus;
	
	void paint(QPainter* ,const QColorGroup& ,const QRect& ,bool );
 
 
};

class RichCustomMenuItem : public QCustomMenuItem
{
public:
	RichCustomMenuItem ( QString richtext ): QCustomMenuItem()
	{
	srt = new QSimpleRichText( richtext, QFont() ); // default app font
	srt->adjustSize();
	}
	
	~RichCustomMenuItem () { delete srt; }
	
	bool fullSpan () { return TRUE; }
	
	QSize sizeHint() { return QSize( srt->widthUsed(), srt->height() ); }
	
	virtual void paint(QPainter* p,const QColorGroup& cg, bool, bool,  int x, int y, int w, int h )
	{
		srt->draw( p, x, y, QRect( x, y, w, h ), cg );
	}
	
private:
	QSimpleRichText* srt;
};

#endif
