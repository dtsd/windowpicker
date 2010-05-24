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
#include "windowpicker/shortcut_controller.h"
#include "windowpicker/shortcut_controller_impl.h"
#include "windowpicker/actuator.h"

#include <QKeySequence>
#include <QMap>
#include <QAction>

namespace WindowPicker {

typedef QMap<QKeySequence, Actuator*> ShortcutActionMap;

struct ShortcutController::Private {
	Private() :isEnabled(true) {};
	ShortcutActionMap keyDownActionMap;
	ShortcutActionMap keyUpActionMap;
	bool isEnabled;
};

ShortcutController::ShortcutController() 
	:impl(new ShortcutControllerImpl(&ShortcutController::emitShortcutInvoked)), 
	p(new Private) {
}

ShortcutController::~ShortcutController() {
	for(ShortcutActionMap::const_iterator it = p->keyDownActionMap.begin(); it != p->keyDownActionMap.end(); ++it) {
		impl->removeShortcut(it.key());
		it.value()->removeAllSlots();
	}

	for(ShortcutActionMap::const_iterator it = p->keyUpActionMap.begin(); it != p->keyUpActionMap.end(); ++it) {
		impl->removeShortcut(it.key());
		it.value()->removeAllSlots();
	}

	delete impl;
	delete p;
}

ShortcutController * ShortcutController::instance() {
	static ShortcutController shortcutController;
	return &shortcutController;
}

bool ShortcutController::removeKeyDown(
	const QKeySequence &ks, 
	const QObject *recv,
	const char *slot) {

	return p->keyDownActionMap.contains(ks)
		&& p->keyDownActionMap[ks]->removeSlot(recv,slot);
}

bool ShortcutController::removeKeyUp(
	const QKeySequence &ks, 
	const QObject *recv,
	const char *slot) {

	return p->keyUpActionMap.contains(ks) 
		&& p->keyUpActionMap[ks]->removeSlot(recv,slot);
}

bool ShortcutController::emitShortcutInvoked(const QKeySequence &ks, bool isDown) {
	if(!ShortcutController::instance()->p->isEnabled) {
		return false;
	}

	if(isDown) {
		if(ShortcutController::instance()->p->keyDownActionMap.contains(ks)
			&& ShortcutController::instance()->p->keyDownActionMap[ks]->hasSlots()) {
			ShortcutController::instance()->p->keyDownActionMap[ks]->trigger();
			return true; 
		}
	}

	if(!isDown) {
		if(ShortcutController::instance()->p->keyUpActionMap.contains(ks)
			&& ShortcutController::instance()->p->keyUpActionMap[ks]->hasSlots())  {
			ShortcutController::instance()->p->keyUpActionMap[ks]->trigger();
			return true;
		}
	}

	return false;
}

bool ShortcutController::installKeyDown(
	const QKeySequence &ks, 
	const QObject *recv, 
	const char *slot) {

	if(!p->keyDownActionMap.contains(ks)) {
		p->keyDownActionMap[ks] = new Actuator(this);
		impl->installShortcut(ks);
	}

	return p->keyDownActionMap[ks]->addSlot(recv, slot);
}

bool ShortcutController::installKeyUp(
	const QKeySequence &ks, 
	const QObject *recv, 
	const char *slot) {

	if(!p->keyUpActionMap.contains(ks)) {
		p->keyUpActionMap[ks] = new Actuator(this);
		impl->installShortcut(ks);
	}

	return p->keyUpActionMap[ks]->addSlot(recv, slot);
}

void ShortcutController::enable() {
	p->isEnabled = true;
}

void ShortcutController::disable() {
	p->isEnabled = false;
}

}
