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
#include "windowpicker/window_list_dialog.h"


#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QStringListModel>
#include <QLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QMenu>
#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>

#include "windowpicker/window_controller.h"
#include "windowpicker/shortcut_controller.h"
#include "windowpicker/mouse_controller.h"
#include "windowpicker/window_list_model.h"
#include "windowpicker/window_list_view.h"
#include "windowpicker/config.h"

namespace WindowPicker {

struct WindowListDialog::Private { 
	Private() :view(0), menu(0),
		selectAction(0),
		cascadeAction(0), tileVerticallyAction(0), tileHorizontallyAction(0),
		minimizeAction(0), maximizeAction(0), closeAction(0),
		isNextPrev(false), isHotkeyLabelsShown(false),
		label(0), pixmapLabel(0), defaultHitCorner(0) {};

	WindowListView *view;
	QMenu *menu;
	QAction *selectAction;
	QAction *cascadeAction, *tileVerticallyAction, *tileHorizontallyAction;
	QAction *minimizeAction, *maximizeAction, *closeAction;
	QKeySequence defaultKeySequence;
	bool isNextPrev;
	bool isHotkeyLabelsShown;
	Hotkey hotkey;
	QTimer hotkeyTimer, updateWindowListTimer, updateWindowPreviewsTimer;
	QLabel *label, *pixmapLabel;
	int defaultHitCorner;
	WindowHandleSet updateWindowPreviewOnShow;
};

WindowListDialog::WindowListDialog(QWidget *parent) 
	: QFrame(parent ,  Qt::Popup /*| Qt::FramelessWindowHint*/), p(new Private) {
	setupActions();
	setupUi();
	setupContextMenu();
	translateUi();

	connect(
		&p->hotkeyTimer,
		SIGNAL(timeout()),
		this,
		SLOT(triggerHotkey())
	);

	connect(
		&p->updateWindowListTimer,
		SIGNAL(timeout()),
		this,
		SLOT(updateWindowList())
	);

	connect(
		&p->updateWindowPreviewsTimer,
		SIGNAL(timeout()),
		this,
		SLOT(updateWindowPreviews())
	);

	connect(
		WindowController::instance(), 
		SIGNAL(windowAppeared(int)),
		this,
		SLOT(delayedUpdateWindowPreview(int))
	);
	connect(
		WindowController::instance(), 
		SIGNAL(windowCaptionChanged(int)),
		this,
		SLOT(delayedUpdateWindowPreview(int))
	);
}

WindowListDialog::~WindowListDialog() {
	delete p;
}

void WindowListDialog::setupActions() {
	p->selectAction = new QAction( this);
	connect(p->selectAction, SIGNAL(triggered()), this, SLOT(selectSelected()));

	p->cascadeAction = new QAction( this);
	connect(p->cascadeAction, SIGNAL(triggered()), this, SLOT(cascadeSelected()));

	p->tileVerticallyAction = new QAction( this);
	connect(p->tileVerticallyAction, SIGNAL(triggered()), this, SLOT(tileVerticallySelected()));

	p->tileHorizontallyAction = new QAction( this);
	connect(p->tileHorizontallyAction, SIGNAL(triggered()), this, SLOT(tileHorizontallySelected()));


	p->minimizeAction = new QAction( this);
	connect(p->minimizeAction, SIGNAL(triggered()), this, SLOT(minimizeSelected()));


	p->maximizeAction = new QAction( this);
	connect(p->maximizeAction, SIGNAL(triggered()), this, SLOT(maximizeSelected()));

	p->closeAction = new QAction( this);
	p->closeAction->setShortcut(QKeySequence(Qt::Key_Delete));
	connect(p->closeAction, SIGNAL(triggered()), this, SLOT(closeSelected()));
}

void WindowListDialog::setupUi() {
	WindowListModel *model = new WindowListModel;

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	p->view = new WindowListView(this);
	p->view->setFrameStyle(QFrame::NoFrame);
	setFocusProxy(p->view);
	p->view->setContextMenuPolicy(Qt::CustomContextMenu);
	p->view->installEventFilter(this);
	p->view->addAction(p->closeAction);
	p->view->setModel(model);

	connect(
		p->view, 
		SIGNAL(customContextMenuRequested(const QPoint &)), 
		this, 
		SLOT(showContextMenu(const QPoint &))
	);

	p->view->setMouseTracking(true);
	connect(
		p->view,
		SIGNAL(hoveredIndexChanged(const QModelIndex &)),
		this,
		SLOT(displayHoveredWindowCaption(const QModelIndex &))
	);
	connect(
		p->view->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this,
		SLOT(displayHoveredWindowCaption(const QModelIndex &))
	);
	connect(
		p->view, 
		SIGNAL(activated(const QModelIndex &)), 
		this, 
		SLOT(selectWindow(const QModelIndex &))
	);


	QFrame *hover = new QFrame;
	hover->setFrameStyle(QFrame::NoFrame);
	hover->setMinimumHeight(50);
	hover->setAutoFillBackground(true);
	QPalette pl = hover->palette();
	pl.setColor(QPalette::Window, p->view->palette().color(QPalette::Base));
	hover->setPalette(pl);
	QHBoxLayout *hoverLayout = new QHBoxLayout(hover);
	hoverLayout->addStretch();

	p->pixmapLabel = new QLabel;
	p->pixmapLabel->setAlignment(Qt::AlignCenter);
	hoverLayout->addWidget(p->pixmapLabel);

	p->label = new QLabel(this);
	QFont f = p->label->font();
	f.setPointSize(f.pointSize() + 5); 
	p->label->setFont(f);
	p->label->setAlignment(Qt::AlignCenter);
	hoverLayout->addWidget(p->label);
	hoverLayout->addStretch();

	layout->addWidget(p->view);
	layout->addWidget(hover);

	QSize size = static_cast<QApplication *>(QApplication::instance())->desktop()->availableGeometry().size();
	size *= 0.9;
	resize(size);

	QSize desktopSize = static_cast<QApplication *>(QApplication::instance())->desktop()->screenGeometry().size();
	QPoint pos((desktopSize.width() - size.width()) / 2, (desktopSize.height() - size.height()) / 2);
	move(pos);
}

void WindowListDialog::setupContextMenu() {
	p->menu = new QMenu(this);

	p->menu->addAction(p->selectAction);
	p->menu->addSeparator();
	p->menu->addAction(p->cascadeAction);
	p->menu->addAction(p->tileHorizontallyAction);
	p->menu->addAction(p->tileVerticallyAction);
	p->menu->addSeparator();
	p->menu->addAction(p->minimizeAction);
	p->menu->addAction(p->maximizeAction);
	p->menu->addSeparator();
	p->menu->addAction(p->closeAction);
}

void WindowListDialog::apply(const Config &config) {
	p->view->setSingleClickToActivate(config.selectWithSingleClick());
	p->isHotkeyLabelsShown = config.showHotkeyLabels();

	QKeySequence 
		alt = QKeySequence(Qt::Key_Alt),
		altTab = QKeySequence(Qt::Key_Tab + Qt::ALT),
		altShiftTab = QKeySequence(Qt::Key_Tab + Qt::ALT + Qt::SHIFT);

	if(p->defaultHitCorner != 0) {
		MouseController::instance()->removeHitCorner(
			(MouseController::ECorner)p->defaultHitCorner, 
			this, 
			SLOT(showThisWindow())
		);
	}

	p->defaultHitCorner = config.defaultHitCorner();
	if(p->defaultHitCorner != 0) {
		MouseController::instance()->installHitCorner(
			(MouseController::ECorner)p->defaultHitCorner, 
			this, 
			SLOT(showThisWindow())
		);
	}

	ShortcutController::instance()->removeKeyDown(
		altTab, 
		this, 
		SLOT(selectNextWindow())
	);

	ShortcutController::instance()->removeKeyDown(
		altShiftTab, 
		this, 
		SLOT(selectPreviousWindow())
	);

	ShortcutController::instance()->removeKeyUp(
		alt, 
		this, 
		SLOT(stopSelectNextPrev())
	);

	if(config.replaceDefaultSwitcher()) {
		ShortcutController::instance()->installKeyDown(
			altTab, 
			this, 
			SLOT(selectNextWindow())
		);

		ShortcutController::instance()->installKeyDown(
			altShiftTab, 
			this, 
			SLOT(selectPreviousWindow())
		);

		ShortcutController::instance()->installKeyUp(
			alt, 
			this, 
			SLOT(stopSelectNextPrev())
		);
	}

	if(!p->defaultKeySequence.isEmpty()) {
		ShortcutController::instance()->removeKeyDown(
			p->defaultKeySequence, 
			this, 
			SLOT(showThisWindow())
		);

		/*ShortcutController::instance()->removeKeyUp(
			p->defaultKeySequence, 
			this, 
			SLOT(dummy())
		);*/
	}

	p->defaultKeySequence = config.defaultKeySequence();
	if(!p->defaultKeySequence.isEmpty()) {
		ShortcutController::instance()->installKeyDown(
			p->defaultKeySequence, 
			this, 
			SLOT(showThisWindow())
		);
		/*ShortcutController::instance()->installKeyUp(
			p->defaultKeySequence, 
			this, 
			SLOT(dummy())
		);*/
	}

}

void WindowListDialog::dummy() {
}

void WindowListDialog::focusOutEvent(QFocusEvent *e) {
	QFrame::focusOutEvent(e);
	hideThisWindow();
}

void WindowListDialog::hideEvent(QHideEvent *e) {
	p->isNextPrev = false;
	QFrame::hideEvent(e);

	//p->updateWindowListTimer.stop();
	p->updateWindowPreviewsTimer.stop();
}

void WindowListDialog::showEvent(QShowEvent *e) {
	p->isNextPrev = false;

	QFrame::showEvent(e);

	while(!p->updateWindowPreviewOnShow.isEmpty()) {
		int handle = *p->updateWindowPreviewOnShow.begin();
		WindowController::instance()->startWindowPreview(handle);
		p->updateWindowPreviewOnShow.remove(handle);
	}

	WindowController::instance()->updateWindowInfoList();
	p->updateWindowListTimer.start(1 * 1000);
	p->updateWindowPreviewsTimer.start(3 * 1000);

	QModelIndex index = p->view->model()->index(0, 0);
	p->view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}

void WindowListDialog::showThisWindow() {
	showFullScreen();
	WindowController::instance()->selectWindow((int)winId());
}

void WindowListDialog::hideThisWindow() {
	hide();
}

void WindowListDialog::selectWindow(const QModelIndex &index) {
	if(!index.isValid()) {
		hideThisWindow();
		return;
	}

	hideThisWindow();

	int handle = p->view->model()->data(index, WindowListModel::ERole_Handle).toInt();
	//WindowController::instance()->startSelectWindow(handle);
	WindowController::instance()->selectWindow(handle);

	delayedUpdateWindowPreview(handle);
}

void WindowListDialog::closeWindow(const QModelIndex &index) {
	if(!index.isValid()) {
		return;
	}
	
	int handle = p->view->model()->data(index, WindowListModel::ERole_Handle).toInt();
	WindowController::instance()->closeWindow(handle);

	QCoreApplication::instance()->processEvents();
	WindowController::instance()->updateWindowInfoList();
}

void WindowListDialog::minimizeWindow(const QModelIndex &index) {
	if(!index.isValid()) {
		return;
	}
	
	int handle = p->view->model()->data(index, WindowListModel::ERole_Handle).toInt();
	WindowController::instance()->minimizeWindow(handle);

	QCoreApplication::instance()->processEvents();
	WindowController::instance()->updateWindowInfoList();
}

void WindowListDialog::maximizeWindow(const QModelIndex &index) {
	if(!index.isValid()) {
		return;
	}
	
	int handle = p->view->model()->data(index, WindowListModel::ERole_Handle).toInt();
	WindowController::instance()->maximizeWindow(handle);

	QCoreApplication::instance()->processEvents();
	WindowController::instance()->updateWindowInfoList();
}

void WindowListDialog::showContextMenu(const QPoint &pos) {
	/*
	QModelIndex index = p->view->indexAt(pos);

	if(!index.isValid()) {
		return;
	}

	int itemIndex = index.row();
	WindowHandleList list = WindowController::instance()->windowHandleList();
	int handle = list.at(itemIndex);
	*/

	p->menu->popup(pos);
}

void WindowListDialog::selectSelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}
	
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		selectWindow(*it);
	}

}

