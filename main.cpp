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
#include <QApplication>
#include <QDesktopWidget>
#include <QKeySequence>

#include <QtSingleApplication>

#include "windowpicker/window_list_dialog.h"
#include "windowpicker/tray_icon.h"
#include "windowpicker/window_controller.h"
#include "windowpicker/i18n_controller.h"
#include "windowpicker/shortcut_controller.h"
#include "windowpicker/config.h"
#include "windowpicker/config_dialog.h"
#include "windowpicker/update_from_googlecode.h"

int main(int argc, char * argv[])
{


	QtSingleApplication app(argc, argv);
	if(app.isRunning()) {
		app.sendMessage(QString::null);
		return 0;
	}


	QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
	QCoreApplication::setApplicationName(APPLICATION_NAME);
	QCoreApplication::setApplicationVersion(QString("%1.%2.%3")
		.arg(APPLICATION_VERSION_MAJOR)
		.arg(APPLICATION_VERSION_MINOR)
		.arg(APPLICATION_VERSION_PATCH));

    QApplication::setQuitOnLastWindowClosed(false);

	WindowPicker::Config config;
	WindowPicker::WindowController::instance()->apply(config);
	WindowPicker::I18nController::instance()->apply(config);
	WindowPicker::WindowListDialog dlg;
	dlg.apply(config);

	WindowPicker::UpdateFromGoogleCode update;

	QObject::connect(
		&update,
		SIGNAL(newVersionAvailable(const QString &, const QString &)),
		&dlg,
		SLOT(notifyAboutNewVersion(const QString &, const QString &))
	);
	update.apply(config);

	WindowPicker::TrayIcon icon;
	WindowPicker::ConfigDialog configDialog(&config);
	QObject::connect(
		WindowPicker::I18nController::instance(),
		SIGNAL(languageChanged()),
		&icon,
		SLOT(translateUi())
	);

	QObject::connect(
		WindowPicker::WindowController::instance(),
		SIGNAL(windowIgnoreRequested(const QString &, const QString &)),
		&configDialog,
		SLOT(ignoreWindow(const QString &, const QString &))
	);

	QObject::connect(
		&app,
		SIGNAL(messageReceived(const QString &)),
		&dlg,
		SLOT(showThisWindow())
	);

	QObject::connect(
		&config,
		SIGNAL(changed(const Config&)),
		&dlg,
		SLOT(apply(const Config&))
	);

	QObject::connect(
		&config,
		SIGNAL(changed(const Config&)),
		WindowPicker::WindowController::instance(),
		SLOT(apply(const Config&))
	);

	QObject::connect(
		&config,
		SIGNAL(changed(const Config&)),
		WindowPicker::I18nController::instance(),
		SLOT(apply(const Config&))
	);

	QObject::connect(
		&icon, 
		SIGNAL(windowPickerRequested()),
		&dlg, 
		SLOT(showThisWindow())
	);

	QObject::connect(
		&icon, 
		SIGNAL(configDialogRequested()),
		&configDialog, 
		SLOT(show())
	);

	icon.show();
	int ret = app.exec();

	return ret;
}
