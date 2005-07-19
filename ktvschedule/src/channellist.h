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

#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#include "channelselect_ui.h"
#include "qpixmap.h"
#include <qlistbox.h>
#include <qpainter.h>
#include "tvguidetable.h"

struct ChannelNameIcon;
/*	{ 
	QString ChannelName;
	QString ChannelIcon;
	};*/

class channellist : public ChannelSelect_ui
{
  Q_OBJECT
	typedef QMap<QString,ChannelNameIcon> Channelmap;
  	
public:
  channellist(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~channellist();
  
  void setChannelMap (Channelmap);
  void downloadPixMapstest();
  
 public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          removeChannel();
  virtual void          addChannel();
  virtual void		moveChannelUp();
  virtual void		moveChannelDown();

signals:
	void channelsChanged();
  
protected:
  /*$PROTECTED_FUNCTIONS$*/

  void openFile();
  void init();
  void UpdateConfigFile();
  
  QString home;
  QStringList extraFileInfo;
  Channelmap Channels;
  
protected slots:

	
  /*$PROTECTED_SLOTS$*/

};

class TvStation : public QListBoxItem
{
public:
	TvStation (QListBox*, QString);
	TvStation ( QListBox* , QString, TvStation* );
	~TvStation();
	QString Name;
	QString fullPath;
	QPixmap Pict;
	
private:
	void getName();
	void getPict();
	bool pictExists;

protected:
	virtual void paint( QPainter * );
	virtual int width( const QListBox* ) const { return 100; }
	virtual int height( const QListBox* ) const { return 35; }

	
};

#endif

