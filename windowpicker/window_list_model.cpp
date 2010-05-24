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
#include "windowpicker/window_list_model.h"

#include "windowpicker/window_controller.h"

namespace WindowPicker {

WindowListModel::WindowListModel() {
	connect(
		WindowController::instance(), 
		SIGNAL(windowPreviewChanged(int)), 
		this, 
		SLOT(emitDataChanged(int))
	);
	connect(
		WindowController::instance(), 
		SIGNAL(windowCaptionChanged(int)), 
		this, 
		SLOT(emitDataChanged(int))
	);
	connect(
		WindowController::instance(), 
		SIGNAL(windowPreviewSizeChanged(int)), 
		this, 
		SLOT(emitLayoutChanged(int))
	);

	connect(
		WindowController::instance(), 
		SIGNAL(windowHandleListChanged(const WindowHandleList &)), 
		this, 
		SLOT(refresh())
	);
}

int WindowListModel::rowCount(const QModelIndex & /*parent*/) const {
	return WindowController::instance()->windowHandleList().size();
}


int WindowListModel::columnCount(const QModelIndex & /*parent*/) const {
	return 1;
}

QVariant WindowListModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid()) {
		return QVariant();
	}

	switch(role) {
		case Qt::DisplayRole:
		case Qt::EditRole: 
		case Qt::ToolTipRole: 
		case Qt::DecorationRole:
		case Qt::SizeHintRole:
		case ERole_Handle:
			break;
		default:
			return QVariant();
	}

	WindowHandleList list = WindowController::instance()->windowHandleList();
	if((index.row() < 0) || (index.row() >= list.size())) {
		return QVariant();
	}

	if(index.column() != 0) {
		return QVariant();
	}

	int handle = list.at(index.row());

	switch(role) {
		case WindowListModel::ERole_Handle:
			return QVariant(handle);
		case Qt::DisplayRole:
		case Qt::EditRole: 
			return WindowController::instance()->windowCaption(handle);
		case Qt::ToolTipRole: 
			break;
			//return WindowController::instance()->windowDescription(handle);
		case Qt::DecorationRole:
			return WindowController::instance()->windowPreviewWithIcon(handle);
		case Qt::SizeHintRole:
			int handle = list.at(index.row());
			QPixmap pixmap = WindowController::instance()->windowPreviewWithIcon(handle);
			QSize size = pixmap.size();
			size.setHeight(size.height() + 30);
			return size/*.expandedTo(QSize(200, 200))*/;
	};

	return QVariant();
}

QVariant WindowListModel::headerData(int /*section*/, Qt::Orientation orientation, int role) const 
{
	if(orientation == Qt::Horizontal) {
		if(role == Qt::SizeHintRole) {
			return QSize(0, 0);
		}
	} else if (orientation == Qt::Vertical) {
		if(role == Qt::SizeHintRole) {
			return QSize(0, 0);
		}
	}
	return QVariant();
}

Qt::ItemFlags WindowListModel::flags(const QModelIndex & index) const {
	Qt::ItemFlags f = QAbstractTableModel::flags(index);
	if(index.isValid()) {
		/*if(...) {
			f |= Qt::ItemIsEditable;
		} else {
			f &= ~ (Qt::ItemIsEnabled);
		}*/
	}
	return f;
}

void WindowListModel::refresh() {
	beginResetModel();
	endResetModel();
}

void WindowListModel::emitDataChanged(int handle) {
	WindowHandleList list = WindowController::instance()->windowHandleList();
	int listIndex = list.indexOf(handle);
	if(listIndex != -1) {
		emit dataChanged(index(listIndex, 0), index(listIndex, 0));
	}

}

void WindowListModel::emitLayoutChanged(int /*handle*/) {
	emit layoutChanged();
}

void WindowListModel::emitRowsInserted(int handle) {
	/*WindowHandleList list = WindowController::instance()->windowHandleList();
	int listIndex = list.indexOf(handle);
	if(listIndex != -1) {
		emit rowsInserted(QModelIndex(), listIndex, listIndex);
	}*/
	emit layoutChanged();
}

void WindowListModel::emitRowsRemoved(int handle) {
	/*WindowHandleList list = WindowController::instance()->windowHandleList();
	int listIndex = list.indexOf(handle);
	if(listIndex != -1) {
		emit rowsRemoved(QModelIndex(), listIndex, listIndex);
	}*/
	emit layoutChanged();
}

}
