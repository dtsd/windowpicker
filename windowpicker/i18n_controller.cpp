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
#include "windowpicker/i18n_controller.h"

#include <QCoreApplication>
#include <QDir>
#include <QTranslator>

#include "windowpicker/config.h"

namespace WindowPicker {

struct I18nControllerPrivate {
	QTranslator translator;
};

I18nController::I18nController() : p(new I18nControllerPrivate)  {
	init();
}

void I18nController::init() {
     QCoreApplication::instance()->installTranslator(&p->translator);
}

I18nController::~I18nController() {
	delete p;
}

I18nController *I18nController::instance() {
	static I18nController thisInstance;
	return &thisInstance;
}

QStringList I18nController::findI18nFiles() {
	QDir dir(":/i18n");
	QStringList fileNames = dir.entryList(
		QStringList("*.qm"), 
		QDir::Files,
		QDir::Name
	);
	QMutableStringListIterator i(fileNames);
	while (i.hasNext()) {
		i.next();
		i.setValue(dir.filePath(i.value()));
	}
	return fileNames;
}

void I18nController::setI18nFile(const QString &value) {
     p->translator.load(value);
	 emit languageChanged();
}

QString I18nController::languageName(const QString &file) {
	QTranslator translator;
	translator.load(file);
	return translator.translate("LanguageName", "English");
}

void I18nController::apply(const Config &config) {
	setI18nFile(config.i18nFile());
}

}
