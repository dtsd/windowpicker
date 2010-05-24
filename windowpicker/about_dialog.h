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
#if !defined (WINDOWPICKER_ABOUT_DIALOG_H)
#define WINDOWPICKER_ABOUT_DIALOG_H

#include <QDialog>

class QEvent;
class QShowEvent;

namespace WindowPicker {

struct AboutDialogPrivate;
class AboutDialog: public QDialog {
	Q_OBJECT
public:
	AboutDialog(QWidget *parent = 0);
	virtual ~AboutDialog();
protected:
	void showEvent(QShowEvent *);
	void changeEvent(QEvent *);
private:
	AboutDialog(const AboutDialog &) {};
	AboutDialog& operator=(const AboutDialog &) { return *this; };
	
	void setupUi();
	void translateUi();

	QString getVersionString() const;

	AboutDialogPrivate *p;
};

}

#endif
