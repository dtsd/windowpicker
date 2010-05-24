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
#if !defined(WINDOWPICKER_SHORTCUT_CONTROLLER_H)
#define WINDOWPICKER_SHORTCUT_CONTROLLER_H

#include <QObject>

class QKeySequence;

namespace WindowPicker {

class ShortcutControllerImpl;

class ShortcutController : public QObject {
Q_OBJECT
public:
	static ShortcutController *instance();

	bool installKeyDown(const QKeySequence &, const QObject *recv, const char *slot);
	bool removeKeyDown(const QKeySequence &, const QObject *recv, const char *slot);

	bool installKeyUp(const QKeySequence &, const QObject *recv, const char *slot);
	bool removeKeyUp(const QKeySequence &, const QObject *recv, const char *slot);

	void enable();
	void disable();

private:
	static bool emitShortcutInvoked(const QKeySequence &, bool isDown);

	ShortcutController();
	~ShortcutController();
	ShortcutController(const ShortcutController &) :QObject(0) {};
	ShortcutController & operator=(const ShortcutController &) { return *this; };


	ShortcutControllerImpl *impl;
	struct Private;
	Private *p;
};

}

#endif
