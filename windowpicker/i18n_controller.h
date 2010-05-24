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
#if !defined(WINDOWPICKER_I18N_CONTROLLER_H)
#define WINDOWPICKER_I18N_CONTROLLER_H 

#include <QObject>

class QStringList;

namespace WindowPicker {

class Config;

struct I18nControllerPrivate;

class I18nController : public QObject {
Q_OBJECT
public:
	static I18nController *instance();

	QStringList findI18nFiles();
	QString languageName(const QString &i18nFile);
	void setI18nFile(const QString &);
public slots:
	void apply(const Config &);
signals:
	void languageChanged();
private:
	I18nController();
	~I18nController();
	I18nController(const I18nController &) {};
	I18nController& operator=(const I18nController &) { return *this; };

	void init();

	I18nControllerPrivate *p;
};

}

#endif
