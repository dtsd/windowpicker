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
#include "windowpicker/mouse_controller.h"
#include "windowpicker/mouse_controller_impl.h"
#include "windowpicker/actuator.h"

#include <QRect>
#include <QMessageBox>
#include <QMap>
#include <QApplication>
#include <QDesktopWidget>
#include <QSize>
#include <QAction>
#include <QPoint>

namespace WindowPicker {

class RectWithHash {
public:
	RectWithHash(const QRect &rect) : m_rect(rect) {};
	operator QRect() const { return m_rect; };
	bool operator<(const RectWithHash &hashRect) const {
		return this->hash() < hashRect.hash();
	};
	bool operator==(const RectWithHash &hashRect) const {
		return this->hash() == hashRect.hash();
	};
	QString hash() const {
		return QString("%1:%2:%3:%4")
			.arg(m_rect.x())
			.arg(m_rect.y())
			.arg(m_rect.width())
			.arg(m_rect.height());
	};
private:
	QRect m_rect;
};


typedef QMap<RectWithHash, Actuator*> RectActionMap;

struct MouseController::Private {
	Private() :isEnabled(true) {};
	RectActionMap rectActionMap;
	bool isEnabled;
};

MouseController::MouseController() 
	:impl(new MouseControllerImpl(&MouseController::emitMouseMove)), 
	p(new Private) {
}

MouseController::~MouseController() {
	if(!p->rectActionMap.isEmpty()) {
		impl->remove();
		for(RectActionMap::const_iterator it = p->rectActionMap.begin(); 
			it != p->rectActionMap.end(); ++it) 
		{
			it.value()->removeAllSlots();
		}
	}

	delete impl;
	delete p;
}

MouseController * MouseController::instance() {
	static MouseController shortcutController;
	return &shortcutController;
}


bool MouseController::emitMouseMove(const QPoint &pt) {
	if(!MouseController::instance()->p->isEnabled) {
		return false;
	}

	for(RectActionMap::const_iterator it 
			= MouseController::instance()->p->rectActionMap.begin(); 
		it != MouseController::instance()->p->rectActionMap.end(); 
		++it) 
	{
		QRect rect = it.key();
		if(rect.contains(pt)) {
			it.value()->trigger();
			return true;
		}
	}
	

	return false;
}


bool MouseController::installHitCorner(
	ECorner corner,
	const QObject *recv, 
	const char *slot) {

	QRect rect = getHitCornerRect(corner);
	return installHitCorner(rect, recv, slot);
}

bool MouseController::installHitCorner(
	const QRect &rect, 
	const QObject *recv, 
	const char *slot) {

	if(!p->rectActionMap.contains(rect)) {
		
		if(p->rectActionMap.isEmpty()) {
			impl->install();
		}
		p->rectActionMap[rect] = new Actuator(this);
	}

	return p->rectActionMap[rect]->addSlot(recv, slot);
}

bool MouseController::removeHitCorner(
	ECorner corner,
	const QObject *recv,
	const char *slot) {

	QRect rect = getHitCornerRect(corner);
	return removeHitCorner(rect, recv, slot);
}

bool MouseController::removeHitCorner(
	const QRect &rect, 
	const QObject *recv,
	const char *slot) {

	bool r = p->rectActionMap.contains(rect) 
		&& p->rectActionMap[rect]->removeSlot(recv,slot);

	if(r) {
		if(!p->rectActionMap[rect]->hasSlots()) {
			p->rectActionMap.remove(rect);
			if(p->rectActionMap.isEmpty()) {
				impl->remove();
			}
		}
	}
	return r;
}

void MouseController::enable() {
	p->isEnabled = true;
}

void MouseController::disable() {
	p->isEnabled = false;
}

QRect MouseController::getHitCornerRect(ECorner corner) {
	QSize desktopSize = static_cast<QApplication *>(
		QApplication::instance()
	)->desktop()->availableGeometry().size();

	QSize rectSize = desktopSize * 0.02;
	QPoint topLeft = QPoint(0, 0);

	if(corner == ECorner_BottonRight || corner == ECorner_TopRight) {
		topLeft.setX(desktopSize.width() - rectSize.width());
	}

	if(corner == ECorner_BottonRight || corner == ECorner_BottomLeft) {
		topLeft.setY(desktopSize.height() - rectSize.height());
	}

	return QRect(topLeft, rectSize);
}


}
