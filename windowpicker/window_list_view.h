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
#if !defined(WINDOWPICKER_WINDOW_LIST_VIEW_H)
#define WINDOWPICKER_WINDOW_LIST_VIEW_H

#include <QCursor>
#include <QListView>

class QModelIndex;

namespace WindowPicker {

class WindowListView : public QListView {
	Q_OBJECT
public:
	WindowListView(QWidget *parent);

	bool sigleClickSetByStyle() const;
	void setSingleClickToActivate(bool b);

	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *e); 
signals:
	void hoveredIndexChanged(const QModelIndex &);
private:
	bool m_singleClickToActivate;
	QCursor m_defaultCursor;
	QModelIndex m_hoveredIndex;
};

}

#endif