void WindowListDialog::cascadeSelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}

	hideThisWindow();

	WindowHandleList handleList;
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		int handle = p->view->model()->data(*it, WindowListModel::ERole_Handle).toInt();
		handleList.push_back(handle);
	}

	WindowController::instance()->cascadeWindows(handleList);

}

void WindowListDialog::tileHorizontallySelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}

	hideThisWindow();

	WindowHandleList handleList;
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		int handle = p->view->model()->data(*it, WindowListModel::ERole_Handle).toInt();
		handleList.push_back(handle);
	}

	WindowController::instance()->tileWindows(handleList, WindowController::ETile_Horizontally);

}

void WindowListDialog::tileVerticallySelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}

	hideThisWindow();

	WindowHandleList handleList;
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		int handle = p->view->model()->data(*it, WindowListModel::ERole_Handle).toInt();
		handleList.push_back(handle);
	}

	WindowController::instance()->tileWindows(handleList, WindowController::ETile_Vertically);

}

void WindowListDialog::minimizeSelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}
	
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		minimizeWindow(*it);
	}
}

void WindowListDialog::maximizeSelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}
	
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		maximizeWindow(*it);
	}
}

void WindowListDialog::closeSelected() {
	QModelIndexList indexList = p->view->selectionModel()->selectedIndexes();
	
	if(indexList.isEmpty()) {
		return;
	}
	
	for(QModelIndexList::const_iterator it = indexList.begin(); it != indexList.end(); ++it) {
		closeWindow(*it);
	}
}

