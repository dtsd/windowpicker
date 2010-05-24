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
#include "windowpicker/keysequence_edit.h"

#include "windowpicker/shortcut_controller.h"

#include <QKeyEvent>
#include <QKeySequence>

namespace WindowPicker {

struct KeySequenceEditPrivate {
	QKeySequence keySequence;
};

int getModifiersCount(int mods) {
	int count = 0;
	if(mods & Qt::ShiftModifier) {
		++count;
	}
	if(mods & Qt::ControlModifier) {
		++count;
	}
	if(mods & Qt::AltModifier) {
		++count;
	}
	if(mods & Qt::MetaModifier) {
		++count;
	}
	return count;
}

KeySequenceEdit::KeySequenceEdit(QWidget *parent) 
	:QLineEdit(parent), p(new KeySequenceEditPrivate) {
	setupUi();
};

KeySequenceEdit::~KeySequenceEdit() {
	delete p;
}

void KeySequenceEdit::keyPressEvent(QKeyEvent *e) {
	if(e->key() == Qt::Key_Backspace) {
		setKeySequence(QKeySequence());
		return;
	}
	if((e->key() >= Qt::Key_A && e->key() <= Qt::Key_Z)
		|| (e->key() >= Qt::Key_0 && e->key() <= Qt::Key_9)
		|| (e->key() == Qt::Key_Escape)
		|| (e->key() == Qt::Key_Tab)
		) {

		if(getModifiersCount(e->modifiers()) == 0) {
			return;
		}

		int qtKeyCode = e->key();

		if(e->modifiers() & Qt::ShiftModifier) {
			qtKeyCode += Qt::SHIFT;
		}
		if(e->modifiers() & Qt::ControlModifier) {
			qtKeyCode += Qt::CTRL;
		}
		if(e->modifiers() & Qt::AltModifier) {
			qtKeyCode += Qt::ALT;
		}
		if(e->modifiers() & Qt::MetaModifier) {
			qtKeyCode += Qt::META;
		}

		setKeySequence(QKeySequence(qtKeyCode));

		return;
	}
	//QLineEdit::keyPressEvent(e);
}; 

void KeySequenceEdit::focusInEvent(QFocusEvent *e) {
	QLineEdit::focusInEvent(e);
	ShortcutController::instance()->disable();
}

void KeySequenceEdit::focusOutEvent(QFocusEvent *e) {
	QLineEdit::focusOutEvent(e);
	ShortcutController::instance()->enable();
}

void KeySequenceEdit::setupUi() {
	//setReadOnly(true);
}

void KeySequenceEdit::setKeySequence(const QKeySequence &value) {
	if(p->keySequence != value) {
		p->keySequence = value;
		setText(p->keySequence.toString());
		emit keySequenceChanged(p->keySequence);
	}
}

QKeySequence KeySequenceEdit::keySequence() const {
	return p->keySequence;
}

}
