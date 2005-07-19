# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktvschedule.pot

KDE_ICON = ktvschedule

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ktvschedule

# the application source, library search path, and link libraries
ktvschedule_SOURCES = main.cpp ktvschedule.cpp tvguidetable.cpp channellist.cpp configuration.cpp generalprefslayout.ui prefsdialog.cpp styleprefslayout.ui scheduleprefslayout.ui tvscheduler.cpp systemtray.cpp detailspreflayout.ui channelselect_ui.ui
ktvschedule_LDFLAGS = $(KDE_RPATH) $(all_libraries)
ktvschedule_LDADD = $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = ktvschedule.desktop
xdg_apps_DATA = ktvschedule.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/ktvschedule
shellrc_DATA = ktvscheduleui.rc eventsrc


noinst_HEADERS = ktvschedule.h tvguidetable.h channellist.h configuration.h prefsdialog.h tvscheduler.h systemtray.h
kde_icon_KDEICON = hi22-app-ktvschedule.png hi16-app-ktvschedule-live.png hi16-app-ktvschedule-live.png hi16-app-ktvschedule-live.png hi16-app-ktvschedule-live.png hi32-app-ktvschedule-live.png hi32-app-ktvschedule-live.png
SUBDIRS =  pics
