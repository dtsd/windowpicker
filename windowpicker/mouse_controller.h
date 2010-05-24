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
#if !defined(WINDOWPICKER_MOUSE_CONTROLLER_H)
#define WINDOWPICKER_MOUSE_CONTROLLER_H

#include <QObject>

class QRect;
class QPoint;

namespace WindowPicker {

class MouseControllerImpl;

class MouseController : public QObject {
Q_OBJECT
public:
	static MouseController *instance();

	enum ECorner { 
			ECorner_TopRight = 1, 
			ECorner_TopLeft = 2, 
			ECorner_BottonRight = 3, 
			ECorner_BottomLeft = 4 
	};

	bool installHitCorner(ECorner, const QObject *recv, const char *slot);
	bool removeHitCorner(ECorner, const QObject *recv, const char *slot);

	bool installHitCorner(const QRect &, const QObject *recv, const char *slot);
	bool removeHitCorner(const QRect &, const QObject *recv, const char *slot);

	void enable();
	void disable();
private:
	static bool emitMouseMove(const QPoint &);

	QRect getHitCornerRect(ECorner);

	MouseController();
	~MouseController();
	MouseController(const MouseController &) :QObject(0) {};
	MouseController & operator=(const MouseController &) { return *this; };


	MouseControllerImpl *impl;
	struct Private;
	Private *p;
};

}

#endif
