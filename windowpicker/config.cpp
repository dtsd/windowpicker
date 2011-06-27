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
#include "windowpicker/config.h"

#include <QMap>
#include <QLocale>

namespace WindowPicker {

Key Config::c_selectWithSingleClick = "selectWithSingleClick";
Key Config::c_defaultKeySequence = "defaultKeySequence";
Key Config::c_replaceDefaultSwitcher = "replaceDefaultSwitcher";
Key Config::c_i18nFile = "i18nFile";
Key Config::c_showHotkeyLabels = "showHotkeyLabels";
Key Config::c_defaultHitCorner = "defaultHitCorner";
Key Config::c_ignoredWindows = "ignoredWindows";

struct ConfigPrivate {
	ConfigPrivate() :isSetLater(false) {};
	QSettings settings;
	QMap<QString, QVariant> defaults;
	QMap<QString, QVariant> later;
	bool isSetLater;
};

Config::Config() :QObject(), p(new ConfigPrivate) {
	setupDefaults();
}

Config::~Config() {
	delete p;
}


void Config::setupDefaults() {
	p->defaults[c_selectWithSingleClick] = true;
	p->defaults[c_defaultKeySequence] = QKeySequence(Qt::Key_E + Qt::CTRL + Qt::ALT);
	p->defaults[c_replaceDefaultSwitcher] = true;
	p->defaults[c_i18nFile] = QLocale::system().name();
	p->defaults[c_showHotkeyLabels] = true;
	p->defaults[c_defaultHitCorner] = 0;

	p->defaults[c_ignoredWindows] = QList<QVariant>();
}

QVariant Config::defaultValue(const QString &key) const {
	Q_ASSERT(p->defaults.contains(key));
	return p->defaults[key];
}

void Config::emitChanged() {
	if(!p->isSetLater) {
		emit changed(*this);
	}
}

void Config::setLater(const QString &key, const QVariant &value) {
	p->later[key] = value;
	p->isSetLater = true;
}

void Config::commit() {
	for(QMap<QString, QVariant>::const_iterator it = p->later.begin(); 
		it != p->later.end(); ++it) 
	{
		this->setProperty(it.key().toLatin1(), it.value());
	}

	p->later.clear();
	p->isSetLater = false;

	emitChanged();
}

void Config::revert() {
	p->later.clear();
	p->isSetLater = false;
}

bool Config::selectWithSingleClick() const {
	return p->settings.value(
		c_selectWithSingleClick, defaultValue(c_selectWithSingleClick)
	).toBool();
}

void Config::setSelectWithSingleClick(bool value) {
	p->settings.setValue(c_selectWithSingleClick, value);
	emitChanged();
}

QKeySequence Config::defaultKeySequence() const {
	return QKeySequence(p->settings.value(
		c_defaultKeySequence, defaultValue(c_defaultKeySequence)
	).toString());
}

void Config::setDefaultKeySequence(const QKeySequence &value) {
	p->settings.setValue(c_defaultKeySequence, value.toString());
	emitChanged();
}

bool Config::replaceDefaultSwitcher() const {
	return p->settings.value(
		c_replaceDefaultSwitcher, defaultValue(c_replaceDefaultSwitcher)
	).toBool();
}


void Config::setReplaceDefaultSwitcher(bool value) {
	p->settings.setValue(c_replaceDefaultSwitcher, value);
	emitChanged();
}

QString Config::i18nFile() const {
	return p->settings.value(
		c_i18nFile, defaultValue(c_i18nFile)
	).toString();
}


void Config::setI18nFile(const QString &value) {
	p->settings.setValue(c_i18nFile, value);
	emitChanged();
}

bool Config::showHotkeyLabels() const {
	return p->settings.value(
		c_showHotkeyLabels, defaultValue(c_showHotkeyLabels)
	).toBool();
}


void Config::setShowHotkeyLabels(bool value) {
	p->settings.setValue(c_showHotkeyLabels, value);
	emitChanged();
}

int Config::defaultHitCorner() const {
	return p->settings.value(
		c_defaultHitCorner, defaultValue(c_defaultHitCorner)
	).toInt();
}


void Config::setDefaultHitCorner(int value) {
	p->settings.setValue(c_defaultHitCorner, value);
	emitChanged();
}

QList<QVariant> Config::ignoredWindows() const {
	return p->settings.value(
		c_ignoredWindows, defaultValue(c_ignoredWindows)
	).toList();
}


void Config::setIgnoredWindows(const QList<QVariant> &value) {
	p->settings.setValue(c_ignoredWindows, value);
	emitChanged();
}

}

