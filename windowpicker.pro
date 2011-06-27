TEMPLATE = app
INCLUDEPATH += .

CONFIG += console

include(version.pri)

DESTDIR += install

{
	QT *= network
	INCLUDEPATH += vendor/qtsingleapplication
	HEADERS += 	vendor/qtsingleapplication/qtsingleapplication.h \
				vendor/qtsingleapplication/qtlocalpeer.h  \
				vendor/qtsingleapplication/qtlockedfile.h 
		
	SOURCES += 	vendor/qtsingleapplication/qtsingleapplication.cpp \
				vendor/qtsingleapplication/qtlocalpeer.cpp \
				vendor/qtsingleapplication/qtlockedfile.cpp 
}


# Input
HEADERS += \
	windowpicker/about_dialog.h \
	windowpicker/actuator.h \
	windowpicker/config.h \
	windowpicker/config_dialog.h \
	windowpicker/i18n_controller.h \
	windowpicker/keysequence_edit.h \
	windowpicker/mouse_controller.h \
	windowpicker/mouse_controller_impl.h \
	windowpicker/shortcut_controller.h \
	windowpicker/shortcut_controller_impl.h \
	windowpicker/window_controller.h \
	windowpicker/window_controller_impl.h \
	windowpicker/window_list_dialog.h \
	windowpicker/window_list_model.h \
	windowpicker/window_list_view.h \
	windowpicker/tray_icon.h \
	windowpicker/unique_runnable.h \
	windowpicker/unique_runnable_inl.h

SOURCES += \
	windowpicker/about_dialog.cpp \
	windowpicker/actuator.cpp \
	windowpicker/config.cpp \
	windowpicker/config_dialog.cpp \
	windowpicker/i18n_controller.cpp \
	windowpicker/keysequence_edit.cpp \
	windowpicker/mouse_controller.cpp \
	windowpicker/shortcut_controller.cpp \
	windowpicker/window_controller.cpp \
	windowpicker/window_list_dialog.cpp \
	windowpicker/window_list_model.cpp \
	windowpicker/window_list_view.cpp \
	windowpicker/tray_icon.cpp \
	windowpicker/unique_runnable.cpp

win32 {
SOURCES += \
	windowpicker/window_controller_impl_win32.cpp \
	windowpicker/mouse_controller_impl_win32.cpp  \
	windowpicker/shortcut_controller_impl_win32.cpp 

	#win32-msvc2005 {
		LIBS += -luser32 -lgdi32
	#}
}

unix {
SOURCES += \
	windowpicker/window_controller_impl_x11.cpp \
	windowpicker/mouse_controller_impl_x11.cpp \
	windowpicker/shortcut_controller_impl_x11.cpp 
}

SOURCES += main.cpp

RESOURCES += windowpicker.qrc
TRANSLATIONS +=  \
	i18n/windowpicker_en.ts \
	i18n/windowpicker_ru.ts
win32 {
	RC_FILE = windowpicker.rc
}

#setup
win32 {
	#nsis.target = windowpicker.exe
	nsis.commands = makensis /DWP=$${VERSION} windowpicker.nsi 
	nsis.depends = windowpicker.nsi

	QMAKE_EXTRA_TARGETS += nsis
}


#tarball.commands = git archive --format zip --output="$${TARGET}-$${APPLICATION_VERSION}.zip" master 
tarball.commands = git archive master | bzip2 > "$${TARGET}-$${APPLICATION_VERSION}.tar.bz2" 

QMAKE_EXTRA_TARGETS += tarball



