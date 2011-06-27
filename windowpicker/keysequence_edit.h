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
#if !defined (WINDOWPICKER_KEYSEQUENCE_EDIT_H)
#define WINDOWPICKER_KEYSEQUENCE_EDIT_H

#include <QLineEdit> 

class QKeyEvent;
class QFocusEvent;
class QKeySequence;

namespace WindowPicker {

struct KeySequenceEditPrivate;

class KeySequenceEdit: public QLineEdit {
Q_OBJECT
public:
	KeySequenceEdit(QWidget *parent = 0);
	virtual ~KeySequenceEdit();

	QKeySequence keySequence() const;
	void setKeySequence(const QKeySequence &);
signals:
	void keySequenceChanged(const QKeySequence &);
protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void focusInEvent(QFocusEvent *e);
	virtual void focusOutEvent(QFocusEvent *e);
private:
	KeySequenceEdit(const KeySequenceEdit &) : QLineEdit() {};
	KeySequenceEdit& operator=(const KeySequenceEdit &) { return *this; };

	void setupUi();
	KeySequenceEditPrivate *p;
};

}

#endif
