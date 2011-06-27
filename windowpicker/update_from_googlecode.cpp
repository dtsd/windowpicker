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

#include "windowpicker/update_from_googlecode.h"

#include "windowpicker/config.h"

#include <QtNetwork>

namespace WindowPicker
{

struct UpdateFromGoogleCode::Private
{
	Private() : checkInterval(0), isCheckRunning(false) {};
	int checkInterval;
	QTimer checkTimer;
	bool isCheckRunning;
};

UpdateFromGoogleCode::UpdateFromGoogleCode(QObject *parent) : QObject(parent), p(new Private)
{
	connect(&p->checkTimer, SIGNAL(timeout()), this, SLOT(check()));
}

UpdateFromGoogleCode::~UpdateFromGoogleCode()
{
	delete p;
}

void UpdateFromGoogleCode::apply(const Config &config)
{
	p->checkInterval = config.updateCheckIntervalHours() * 60 * 60;

	if(p->checkInterval > 0)
	{
		start();
	}
	else
	{
		stop();
	}
}

void UpdateFromGoogleCode::check()
{
	if(p->isCheckRunning)
	{
		return;
	}

	p->isCheckRunning = true;

	{
		QNetworkAccessManager manager;
		QNetworkRequest request(
			QUrl(
				QString("http://code.google.com/p/%s/downloads/list")
					.arg(QCoreApplication::applicationName())
			)
		);
		QNetworkReply *reply = manager.get(request);
		do
		{
			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
		}
		while(!reply->isFinished());

		QTextStream stream(reply);
		QString html = stream.readAll();
		reply->close();

		QRegExp re("\\['[^']+', 'Download[^']+', '//(?P<url>[^']+)', '(?P<tags>[^']+)'\\]");
		int pos = 0;
		
		QStringList requiredTags;
#ifdef Q_WS_WIN
		requiredTags << "Type-Executable" << "OpSys-Windows";
#endif

		QString maxVersion;
		while((pos = re.indexIn(html)) != -1)
		{
			QString url = re.cap(1),
				tags = re.cap(2);

			for(QStringList::const_iterator it = requiredTags.begin(); it != requiredTags.end(); ++it)
			{
				if(!tags.contains(*it))
				{
					goto not_found;
				}
			}

			QString version = extractVersionFromUrl(url);

			if(isVersionGreater(maxVersion, version))
			{
				maxVersion = version;
			}

			not_found:
				;
		}

		if(isVersionGreater(QCoreApplication::applicationVersion(), maxVersion))
		{
			emit newVersionAvailable(maxVersion);
		}
	}

	p->isCheckRunning = false;
}

void UpdateFromGoogleCode::start()
{
	check();
	p->checkTimer.start(p->checkInterval);
}

void UpdateFromGoogleCode::stop()
{
	p->checkTimer.stop();
}

QString UpdateFromGoogleCode::extractVersionFromUrl(const QString &url)
{
	QString fileName = QFileInfo(url).fileName();
	QRegExp re("\\d(\\.\\d)+");
	QString result;
	if(re.indexIn(fileName) != -1) 
	{
		result = re.cap();
	}
	return result;
}

bool UpdateFromGoogleCode::isVersionGreater(const QString &one, const QString &two)
{
	QStringList oneList = one.split('.'), twoList = two.split('.');
	for(int i = 0; i < qMin(oneList.size(), twoList.size()); ++i)
	{
		int oneNumber = oneList.at(i).toInt(),
			twoNumber = twoList.at(i).toInt();
		if(twoNumber > oneNumber)
		{
			return true;
		}
		else if(twoNumber < oneNumber)
		{
			return false;
		}
	}

	if(twoList.size() > oneList.size())
	{
		return true;
	}

	return false;
}

}
	/*
	QMap<QString, QString> updateTest;
	updateTest["1.1"] = "1.1";
	updateTest["1.0"] = "1.1";
	updateTest["1.2"] = "1.0";
	updateTest["1.3"] = "1.4";
	updateTest["1.2.30"] = "1.0.40";
	updateTest["1.0.30"] = "1.0";
	updateTest["1.5"] = "1.5.30";
	updateTest["1"] = "0.0.40";
	updateTest["2"] = "2.0.40";
	for(QMap<QString, QString>::const_iterator it = updateTest.begin(); 
		it != updateTest.end(); ++it)
	{
		qDebug("%s < %s = %i",
			qPrintable(it.key()),
			qPrintable(it.value()),
			WindowPicker::UpdateFromGoogleCode::isVersionGreater(it.key(), it.value())
		);
	}

	qDebug(qPrintable(WindowPicker::UpdateFromGoogleCode::extractVersionFromUrl("windowpicker.googlecode.com/files/windowpicker-1.1.exe")));
	*/
