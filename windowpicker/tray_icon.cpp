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
#include "windowpicker/tray_icon.h"

#include "windowpicker/window_list_dialog.h"
#include "windowpicker/window_controller.h"
#include "windowpicker/about_dialog.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPoint>

namespace WindowPicker {

struct TrayIcon::Private {
	Private() :quitAction(0), windowPickerAction(0), configurationAction(0) {};
	QAction *quitAction, *windowPickerAction, *configurationAction,
		*aboutDialogAction;
};

TrayIcon::TrayIcon(QObject *parent) : QSystemTrayIcon(parent), p(new Private) {
	setupActions();
	setupUi();
	setupMenu();

	translateUi();
}

TrayIcon::~TrayIcon() {
	delete p;
}

void TrayIcon::setupActions() {
	p->quitAction = new QAction( this);
	connect(p->quitAction, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));
	p->windowPickerAction = new QAction( this);
	connect(
		p->windowPickerAction, 
		SIGNAL(activated()), 
		this, 
		SLOT(emitWindowPickerRequested())
	);
	p->configurationAction = new QAction(this);
	connect(
		p->configurationAction,
		SIGNAL(activated()),
		this,
		SLOT(emitConfigDialogRequested())
	);
	p->aboutDialogAction = new QAction(this);
	connect(
		p->aboutDialogAction,
		SIGNAL(activated()),
		this,
		SLOT(showAboutDialog())
	);
}

void TrayIcon::setupMenu() {
	QMenu *cm = new QMenu;
	cm->addAction(p->windowPickerAction);
	cm->addAction(p->configurationAction);
	cm->addAction(p->aboutDialogAction);
	cm->addSeparator();
	cm->addAction(p->quitAction);
	setContextMenu(cm);
}

void TrayIcon::setupUi() {
	setIcon(QIcon(":preferences-system-windows"));

	connect(
		this, 
		SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this,
		SLOT(action(QSystemTrayIcon::ActivationReason))
	);
}

void TrayIcon::action(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
		emitWindowPickerRequested();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
/*	case QSystemTrayIcon::Context:
		contextMenu()->popup(QPoint());*/
    default:
        ;
    }
}

void TrayIcon::emitWindowPickerRequested() {
	emit windowPickerRequested();
}

void TrayIcon::emitConfigDialogRequested() {
	emit configDialogRequested();
}

void TrayIcon::translateUi() {
	p->quitAction->setText(tr("&Quit"));
	p->windowPickerAction->setText(tr("&Show WindowPicker"));
	p->configurationAction->setText(tr("&Options"));
	p->aboutDialogAction->setText(tr("&About"));
}

void TrayIcon::showAboutDialog() {
	AboutDialog dlg;
	dlg.exec();
}

}
