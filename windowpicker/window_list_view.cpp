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
#include "windowpicker/window_list_view.h"

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>

namespace WindowPicker {

WindowListView::WindowListView(QWidget *parent) 
	: QListView(parent), m_singleClickToActivate(false), m_defaultCursor(cursor())
{
    setViewMode(QListView::IconMode);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSpacing(10);
};


void WindowListView::setSingleClickToActivate(bool b) {
	if(m_singleClickToActivate != b) {
		m_singleClickToActivate = b;

		if(m_singleClickToActivate) {
			setCursor(Qt::PointingHandCursor);
		} else {
			setCursor(m_defaultCursor);
		}
	}
};

void WindowListView::mouseMoveEvent(QMouseEvent *me) {
	QListView::mouseMoveEvent(me);
	QModelIndex index = indexAt(me->pos());
	if(index != m_hoveredIndex) {
		m_hoveredIndex = index;
		emit hoveredIndexChanged(m_hoveredIndex);
	}
}


void WindowListView::mouseReleaseEvent(QMouseEvent *event) {
	QListView::mouseReleaseEvent(event);

	if(m_singleClickToActivate 
		&& !sigleClickSetByStyle() 
		&& event->button() != Qt::RightButton
		&& !(QApplication::keyboardModifiers() & Qt::ControlModifier))
	{
		QPoint pos = event->pos();
		QPersistentModelIndex index = indexAt(pos);
		emit activated(index);
	}
};

bool WindowListView::sigleClickSetByStyle() const {
	return style()->styleHint(
		QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, this
	);
};

void WindowListView::keyPressEvent(QKeyEvent *e) {
	QListView::keyPressEvent(e);
}

}
