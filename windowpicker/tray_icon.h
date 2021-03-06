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
#if !defined(WINDOWPICKER_TRAY_ICON_H)
#define WINDOWPICKER_TRAY_ICON_H

#include <QSystemTrayIcon>

class QObject;

namespace WindowPicker {

class TrayIcon : public QSystemTrayIcon {
Q_OBJECT
public:
	TrayIcon(QObject *parent = 0);
	~TrayIcon();
signals:
	void windowPickerRequested();
	void configDialogRequested();
public slots:
	void translateUi();
protected slots:
	void action(QSystemTrayIcon::ActivationReason);
	void emitWindowPickerRequested();
	void emitConfigDialogRequested();
	void showAboutDialog();
private:
	TrayIcon(const TrayIcon &) : QSystemTrayIcon() {};
	TrayIcon & operator=(const TrayIcon &) { return *this; };

	void setupActions();
	void setupUi();
	void setupMenu();

	struct Private;
	Private *p;
};

}

#endif
