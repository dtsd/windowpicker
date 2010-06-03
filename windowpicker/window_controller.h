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
#if !defined(WINDOWPICKER_WINDOW_CONTROLLER_H)
#define WINDOWPICKER_WINDOW_CONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPixmap>
#include <QString>

namespace WindowPicker {

class Config;

typedef QList<int> WindowHandleList;
typedef QSet<int> WindowHandleSet;
typedef QList<int> Hotkey;

class WindowControllerImpl;
struct WindowControllerPrivate;
class WindowPriview;

class WindowController : public QObject {
	Q_OBJECT
public:
	static WindowController *instance();


	WindowHandleList windowHandleList() const;
	void updateWindowInfoList();
	void startUpdateWindowPreviews();

	QString	windowCaption(int) const;
	QString	windowDescription(int) const;

	QPixmap windowPreview(int) const;
	QPixmap windowIcon(int) const;
	QPixmap desktopPixmap() const;

	QPixmap windowPreviewWithIcon(int) const;

	bool isMinimized(int) const;

	void selectWindow(int);

	void cascadeWindows(const WindowHandleList&);
	enum ETile { ETile_Vertically, ETile_Horizontally };
	void tileWindows(const WindowHandleList&, ETile);

	void minimizeWindow(int);
	void maximizeWindow(int);
	void closeWindow(int);

	QPixmap windowHotkeyPixmap(int) const;
	Hotkey windowHotkey(int) const;
	int findWindowByHotkey(const Hotkey&) const;
	int countHotkeyLike(const Hotkey &that) const;
public slots:
	void startSelectWindow(int);
	void startWindowPreview(int handle);
	void apply(const Config &);
signals:
	void windowPreviewChanged(int handle);
	void windowPreviewSizeChanged(int handle);
	void windowCaptionChanged(int handle);

	void windowAppeared(int handle);
	void windowDisappeared(int handle);

	void windowHandleListChanged(const WindowHandleList &);
private:
	void setWindowPreview(int handle, const QPixmap &);
	WindowControllerImpl *impl;
	void updateHotkeys();
	void updateCaptionCache();
	void compareAndEmitAppeared(
		const WindowHandleList &oldList,
		const WindowHandleList &newList
	);
	void compareAndEmitDisappeared(
		const WindowHandleList &oldList,
		const WindowHandleList &newList
	);

	void cleanUpPreviewCache();
	void cleanUpCaptionCache();

	WindowController();
	~WindowController();
	WindowController(const WindowController &) :QObject() {};
	WindowController & operator=(const WindowController&) { return *this; };

	WindowControllerPrivate *p;

	friend class WindowPreview;
	friend class WindowSelect;
};

}
#endif
