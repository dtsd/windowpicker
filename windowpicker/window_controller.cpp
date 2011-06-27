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
#include "windowpicker/window_controller.h"

#include <QString>
#include <QList>
#include <QSet>
#include <QPixmapCache>
#include <QTime>
#include <QMap>
#include <QPainter>
#include <QTimer>

#include <QMutex>
#include <QReadWriteLock>
#include <QThreadPool>
#include <QDataStream>

#include "windowpicker/window_controller_impl.h"
#include "windowpicker/config.h"
#include "windowpicker/unique_runnable.h"
#include "windowpicker/unique_runnable_inl.h"

QDataStream & operator<<(QDataStream & out, const WindowPicker::WindowInfo & info)
{
	out << info.handle ;
	out << info.caption;
	out << info.className;
	return out;
};

QDataStream & operator>>(QDataStream & in, WindowPicker::WindowInfo & info)
{
	in >> info.handle;
	in >> info.caption;
	in >> info.className;
	return in;
};


namespace WindowPicker {

typedef QMap<int, QPixmap> PixmapCache;
typedef QMap<int, QString> StringCache;
typedef QMap<int, Hotkey> HotkeyMap;

Hotkey getNextHotkey(const Hotkey &hotkey) {
	const int c_first = Qt::Key_A;
	const int c_last = Qt::Key_Z;

	Hotkey result = hotkey;
	int carry = 1, index = result.size() - 1;
	
	while(carry) {
		if(index < 0) {
			result.push_front(c_first - 1);
			index = 0;
		}

		result[index] += carry;
		carry = 0;
		if(result[index] > c_last) {
			result[index] = c_first;
			++carry;
		}

		--index;
	}	

	return result;
};

QString hotkeyToString(const Hotkey &hotkey) {
	QString s;
	for(Hotkey::const_iterator it = hotkey.begin(); it != hotkey.end(); ++it) {
		s.append(QChar(*it));
	}
	return s;
}

class WindowPreview : public UniqueRunnable<WindowPreview> {
public:
	WindowPreview(int handle)  :UniqueRunnable<WindowPreview>(handle) {};
	virtual void uniqueRun(int handle) {
		QPixmap pixmap;
		if(!WindowController::instance()->impl->isMinimized(handle)) {
			pixmap = WindowController::instance()->impl->windowPixmap(handle);
			QSize reducedSize = pixmap.size() * 0.2;
			pixmap = pixmap.scaled(reducedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		WindowController::instance()->setWindowPreview(handle, pixmap);
	};
private:
	static QMutex s_lock;
	static HandleSet s_handleSet;
	friend class UniqueRunnable<WindowPreview>;
};

HandleSet WindowPreview::s_handleSet;
QMutex WindowPreview::s_lock;

class WindowSelect : public UniqueRunnable<WindowSelect> {
public:
	WindowSelect(int handle)  :UniqueRunnable<WindowSelect>(handle) {};
	virtual void uniqueRun(int handle) {
		WindowController::instance()->impl->selectWindow(handle);
	};
private:
	static QMutex s_lock;
	static HandleSet s_handleSet;
	friend class UniqueRunnable<WindowSelect>;
};

HandleSet WindowSelect::s_handleSet;
QMutex WindowSelect::s_lock;

struct WindowControllerPrivate {
	WindowControllerPrivate() :isHotkeyLabelsShown(false) {};

	PixmapCache previewCache, iconCache;
	StringCache captionCache;
	QReadWriteLock previewCacheLock;
	QPixmap defaultPreview;
	QThreadPool previewThreads, selectThreads;
	WindowHandleList windowHandleList/*, windowHandleListBackup*/;
	HotkeyMap hotkeyMap;
	bool isHotkeyLabelsShown;
};

WindowController::WindowController() : impl(new WindowControllerImpl), p(new WindowControllerPrivate) {
	p->defaultPreview = QPixmap(":default-preview");

	updateWindowInfoList();
	startUpdateWindowPreviews();

	qRegisterMetaType<WindowPicker::WindowInfo>("WindowPicker::WindowInfo");
	qRegisterMetaTypeStreamOperators<WindowPicker::WindowInfo>("WindowPicker::WindowInfo");
}

WindowController::~WindowController() {
	delete impl;
	delete p;
}

WindowController * WindowController::instance() {
	static WindowController windowController;
	return &windowController;
}


void WindowController::updateHotkeys() {
	p->hotkeyMap.clear();
	Hotkey hotkey;
	for(WindowHandleList::const_iterator it = p->windowHandleList.begin();
		it != p->windowHandleList.end(); ++it) {
		hotkey = getNextHotkey(hotkey);
		p->hotkeyMap[* it] = hotkey;
	}
}

void WindowController::updateWindowInfoList() {
	impl->updateWindowInfoList();
	WindowHandleList tmp1 = p->windowHandleList, tmp2 = impl->windowHandleList();

	if(tmp1 != tmp2) {
		compareAndEmitDisappeared(tmp1, tmp2);
		p->windowHandleList = tmp2;
		compareAndEmitAppeared(tmp1, tmp2);

		emit windowHandleListChanged(p->windowHandleList);

		updateHotkeys();
	}

	updateCaptionCache();

	cleanUpPreviewCache();
	cleanUpCaptionCache();
}

void WindowController::cleanUpPreviewCache() {
	WindowHandleSet set = windowHandleList().toSet();
	PixmapCache::iterator it = p->previewCache.begin(); 
	while(it != p->previewCache.end()){
		if(!set.contains(it.key())) {
			it = p->previewCache.erase(it);
		} else {
			++it;
		}
	}
}

void WindowController::cleanUpCaptionCache() {
	WindowHandleSet set = windowHandleList().toSet();
	StringCache::iterator it = p->captionCache.begin(); 
	while(it != p->captionCache.end()){
		if(!set.contains(it.key())) {
			it = p->captionCache.erase(it);
		} else {
			++it;
		}
	}
}

void WindowController::updateCaptionCache() {
	StringCache tmp = p->captionCache;
	for(WindowHandleList::const_iterator it = p->windowHandleList.begin();
		it != p->windowHandleList.end(); ++it) {
		p->captionCache[*it] = impl->windowCaption(*it);
		if(tmp.contains(*it) && tmp[*it] != p->captionCache[*it]) {
			emit windowCaptionChanged(*it);
		}
	}
}

WindowHandleList WindowController::windowHandleList() const {
	return p->windowHandleList;
}

QString WindowController::windowCaption(int handle) const {
	if(p->captionCache.contains(handle)) {
		return p->captionCache[handle];
	}
	return QString::null;
}

QString WindowController::windowClassName(int handle) const {
	return impl->windowClassName(handle);
}

QString WindowController::windowDescription(int handle) const {
	return impl->windowDescription(handle);
}

QPixmap WindowController::windowHotkeyPixmap(int handle) const {
	Hotkey hk = windowHotkey(handle);
	QString s = hotkeyToString(hk);
	QImage image(QSize(16 * hk.size(), 16), QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&image);
	painter.setPen(Qt::white);
	painter.setFont(QFont("sans", 12, QFont::Bold));
	painter.fillRect(image.rect(), QBrush(Qt::darkBlue));
	painter.drawText(image.rect(), Qt::AlignCenter, s);
	painter.end();
	return QPixmap::fromImage(image);
}

Hotkey WindowController::windowHotkey(int handle) const {
	if(p->hotkeyMap.contains(handle)) {
		return p->hotkeyMap[handle];
	}
	return Hotkey();
}

int WindowController::findWindowByHotkey(const Hotkey &hk) const {
	for(HotkeyMap::const_iterator it = p->hotkeyMap.begin();
		it != p->hotkeyMap.end(); ++it) {
		if(it.value() == hk) {
			return it.key();
		}
	}
	return 0;
}

bool isHotkey(const Hotkey &like, const Hotkey &that) {
	for(int i = 0; i < that.size(); ++i) {
		if(i >= like.size() || like[i] != that[i]) {
			return false;
		}
	}
	return true;
}

int WindowController::countHotkeyLike(const Hotkey &that) const {
	int i = 0;
	for(HotkeyMap::const_iterator it = p->hotkeyMap.begin();
		it != p->hotkeyMap.end(); ++it) {
		if(isHotkey(it.value(), that)) {
			++i;
		}
	}
	return i;
}

QPixmap WindowController::windowPreview(int handle) const {
	{
		QReadLocker locker(&p->previewCacheLock);
		if(p->previewCache.contains(handle)) {
			QPixmap pixmap = p->previewCache[handle];
			if(!pixmap.isNull()) {
				return pixmap;
			}
		}
	}

	/*
	QString key = QString("pw_%1_%2").arg(handle).arg(QTime::currentTime().minute());
	if(!QPixmapCache::find(key, pixmap)) {
		pixmap = impl->windowPixmap(handle);
		QSize reducedSize = pixmap.size() * 0.2;
		pixmap = pixmap.scaled(reducedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmapCache::insert(key, pixmap);
	}
	*/

	/*const_cast<WindowController *>(this)->startWindowPreview(handle);*/

	return p->defaultPreview;
}

QPixmap WindowController::desktopPixmap() const {
	return impl->desktopPixmap();
}

QPixmap WindowController::windowPreviewWithIcon(int handle) const {
	QPixmap previewPixmap, iconPixmap = windowIcon(handle), hotkeyPixmap;
	
	if(p->isHotkeyLabelsShown) {
		hotkeyPixmap = windowHotkeyPixmap(handle);
	}


	if(!isMinimized(handle)) {
		previewPixmap = windowPreview(handle);
	}

	QSize previewSize = previewPixmap.size();
	if(previewPixmap.isNull()) {
		previewSize = QSize(200, 
			qBound(32, iconPixmap.size().height(), 100));
	}

	QImage image(previewSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(image.rect(), Qt::transparent);

	if(!iconPixmap.isNull()) {
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		int left = previewSize.width() - iconPixmap.size().width();
		int top = previewSize.height() - iconPixmap.size().height();
		if(previewPixmap.isNull()) {
			left /= 2;
			top /= 2;
		}
		painter.drawImage(
			left, 
			top, 
			iconPixmap.toImage()
		);
	}

	if(!hotkeyPixmap.isNull()) {
		painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
		int left = 0;
		int top = 0;
		painter.drawImage(
			left, 
			top, 
			hotkeyPixmap.toImage()
		);
	}

	if(!previewPixmap.isNull()) {
		painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
		painter.drawImage(0, 0, previewPixmap.toImage());
	}
    painter.end();

    return QPixmap::fromImage(image);
}

QPixmap WindowController::windowIcon(int handle) const {
	if(!handle) {
		return QPixmap();
	}

	if(p->iconCache.contains(handle)) {
		return p->iconCache[handle];
	}

	QPixmap pixmap = impl->windowIcon(handle);
	p->iconCache[handle] = pixmap;
	return pixmap;
}

bool WindowController::isMinimized(int handle)  const {
	return impl->isMinimized(handle);
}

void WindowController::selectWindow(int handle) {
	impl->selectWindow(handle);
}

void WindowController::startSelectWindow(int handle) {
	p->selectThreads.start(new WindowSelect(handle));
}

void WindowController::cascadeWindows(const WindowHandleList &list) {
	impl->cascadeWindows(list);
}

void WindowController::tileWindows(const WindowHandleList &list, ETile tile) {
	impl->tileWindows(list, tile);
}

void WindowController::minimizeWindow(int handle) {
	impl->minimizeWindow(handle);
}

void WindowController::ignoreWindow(int handle)
{
	emit windowIgnoreRequested(
		 impl->windowCaption(handle),
		 impl->windowClassName(handle)
	);
}

void WindowController::maximizeWindow(int handle) {
	impl->maximizeWindow(handle);
}

void WindowController::closeWindow(int handle) {
	impl->closeWindow(handle);
}

void WindowController::startWindowPreview(int handle) {
	//TODO: threading
#ifdef Q_WS_WIN
	p->previewThreads.start(new WindowPreview(handle));
#endif
#ifdef Q_WS_X11
	QPixmap pixmap = WindowController::instance()->impl->windowPixmap(handle);
	QSize reducedSize = pixmap.size() * 0.2;
	pixmap = pixmap.scaled(reducedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	WindowController::instance()->setWindowPreview(handle, pixmap);
#endif
}

void WindowController::setWindowPreview(int handle, const QPixmap &pixmap) {
	QSize oldSize, newSize = pixmap.size();
	{
		QReadLocker locker(&p->previewCacheLock);
		if(p->previewCache.contains(handle)) {
			oldSize = p->previewCache[handle].size();
		}
	}
	{
		QWriteLocker locker(&p->previewCacheLock);
		p->previewCache[handle] = pixmap;
	}
	emit windowPreviewChanged(handle);
	if(oldSize != newSize) {
		emit windowPreviewSizeChanged(handle);
	}
}

void WindowController::startUpdateWindowPreviews() {
	WindowHandleList list = windowHandleList();
	for(WindowHandleList::const_iterator it = list.begin(); it != list.end(); ++it) {
		startWindowPreview(*it);
	}
}

void WindowController::apply(const Config &config) {
	p->isHotkeyLabelsShown = config.showHotkeyLabels();
}

void WindowController::compareAndEmitAppeared(
	const WindowHandleList &oldList,
	const WindowHandleList &newList
) {
	for(WindowHandleList::const_iterator it = oldList.begin();
		it != oldList.end(); ++it) {
		if(!newList.contains(*it)) {
			emit windowDisappeared(*it);
		}
	}
}

void WindowController::compareAndEmitDisappeared(
	const WindowHandleList &oldList,
	const WindowHandleList &newList
) {
	for(WindowHandleList::const_iterator it = newList.begin();
		it != newList.end(); ++it) {
		if(!oldList.contains(*it)) {
			emit windowAppeared(*it);
		}
	}
}

}
