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
#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

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

#include <kdialogbase.h>

/**
@author Alexios Beveratos
*/
class GeneralPrefsLayout;
class StylePrefsLayout;
class SchedulePrefsLayout;
class DetailsPrefsLayout;

class PrefsDialog : public KDialogBase
{
Q_OBJECT
public:
    PrefsDialog(QWidget *parent = 0, const char *name = 0, WFlags f=0);

    ~PrefsDialog();
    
    /// Transfers the settings from the configuration object to the dialog.
    void updateDialog();
    
    /// Transfers the settings from the dialog to the configuration object.
    void updateConfiguration();

  public slots:
    /// Will be called when the "Default" button has been clicked.
    void slotDefault();
    
    /// Will be called when the "Apply" button has been clicked.
    void slotApply();
    
    /// Will be called whenever a setting was changed.
    void enableApply();

   /// will be called whenever the path is changed
   void enableRefresh();
    
    /// Will fill the m_GrabberToUse accordingling to the selected value
    void slotGrabberToUse(int);

    /// fills the list of availlable grabbers, called by the "list grabber" button
    void fillGrabbersList();
	
    /// applies some choices to the gui depending on the grabber selected
    void applyGrabberConfig();

    /// called when the grabber selection is changed
    void ChangeSelectedGrabber();

    /// launches a config console or xterm for configuration, called by the "config" button
    void launchConfigKonsole();

  signals:
    /// Will be emitted when the new settings should be applied.
    void settingsChanged();

private:
	//The four layouts:
      	GeneralPrefsLayout *m_GeneralPrefs;
	StylePrefsLayout *m_StylePrefs;
	SchedulePrefsLayout *m_SchedulePrefs;
	DetailsPrefsLayout *m_DetailsPrefs;
	
	//The path to the directory containing the grabbers (for expl /usr/bin)
	QString ExecutableDirPath;
};

#endif
