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
#if !defined(WINDOWPICKER_TASK_LIST_DIALOG_H)
#define WINDOWPICKER_TASK_LIST_DIALOG_H

#include <QFrame>

class QWidget;
class QFocusEvent;
class QEvent;
class QKeyEvent;
class QModelIndex;

namespace WindowPicker {

class Config;

class WindowListDialog : public QFrame {
	Q_OBJECT
public:
	WindowListDialog(QWidget *parent = 0);
	virtual ~WindowListDialog();
	virtual bool eventFilter(QObject *obj, QEvent *event);
protected:
	virtual void showEvent(QShowEvent *); 
	virtual void focusOutEvent(QFocusEvent *); 
	virtual void hideEvent (QHideEvent *); 
	virtual void changeEvent(QEvent *);
public slots:
	void showThisWindow();
	void hideThisWindow();
	void selectNextWindow();
	void selectPreviousWindow();
	void stopSelectNextPrev();

	void apply(const Config&);
	void dummy();
protected slots:
	void selectWindow(const QModelIndex &);
	void showContextMenu(const QPoint &);
	void minimizeWindow(const QModelIndex &);
	void ignoreWindow(const QModelIndex &);
	void maximizeWindow(const QModelIndex &);
	void closeWindow(const QModelIndex &);

	void selectSelected();
	void cascadeSelected();
	void tileHorizontallySelected();
	void tileVerticallySelected();
	void minimizeSelected();
	void maximizeSelected();
	void closeSelected();
	void ignoreSelected();

	void triggerHotkey();
	void displayHoveredWindowCaption(const QModelIndex &);

	void updateWindowList();
	void updateWindowPreviews();

	void delayedUpdateWindowPreview(int);
	void notifyAboutNewVersion(const QString &version, const QString &url);
private:
	void setupUi();
	void translateUi();
	void setupActions();
	void setupContextMenu();

	struct Private;
	Private *p;
};

}

#endif
