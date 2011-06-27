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
#if !defined(WINDOWPICKER_CONFIG_H)
#define WINDOWPICKER_CONFIG_H

#include <QSettings>
#include <QKeySequence>

namespace WindowPicker {

struct ConfigPrivate;

typedef const char *const Key;

class Config: public QObject {
	Q_OBJECT

	Q_PROPERTY ( bool selectWithSingleClick 
		READ selectWithSingleClick WRITE setSelectWithSingleClick
	)

	Q_PROPERTY (QKeySequence defaultKeySequence
		READ defaultKeySequence WRITE setDefaultKeySequence
	)

	Q_PROPERTY (bool replaceDefaultSwitcher
		READ replaceDefaultSwitcher WRITE setReplaceDefaultSwitcher
	)

	Q_PROPERTY (QString i18nFile
		READ i18nFile WRITE setI18nFile
	)

	Q_PROPERTY (bool showHotkeyLabels
		READ showHotkeyLabels WRITE setShowHotkeyLabels
	)

	Q_PROPERTY (int defaultHitCorner
		READ defaultHitCorner WRITE setDefaultHitCorner
	)

	Q_PROPERTY (QList<QVariant> ignoredWindows
		READ ignoredWindows WRITE setIgnoredWindows
	)
public:
	Config();
	virtual ~Config();
	void setLater(const QString &key, const QVariant &value);
	void commit();
	void revert();
	QVariant defaultValue(const QString &key) const;

	QString i18nFile() const;
	void setI18nFile(const QString &);
	static Key c_i18nFile;

	bool selectWithSingleClick() const;	
	void setSelectWithSingleClick(bool);
	static Key c_selectWithSingleClick;

	QKeySequence defaultKeySequence() const;
	void setDefaultKeySequence(const QKeySequence &);
	static Key c_defaultKeySequence;

	bool replaceDefaultSwitcher() const;
	void setReplaceDefaultSwitcher(bool);
	static Key c_replaceDefaultSwitcher;

	bool showHotkeyLabels() const;	
	void setShowHotkeyLabels(bool);
	static Key c_showHotkeyLabels;

	int defaultHitCorner() const;	
	void setDefaultHitCorner(int);
	static Key c_defaultHitCorner;

	QList<QVariant> ignoredWindows() const;
	void setIgnoredWindows(const QList<QVariant> &);
	static Key c_ignoredWindows;
signals:
	void changed(const Config &);
private:
	Config(const Config &) {};
	Config & operator=(const Config&) { return *this; };

	void setupDefaults();
	void emitChanged();

	ConfigPrivate *p;
};

}

#endif

