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
#include <qlayout.h>        // for QVBoxLayout
#include <qlabel.h>         // for QLabel
#include <qframe.h>         // for QFrame
#include <kcolorbutton.h>   // for KColorButton
#include <kpushbutton.h>    // for KPushButton
#include <klocale.h>        // for i18n()
#include <kiconloader.h>    // for KIconLoader
#include <kglobal.h>        // for KGlobal
#include <klineedit.h>      // for KLineEdit
#include <kmessagebox.h> 
#include <qwidgetstack.h>

#include "configuration.h"
#include "styleprefslayout.h"
#include "generalprefslayout.h"
#include "scheduleprefslayout.h"
#include "detailspreflayout.h"

#include "prefsdialog.h"

#include <qbuttongroup.h>
#include <qradiobutton.h> 

#include <qcheckbox.h>
#include <qspinbox.h>
#include <kurlrequester.h>
#include <klocale.h>

#include <qfileinfo.h>
#include <qcombobox.h>

PrefsDialog::PrefsDialog(QWidget *parent, const char *name, WFlags f)
 : KDialogBase(IconList, i18n("Preferences"), Default|Ok|Apply|Cancel, Ok, parent, name, f)
{
    //////////// General Prefs //////////////
    
    QFrame *frame = addPage(i18n("General"), i18n("General options"),KGlobal::iconLoader()->loadIcon("kdf",KIcon::FirstGroup,0,false) );
	
    QVBoxLayout *frameLayout = new QVBoxLayout( frame, 0, 0 );
    
    m_GeneralPrefs = new GeneralPrefsLayout(frame);
    m_GeneralPrefs->m_DaysToGrab->setEnabled( true);
        
    frameLayout->addWidget(m_GeneralPrefs);
    
    // Set the grabber preference according to the Config() value
    int id=0;
    if (Config().m_GrabberToUse == "tvgrab")
    	{
    		id = 0;
	}
    if (Config().m_GrabberToUse == "nxtvepg")
       {
       		id = 1;
       }
    dynamic_cast<QRadioButton*>( m_GeneralPrefs->b_buttonGroup->find(id) )->setChecked( TRUE );
           
    ///////////Style Prefs ////////////////
 
    frame = addPage(i18n("Style"), i18n("Style settings"),KGlobal::iconLoader()->loadIcon("colors",KIcon::FirstGroup,0,false) );
	
    frameLayout = new QVBoxLayout( frame, 0, 0 );
    
    m_StylePrefs = new StylePrefsLayout(frame);
    
    frameLayout->addWidget(m_StylePrefs);
    
    /////// Notification prefsdialog ////////////////
    
    frame = addPage(i18n("Schedule"), i18n("Schedule settings"),KGlobal::iconLoader()->loadIcon("bell",KIcon::FirstGroup,0,false) );
    
    frameLayout = new QVBoxLayout (frame, 0, 0);
    
    m_SchedulePrefs = new SchedulePrefsLayout ( frame );
    
    dynamic_cast<QRadioButton*>(m_SchedulePrefs->bg_schedule->find (Config().m_ScheduleType ) )->setChecked( TRUE) ;
    m_SchedulePrefs->ScheduleWidgetStack->raiseWidget( Config().m_ScheduleType ) ;
    
    frameLayout->addWidget(m_SchedulePrefs);
    
    /////////// Detail Prefs ///////////////
    
    frame = addPage(i18n("Details"), i18n("Details Selection"), KGlobal::iconLoader()->loadIcon("enhanced_browsing",KIcon::FirstGroup,0,false) );
    
    frameLayout = new QVBoxLayout (frame, 0, 0);
    
    m_DetailsPrefs = new DetailsPrefsLayout(frame, 0, 0);
        
    frameLayout->addWidget(m_DetailsPrefs);

    //connections for general prefs layout
    connect( m_GeneralPrefs->m_SettingsFile, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
    connect( m_GeneralPrefs->m_ScheduleFile, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
    connect( m_GeneralPrefs->m_setToCurrentTime, SIGNAL( clicked () ), this, SLOT( enableApply() )  );
    connect( m_GeneralPrefs->m_Path, SIGNAL ( textChanged(const QString&) ), this, SLOT (enableApply() ) );
    connect( m_GeneralPrefs->m_Path, SIGNAL ( textChanged(const QString&) ), this, SLOT (enableRefresh() ) );
    connect( m_GeneralPrefs->m_nxtvepgPath, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
    
    connect( m_GeneralPrefs->m_FullDetails, SIGNAL( clicked() ), this, SLOT( enableApply() ) );
    connect( m_GeneralPrefs->m_DaysToGrab, SIGNAL(  valueChanged(int ) ), this, SLOT (enableApply() ) );
    connect( m_GeneralPrefs->m_BeginIn, SIGNAL( valueChanged(int ) ), this, SLOT (enableApply() ) );
   
    connect( m_GeneralPrefs->b_ListGrabbers, SIGNAL( clicked () ), this, SLOT (fillGrabbersList() ) );
    connect( m_GeneralPrefs->choice_Grabber,SIGNAL(activated(int)),this,SLOT(ChangeSelectedGrabber()));
    connect( m_GeneralPrefs->b_Config,SIGNAL(clicked()),this,SLOT(launchConfigKonsole()));
    connect( m_GeneralPrefs->b_buttonGroup, SIGNAL( clicked(int)) , this, SLOT(slotGrabberToUse(int )) );
    
    //connections for style prefs layout
    connect( m_StylePrefs->b_ColorOfActive, SIGNAL ( changed(const QColor&) ), this, SLOT ( enableApply() ) );
    connect( m_StylePrefs->b_ColorOfPast , SIGNAL ( changed(const QColor&) ), this, SLOT ( enableApply() ) );
    connect( m_StylePrefs->b_ColorOfFuture, SIGNAL ( changed(const QColor&) ), this, SLOT ( enableApply() ) );
    
    connect( m_StylePrefs->b_ColorText, SIGNAL ( changed(const QColor&) ), this, SLOT ( enableApply() ) );
    connect( m_StylePrefs->b_ColorSelected, SIGNAL ( changed(const QColor&) ), this, SLOT ( enableApply() ) );
    connect( m_StylePrefs->b_RowSize, SIGNAL ( valueChanged(int) ), this, SLOT ( enableApply() ) );
    
    //connections for schedule prefs layout
    connect (m_SchedulePrefs->bg_schedule, SIGNAL ( pressed (int ) ), this, SLOT ( enableApply() ) );
    connect (m_SchedulePrefs->b_fontColor, SIGNAL (changed(const QColor&) ), this, SLOT (enableApply() ) );
    connect (m_SchedulePrefs->b_backColor, SIGNAL (changed(const QColor&) ), this, SLOT (enableApply() ) );
    connect (m_SchedulePrefs->sb_AlarmMins, SIGNAL (valueChanged(int) ), this, SLOT (enableApply() ) );
    connect (m_SchedulePrefs->b_Cancel, SIGNAL ( clicked() ), this, SLOT (enableApply() ) );

    //connections for details prefs layout
    connect (m_DetailsPrefs->checkTitle, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkSubTitle, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkRating, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkDescription, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkTotalPlayTime, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkLanguage, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkScheduler, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkCredits, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkActors, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkGuests, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
    connect (m_DetailsPrefs->checkProducers, SIGNAL ( clicked() ), this , SLOT (enableApply() ) );
}


PrefsDialog::~PrefsDialog()
{
}

void PrefsDialog::updateDialog() {
    
    m_GeneralPrefs->m_SettingsFile->setURL( Config().m_SettingsFile );
    
    if (Config().m_GrabberToUse == "tvgrab")
    {
    	m_GeneralPrefs->m_ScheduleFile->setURL( Config().m_ScheduleFile );
    }
    if (Config().m_GrabberToUse == "nxtvepg")
    {
    	m_GeneralPrefs->m_nxtvepgPath->setURL( Config().m_ScheduleFile );
    }
    

	//determination of path
    QFileInfo E(Config().m_Executable);
    m_GeneralPrefs->m_Path->setText(E.dirPath(true));

    fillGrabbersList();
    m_GeneralPrefs->b_ListGrabbers->setEnabled(FALSE);
    
    m_GeneralPrefs->m_DaysToGrab->setValue(Config().m_DaysToGrab );
    m_GeneralPrefs->m_BeginIn->setValue(Config().m_BeginIn );
    
    m_GeneralPrefs->m_setToCurrentTime->setChecked( Config().m_setToCurrentTime );
 
    m_GeneralPrefs->m_FullDetails->setChecked( Config().m_FullDetails );
    
    m_StylePrefs->b_ColorOfActive->setColor (Config().m_ColorOfActive );
    m_StylePrefs->b_ColorOfPast->setColor (Config().m_ColorOfPast );
    m_StylePrefs->b_ColorOfFuture->setColor (Config().m_ColorOfFuture );
    
    m_StylePrefs->b_ColorText->setColor (Config().m_ColorText );
    m_StylePrefs->b_ColorSelected->setColor (Config().m_ColorSelected );
    m_StylePrefs->b_RowSize->setValue( Config().m_RowSize );
    
    dynamic_cast<QRadioButton*>(m_SchedulePrefs->bg_schedule->find (Config().m_ScheduleType ) )->setChecked( TRUE) ;
    
    m_SchedulePrefs->b_fontColor->setColor (Config().m_fontColor );
    m_SchedulePrefs->b_backColor->setColor (Config().m_backColor );
    m_SchedulePrefs->sb_AlarmMins->setValue (Config().m_AlarmMins);
  
    m_SchedulePrefs->b_Cancel->setChecked ( Config().m_Cancel );
    
     m_DetailsPrefs->checkTitle->setChecked ( (Config().m_DetailsInt & val_Title) == val_Title );
     m_DetailsPrefs->checkSubTitle->setChecked ( (Config().m_DetailsInt & val_SubTitle) == val_SubTitle );
     m_DetailsPrefs->checkRating->setChecked ( (Config().m_DetailsInt & val_Rating) == val_Rating );
     m_DetailsPrefs->checkDescription->setChecked ( (Config().m_DetailsInt & val_Description) == val_Description );
     m_DetailsPrefs->checkTotalPlayTime->setChecked ( (Config().m_DetailsInt & val_TotalPlayTime) == val_TotalPlayTime );
     m_DetailsPrefs->checkLanguage->setChecked ( (Config().m_DetailsInt & val_Language) == val_Language );
     m_DetailsPrefs->checkScheduler->setChecked ( (Config().m_DetailsInt & val_Scheduler) == val_Scheduler );
     m_DetailsPrefs->checkCredits->setChecked ( (Config().m_DetailsInt & val_Credits) == val_Credits );
     m_DetailsPrefs->checkActors->setChecked ( (Config().m_DetailsInt & val_Actors) == val_Actors );
     m_DetailsPrefs->checkGuests->setChecked ( (Config().m_DetailsInt & val_Guests) == val_Guests );
     m_DetailsPrefs->checkProducers->setChecked ( (Config().m_DetailsInt & val_Producers) == val_Producers );
     m_DetailsPrefs->checkCategories->setChecked ( (Config().m_DetailsInt & val_Categories) == val_Categories );
     m_DetailsPrefs->checkEpisodeNumber->setChecked ( (Config().m_DetailsInt & val_EpisodeNumber) == val_EpisodeNumber );
     m_DetailsPrefs->checkShowView->setChecked ( (Config().m_DetailsInt & val_ShowView) == val_ShowView );
     m_DetailsPrefs->checkPRRatingsystem->setChecked ( (Config().m_DetailsInt & val_PRRatingsystem) == val_PRRatingsystem );    
    
    enableButtonApply(false);   // disable apply button
}


void PrefsDialog::updateConfiguration() {
    Config().m_SettingsFile    = m_GeneralPrefs->m_SettingsFile->url();
    
    if (Config().m_GrabberToUse == "tvgrab")
    {
    	Config().m_ScheduleFile    = m_GeneralPrefs->m_ScheduleFile->url();
    }
    if (Config().m_GrabberToUse == "nxtvepg")
    {
    	Config().m_ScheduleFile    = m_GeneralPrefs->m_nxtvepgPath->url();
    }
    	

    Config().m_Executable      = m_GeneralPrefs->m_Path->text()+"/"+m_GeneralPrefs->choice_Grabber->currentText();

    Config().m_DaysToGrab        = m_GeneralPrefs->m_DaysToGrab->value();
    Config().m_BeginIn           = m_GeneralPrefs->m_BeginIn->value();
        
    // Style
    
    Config().m_ColorOfActive   = m_StylePrefs->b_ColorOfActive->color();
    Config().m_ColorOfFuture   = m_StylePrefs->b_ColorOfFuture->color();
    Config().m_ColorOfPast     = m_StylePrefs->b_ColorOfPast->color();
    
    Config().m_ColorText       = m_StylePrefs->b_ColorText->color();
    Config().m_ColorSelected   = m_StylePrefs->b_ColorSelected->color();
    Config().m_RowSize	       = m_StylePrefs->b_RowSize->value();   
    
    Config().m_setToCurrentTime = m_GeneralPrefs->m_setToCurrentTime->isChecked ();
    
    Config().m_FullDetails 	= m_GeneralPrefs->m_FullDetails->isChecked ();
    
    // Schedules
    
    Config().m_ScheduleType	= m_SchedulePrefs->bg_schedule->selectedId ();
    Config().m_fontColor	= m_SchedulePrefs->b_fontColor->color();
    Config().m_backColor	= m_SchedulePrefs->b_backColor->color();
    Config().m_AlarmMins	= m_SchedulePrefs->sb_AlarmMins->value();
    Config().m_Cancel	        = m_SchedulePrefs->b_Cancel->isChecked ();
    
    
    // Details
     unsigned int DetailsInt= 0;
     
     if (m_DetailsPrefs->checkTitle->isChecked() ) { DetailsInt += val_Title; }
     if (m_DetailsPrefs->checkSubTitle->isChecked() ) { DetailsInt += val_SubTitle; }
     if (m_DetailsPrefs->checkRating->isChecked() ) { DetailsInt += val_Rating ; }
     if (m_DetailsPrefs->checkDescription->isChecked() ) { DetailsInt += val_Description ; }
     if (m_DetailsPrefs->checkTotalPlayTime->isChecked() ) { DetailsInt += val_TotalPlayTime ; }
     if (m_DetailsPrefs->checkLanguage->isChecked() ) { DetailsInt += val_Language ; }
     if (m_DetailsPrefs->checkScheduler->isChecked() ) { DetailsInt += val_Scheduler ; }
     if (m_DetailsPrefs->checkCredits->isChecked() ) { DetailsInt += val_Credits ; }
     if (m_DetailsPrefs->checkActors->isChecked() ) { DetailsInt += val_Actors ; }
     if (m_DetailsPrefs->checkGuests->isChecked() ) { DetailsInt += val_Guests ; }
     if (m_DetailsPrefs->checkProducers->isChecked() ) { DetailsInt += val_Producers ; }
     if (m_DetailsPrefs->checkCategories->isChecked() ) { DetailsInt += val_Categories ; }
     if (m_DetailsPrefs->checkEpisodeNumber->isChecked() ) { DetailsInt += val_EpisodeNumber ; }
     if (m_DetailsPrefs->checkShowView->isChecked() ) { DetailsInt += val_ShowView ; }
     if (m_DetailsPrefs->checkPRRatingsystem->isChecked() ) { DetailsInt += val_PRRatingsystem ; }
     
     
     Config().m_DetailsInt = DetailsInt;    
     
    enableButtonApply(false);   // disable apply button
}


void PrefsDialog::slotDefault() {
    if (KMessageBox::warningContinueCancel(this, i18n("This will set the default options "
        "in ALL pages of the preferences dialog! Continue?"), i18n("Set default options?"),
        i18n("Set defaults"))==KMessageBox::Continue)
    {
        m_StylePrefs->b_ColorOfActive->setColor (Config().m_defaultColorOfActive );
        m_StylePrefs->b_ColorOfPast->setColor (Config().m_defaultColorOfPast );
        m_StylePrefs->b_ColorOfFuture->setColor (Config().m_defaultColorOfFuture );
	
	m_StylePrefs->b_ColorText->setColor (Config().m_defaultColorText );
	m_StylePrefs->b_ColorSelected->setColor (Config().m_defaultColorSelected );
	m_StylePrefs->b_RowSize->setValue( Config().m_defaultRowSize );
	
	m_GeneralPrefs->m_DaysToGrab->setValue (Config().m_defaultDaysToGrab );
	m_GeneralPrefs->m_BeginIn->setValue (Config().m_defaultBeginIn );
	
	//m_SchedulePrefs->bg_schedule->find( Config().m_defaultScheduleType )->toggle ();
	dynamic_cast<QRadioButton*>(m_SchedulePrefs->bg_schedule->find (Config().m_defaultScheduleType ) )->setChecked( TRUE) ;
	
        enableApply();   // enable apply button
    }
}


void PrefsDialog::slotApply() {
    updateConfiguration();      // transfer settings to configuration object
    emit settingsChanged();     // apply the preferences
    enableButtonApply(false);   // disable apply button again
}


void PrefsDialog::enableApply() {
    enableButtonApply(true);   // enable apply button
}

void PrefsDialog::enableRefresh() {
    m_GeneralPrefs->b_ListGrabbers->setEnabled(TRUE);   // enable apply button
}


void PrefsDialog::fillGrabbersList()
{
        ExecutableDirPath=m_GeneralPrefs->m_Path->text();
	int nb_grabbers_found=0;
	
	//gets the list of grabbers recognized by this version of the soft
	QString * List_Grabbers_Found=Config().getListOfGrabbers(&nb_grabbers_found);

	m_GeneralPrefs->choice_Grabber->clear();//clears the list
	int current=0;
	for(int i=0;i<nb_grabbers_found;i++)//for each grabber, tests if it is present in the system
	{
		QFile Fi(ExecutableDirPath+"/"+List_Grabbers_Found[i]);
		if(Fi.exists())
		{
			m_GeneralPrefs->choice_Grabber->insertItem(List_Grabbers_Found[i]);
			if(ExecutableDirPath+"/"+List_Grabbers_Found[i]==Config().m_Executable)
			{
				current=i;//the one present in the KURLrequester is put by default
			}
		}
	}
	m_GeneralPrefs->choice_Grabber->setCurrentItem(current);
	m_GeneralPrefs->b_ListGrabbers->setEnabled(FALSE);
}

void PrefsDialog::applyGrabberConfig()
{
	GrabberConfig * G=Config().getGrabberConfig(m_GeneralPrefs->choice_Grabber->currentText());
	if(G!=NULL)//if a grabber was returned, adapt the buttons to the particular config
	{
		m_GeneralPrefs->m_SettingsFile->setEnabled(G->config_file);
		m_GeneralPrefs->b_Config->setEnabled(G->config_file);
		m_GeneralPrefs->m_FullDetails->setEnabled(G->slow);
		m_GeneralPrefs->m_FullDetails->setChecked(G->slow);
		m_GeneralPrefs->m_DaysToGrab->setEnabled(G->days);
		m_GeneralPrefs->m_DaysToGrab->setValue(0);
		m_GeneralPrefs->m_BeginIn->setEnabled(G->offset);
		m_GeneralPrefs->m_BeginIn->setValue(0);
	}
}

void PrefsDialog::ChangeSelectedGrabber()
{
	//changes the name of the config file depending on the name of the grabber
	QFileInfo F(m_GeneralPrefs->m_SettingsFile->url());
	m_GeneralPrefs->m_SettingsFile->setURL(F.dirPath(true)+"/"+m_GeneralPrefs->choice_Grabber->currentText()+".conf");
//	m_GeneralPrefs->m_SettingsFile->setURL(F.dirPath(true)+"/"+m_GeneralPrefs->choice_Grabber->currentText()+".conf");

	//adapt the buttons depending on the grabber
	applyGrabberConfig();
}

void PrefsDialog::launchConfigKonsole()
{
	QString T(m_GeneralPrefs->m_Path->text()+"/"+m_GeneralPrefs->choice_Grabber->currentText()+" --configure --config-file "+m_GeneralPrefs->m_SettingsFile->url());//create the command to run
	
	if(system("konsole -e "+T+" &")!=0)//if konsole is not present test with xterm
	{
		if(system("xterm -e "+T+" &")!=0)//if not present show an error message
		{
 KMessageBox::error (this, "xterm or konsole were not found on your system, please run the following command in a terminal:\n"+T,"File not found") ;
		}
	}
}

void PrefsDialog::slotGrabberToUse(int grabberId)
{
	if (grabberId == 0)
	{
		Config().m_GrabberToUse = "tvgrab";
	}
	
	if (grabberId == 1)
	{
		Config().m_GrabberToUse = "nxtvepg";
	}
	
	enableApply();
}

#include "prefsdialog.moc"