void WindowListDialog::selectNextWindow() {

	if(!isVisible()) {
		showThisWindow();
	}

	p->isNextPrev = true;

	QModelIndex index = p->view->selectionModel()->currentIndex();
	index = p->view->model()->index(index.row() + 1, 0);
	if(!index.isValid()) {
		index = p->view->model()->index(0, 0);
	}
	p->view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
	//displayHoveredWindowCaption(index);
}

void WindowListDialog::selectPreviousWindow() {

	if(!isVisible()) {
		showThisWindow();
	}

	p->isNextPrev = true;

	QModelIndex index = p->view->selectionModel()->currentIndex();
	index = p->view->model()->index(index.row() - 1, 0);
	if(!index.isValid()) {
		index = p->view->model()->index(p->view->model()->rowCount() - 1, 0);
	}
	p->view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
	//displayHoveredWindowCaption(index);
}


void WindowListDialog::stopSelectNextPrev() {
	if(p->isNextPrev) {
		//TODO: warning
		selectSelected();
	}
}
/*void WindowListDialog::keyPressEvent ( QKeyEvent * event ) {
	QMessageBox::information(0, "event", "keypress");
}*/

void WindowListDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		translateUi();
	} else {
		QFrame::changeEvent(e);
	}
}

void WindowListDialog::translateUi() {
	p->selectAction->setText(tr("Select"));
	p->cascadeAction->setText(tr("Cascade"));
	p->tileVerticallyAction->setText(tr("Tile Vertically"));
	p->tileHorizontallyAction->setText(tr("Tile Horizontally"));
	p->minimizeAction->setText(tr("Minimize"));
	p->maximizeAction->setText(tr("Maximize"));
	p->closeAction->setText(tr("Close"));
}

