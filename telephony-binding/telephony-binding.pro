TARGET = telephony-binding

SOURCES = telephony-binding.c \
	  gdbus/ofono_manager.c \
	  gdbus/ofono_manager_interface.c \
	  gdbus/ofono_voicecall.c \
	  gdbus/ofono_voicecall_interface.c \
	  gdbus/ofono_voicecallmanager.c \
	  gdbus/ofono_voicecallmanager_interface.c

INCLUDEPATH += $$PWD/gdbus
LIBS += -Wl,--version-script=$$PWD/export.map

CONFIG += link_pkgconfig
PKGCONFIG += json-c afb-daemon glib-2.0 gio-2.0 gio-unix-2.0

include(telephony-binding.pri)
