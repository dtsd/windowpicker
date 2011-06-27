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
#if !defined(WINDOWPICKER_CONFIG_DIALOG_H)
#define WINDOWPICKER_CONFIG_DIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QShowEvent;
class QEvent;

namespace WindowPicker {

class Config;

struct ConfigDialogPrivate;

class ConfigDialog : public QDialog {
	Q_OBJECT
public:
	ConfigDialog(Config *, QWidget *parent = 0);
	virtual ~ConfigDialog();
public slots:
	virtual void accept();
	virtual void reject();

	void ignoreWindow(const QString &, const QString &);
protected slots:
	void changePage(QListWidgetItem*, QListWidgetItem*);
	void setSelectWithSingleClick(int);
	void setDefaultKeySequence(const QKeySequence &);
	void setReplaceDefaultSwitcher(bool);
	void setI18nFileByIndex(int);
	void setShowHotkeyLabels(bool);
	void setDefaultHitCorner(int);
	void removeSelectedIgnoredWindow();
protected:
	void showEvent(QShowEvent *);
	void changeEvent(QEvent *);
private:
	ConfigDialog(const ConfigDialog&) {};
	ConfigDialog & operator=(const ConfigDialog &) { return *this; };

	void setupUi();
	void translateUi();
	QWidget *createMouseWidget();
	QWidget *createKeyboardWidget();
	QWidget *createI18nWidget();
	QWidget *createComatibilityWidget();
	QWidget *createUpdateWidget();
	void createItems(QListWidget *);

	ConfigDialogPrivate *p;
};

}

#endif