bool WindowListDialog::eventFilter(QObject *obj, QEvent *event) {
	if (obj == p->view && p->isHotkeyLabelsShown && event->type() == QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		
		if(ke->key() >= Qt::Key_A && ke->key() <= Qt::Key_Z) {
			p->hotkey.push_back(ke->key());
			if(WindowController::instance()->countHotkeyLike(p->hotkey) > 1) {
				p->hotkeyTimer.stop();
				p->hotkeyTimer.start(500);
			} else {
				triggerHotkey();
			}

			return true;
		}
	}
	/*
	TODO: must guarantee there's no scrollbar
	if(obj == p->view && event->type() == QEvent::Wheel) {
		QWheelEvent *we = static_cast<QWheelEvent *>(event);
		QModelIndex index = p->view->selectionModel()->currentIndex();
		int rowCount = p->view->model()->rowCount();
		int scrollTo = index.row() - (we->delta() / 8 / 15);
		while(scrollTo < 0) {
			scrollTo += rowCount;
		}
		while(scrollTo >= rowCount) {
			scrollTo -= rowCount;
		}
		index = p->view->model()->index(scrollTo, 0);
		p->view->selectionModel()->setCurrentIndex(
			index, 
			QItemSelectionModel::ClearAndSelect
		);
		return true;
	}*/

	return QObject::eventFilter(obj, event);
}

void WindowListDialog::triggerHotkey() {
	int handle = WindowController::instance()->findWindowByHotkey(p->hotkey);
	if(handle) {
		hideThisWindow();
		WindowController::instance()->startSelectWindow(handle);
	}
	p->hotkey.clear();
	p->hotkeyTimer.stop();
}

void WindowListDialog::displayHoveredWindowCaption(const QModelIndex &index) {
	int handle = p->view->model()->data(index, WindowListModel::ERole_Handle).toInt();
	p->label->setText(
		WindowController::instance()->windowCaption(handle)
	);
	p->pixmapLabel->setPixmap(
		WindowController::instance()->windowIcon(handle)
	);
}

void WindowListDialog::updateWindowList() {
	WindowController::instance()->updateWindowInfoList();
}

void WindowListDialog::updateWindowPreviews() {
	WindowController::instance()->startUpdateWindowPreviews();
}

void WindowListDialog::delayedUpdateWindowPreview(int handle) {
	if(isVisible()) {
		WindowController::instance()->startWindowPreview(handle);	
	} else {
		p->updateWindowPreviewOnShow.insert(handle);
	}
}

}
