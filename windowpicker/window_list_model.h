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
#if !defined(WINDOWPICKER_WINDOW_LIST_MODEL_H)
#define WINDOWPICKER_WINDOW_LIST_MODEL_H

#include <QAbstractTableModel>

namespace WindowPicker {

class WindowListModel : public QAbstractTableModel {
	Q_OBJECT
public:
	enum ERole { ERole_Handle = Qt::UserRole + 1 };
	WindowListModel();			
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, 
		int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;

public slots:
	void refresh();
protected slots:
	void emitDataChanged(int handle);
	void emitLayoutChanged(int handle);
	void emitRowsInserted(int handle);
	void emitRowsRemoved(int handle);
};

}

#endif
