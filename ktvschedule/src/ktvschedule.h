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

#ifndef KTVSCHEDULE_H
#define KTVSCHEDULE_H

// Class ktvschedule

#include "tvguidetable.h"
#include "prefsdialog.h"
#include "tvscheduler.h"

#include <qdom.h>

#include <qdatetimeedit.h>
#include <qdatetime.h>

#include <kcombobox.h>
#include <kmainwindow.h>
#include <qgroupbox.h>
#include <kled.h>
#include <kprocess.h>
#include <kstatusbar.h>
#include "systemtray.h"



class ktvschedule : public KMainWindow
{
  Q_OBJECT
		
	typedef QMap<QString,ChannelNameIcon> Channelmap;

  
public:
  //ktvschedule(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ktvschedule();

  ~ktvschedule();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:

  void correspondChannels();
  //QString findChannelNameFromChannelNumber(QString );
  void init();
  void LoadFileAndData();
  void sortTvTable();
  void setupSystemTray();
  void tvgrabXML(TvItem*,QDomElement,QDomNode);
  void nxtvepgXML(TvItem*,QDomElement,QDomNode);
  
  SystemTray  *m_systemTray;
  
  QGroupBox *tvgridbox;
  
  QDateEdit *date_show;
  KStatusBar* StatusBar;
  
  QDateTime getDateAndTime(QString );
   
  QDate FirstInput;
  QDate LastInput;
  
  KComboBox *combo_Categories;
  QStringList Categories;
  
  Channelmap Channels;
  TvGuideTable *t_TVGrid;
  QDomDocument TvXML;
  
  PrefsDialog *m_prefDialog;
  
  KShellProcess UpdateGuide;
  
  KLed *led;
  
  tvscheduler *tvShowScheduler;
  
protected slots:

  bool loadXMLFile(const QString&);
  void channelList();
  void PopulateTvGrid();
  void RefreshXMLGuide();
  void runConfig();
  void applyPreferences();
  void notificationsEvent();
  
  void enableGoHomeButton();
  void SetToCurrentTime();
  
  bool checkExistingCat(QString );
  void slotScanStdout(KProcess*,char*,int);
  void UpdateFinish();
  void findNextCategory();
  //void doNotMiss(TvItem*);
  void highlightCategory(const QString&);
  void doNewSorting();
  
  void slotQuitFromSystray();
  void closeEvent(QCloseEvent *);

};

#endif

