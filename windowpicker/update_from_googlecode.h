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

#if !defined WINDOWPICKER_UPDATE_FROM_GOOGLE_CODE_H
#define WINDOWPICKER_UPDATE_FROM_GOOGLE_CODE_H

#include <QtCore>

namespace WindowPicker
{

class Config;

class UpdateFromGoogleCode  : public QObject
{
public:
	UpdateFromGoogleCode(QObject *);
	virtual ~UpdateFromGoogleCode();

	static bool isVersionGreater(const QString &, const QString &);
	static QString extractVersionFromUrl(const QString &);
public slots:
	void apply(const Config &);
	void check();

signals:
	void newVersionAvailable(const QString &);
private:
	void start();
	void stop();

	struct Private;
	Private *p;
};

}

#endif
