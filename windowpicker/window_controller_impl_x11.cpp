/*
    Application for picking right windows fast and easy way

    Copyright (C) 2010  Dmitry Teslenko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "windowpicker/window_controller_impl.h"

#include <QMessageBox>
#include <QX11Info>

#include <X11/Xlib.h>

namespace WindowPicker {

struct WindowInfo {
	int window;
	QString caption;
};

typedef QList<WindowInfo> WindowInfoList;
WindowInfoList WindowControllerImpl_taskInfoList;

int WindowInfoList_findByWindow(const WindowInfoList &list, int window) {
	for(int i = 0; i < list.size(); ++i) {
		if(list[i].window == window) {
			return i;
		}
	}
	return -1;
}

void WindowControllerImpl::updateWindowInfoList() {
	WindowControllerImpl_taskInfoList.clear();
	Display *dpy = XOpenDisplay(0);
	Q_ASSERT(dpy);

	int screen = DefaultScreen(dpy);
	Window xroot = RootWindow(dpy, screen);
	
	Window root, parent, *pChildren;
	size_t childCount;
	Status status = XQueryTree(dpy, xroot, &root, &parent, &pChildren, &childCount);
	if(status == BadWindow) {
		return;
	}

	char *pCaption;
	for(size_t i = 0; i < childCount; ++i) {
		Window w = pChildren[i];
		XWindowAttributes attrs;
		XGetWindowAttributes(dpy, w, &attrs); 
		if(attrs.map_state != IsViewable) {
			continue;
		}

		QString caption;
		XFetchName(dpy, w, &pCaption);
		caption = QString(pCaption);
		XFree(pCaption);

		if(caption.isEmpty()) {
			continue;
		}

		WindowInfo ti;
		ti.window = w;
		ti.caption = caption;
		WindowControllerImpl_taskInfoList.push_back(ti);
	}

	XFree(pChildren);

	XCloseDisplay(dpy);
}

WindowHandleList WindowControllerImpl::windowHandleList() const {
	WindowHandleList list;
	for(int i = 0; i < WindowControllerImpl_taskInfoList.size(); ++i) {
		list.push_back(WindowControllerImpl_taskInfoList[i].window);
	}
	return list;
}

QString WindowControllerImpl::windowCaption(int handle) const {
	int i = WindowInfoList_findByWindow(WindowControllerImpl_taskInfoList, handle);
	if(i != -1) {
		return WindowControllerImpl_taskInfoList.at(i).caption;
	}
	return QString::null;
}

QString WindowControllerImpl::windowDescription(int handle) const {
	int i = WindowInfoList_findByWindow(WindowControllerImpl_taskInfoList, handle);
	if(i != -1) {
		return WindowControllerImpl_taskInfoList.at(i).caption;
	}
	return QString::null;
}

QPixmap WindowControllerImpl::windowPixmap(int handle) const {
	//return QPixmap::grabWindow(handle);
    Window root;
    int y, x;
    uint w, h, border, depth;
    XGrabServer( QX11Info::display() );
    Window child = handle/*windowUnderCursor( includeDecorations )*/;
    XGetGeometry(QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth);
    Window parent;
    Window* children;
    unsigned int nchildren;
    if( XQueryTree( QX11Info::display(), child, &root, &parent,
                    &children, &nchildren ) != 0 ) {
	if( children != NULL )
	    XFree( children );
	int newx, newy;
	Window dummy;
	if( XTranslateCoordinates( QX11Info::display(), parent, QX11Info::appRootWindow(),
	    x, y, &newx, &newy, &dummy )) {
	    x = newx;
	    y = newy;
	}
    }
	QPixmap pm = QPixmap::grabWindow( QX11Info::appRootWindow(), x, y, w, h );
    XUngrabServer(QX11Info::display());
    return pm;
}


QPixmap WindowControllerImpl::windowIcon(int /*handle*/) const {
	QPixmap pixmap;
	return pixmap;
}

void WindowControllerImpl::selectWindow(int handle) {
	Display *dpy = XOpenDisplay(0);
	Q_ASSERT(dpy);

	//XRaiseWindow(dpy, ti.window);
	//XMapRaised(dpy, ti.window);

	XSetInputFocus(dpy, handle, RevertToPointerRoot, CurrentTime);
	XRaiseWindow(dpy, handle);

	XCloseDisplay(dpy);
}

bool WindowControllerImpl::isMinimized(int /*handle*/) const {
	return false;
}

void WindowControllerImpl::cascadeWindows(const WindowHandleList&) {
}

void WindowControllerImpl::tileWindows(const WindowHandleList&, WindowController::ETile) {
}

void WindowControllerImpl::minimizeWindow(int) {
}

void WindowControllerImpl::maximizeWindow(int) {
}

void WindowControllerImpl::closeWindow(int) {
}

QPixmap WindowControllerImpl::desktopPixmap() const {
	QPixmap pixmap;
	return pixmap;
}



}
