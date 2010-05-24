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
#if !defined(WINDOWPICKER_SHORTCUT_CONTROLLER_IMPL_H)
#define WINDOWPICKER_SHORTCUT_CONTROLLER_IMPL_H

class QKeySequence; 

namespace WindowPicker {

typedef bool (*ShortcutCallback)(const QKeySequence &, bool);

class ShortcutControllerImpl {
public:
	ShortcutControllerImpl(ShortcutCallback callback);

	bool installShortcut(const QKeySequence &);
	bool removeShortcut(const QKeySequence &);
};

}

#endif
