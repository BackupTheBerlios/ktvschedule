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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <qstring.h>
//#include <qfont.h>
#include <qcolor.h>

/// This class contains information about a specific grabber
/// it is mainly used to get such information from the config 
class GrabberConfig{
public:
	GrabberConfig();
	~GrabberConfig();
	bool config_file;
	bool slow;
	bool days;
	bool offset;
	bool days_linked_with_slow;
	bool offset_linked_with_slow;
};

/// This is the one and only configuration object.
/// The member functions read() and write() can be used to load and save
/// the properties to the application configuration file.
class Configuration {
  public:
    /// Reads the configuration data from the application config file.
    /// If a property does not already exist in the config file it will be
    /// set to a default value.
    void read();
    /// Writes the configuration data to the application config file.
    void write() ;//const;

    /// Returns the list of the grabbers recognized by this version of the software
    QString * getListOfGrabbers(int *);

    /// Returns the configuration of a grabber called by its name, or null if no such grabber
    GrabberConfig * getGrabberConfig(QString);
    
    /// String with the URL where the pic can be found
    QString picURL;

    // read general options
    QString m_SettingsFile;     ///< The working directory for tv_grab
    QString m_ScheduleFile;	///< The channel files
    QString m_Executable;	///< Executable of the tv program
    bool m_FirstRun;		///< The first run variable
    
    bool m_setToCurrentTime;	///< Show current programms
    
    bool m_FullDetails;	///< Get all details
    
    int m_DaysToGrab;
    int m_BeginIn;
    QString m_GrabberToUse;
    
    // read style options
    QColor  m_ColorOfActive;     ///< The colour used for the Current Program.
    QColor  m_ColorOfPast;       ///< The colour used for the Past Program
    QColor  m_ColorOfFuture;     ///< The colour used for the Future Program
    QColor  m_ColorText;	///< The colour used for displaying text
    QColor  m_ColorSelected;    ///< The colour used for selected category text
    int     m_RowSize;          ///< The size of the rows in the TvTable
    
    // read Schedule options
    int     m_ScheduleType;
    QColor  m_fontColor;
    QColor  m_backColor;
    int     m_AlarmMins;
    bool    m_Cancel;
    
    // Details. We are going to use a int to store everything
    int     m_DetailsInt;
    
// Defaults
    static const QColor m_defaultColorOfActive;
    static const QColor m_defaultColorOfPast;
    static const QColor m_defaultColorOfFuture;
    static const QColor m_defaultColorText;
    static const QColor m_defaultColorSelected;
    
    static const int m_defaultBeginIn;
    static const int m_defaultDaysToGrab;
    static const int m_defaultRowSize;
    
    static const int m_defaultScheduleType;
    static const QColor m_defaultFontColor;
    static const QColor m_defaultBackColor;
    static const int m_defaultAlarmMins;
    static const QString m_defaultCancel;
    
  private:
    Configuration();
    Configuration(const Configuration&);
    bool returnBool(QString);
    QString returnString(bool);
    
  
  friend Configuration& Config();
     
};

/// Returns a reference to the application configuration object.
Configuration& Config();

#endif  // CONFIGURATION_H
