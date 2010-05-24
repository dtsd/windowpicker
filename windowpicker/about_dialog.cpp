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

#include "windowpicker/about_dialog.h" 

#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>

namespace WindowPicker {

struct AboutDialogPrivate {
	AboutDialogPrivate() :close(0), aboutText(0) {};
	QPushButton *close;
	QTextBrowser *aboutText;
};

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), p(new AboutDialogPrivate) {
	setupUi();
	translateUi();
}

AboutDialog::~AboutDialog() {
	delete p;
}

QString AboutDialog::getVersionString() const {
	return tr("Version") + QString(" %1").arg(QCoreApplication::applicationVersion());
}

void AboutDialog::setupUi() {
	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *aboutLabel = new QLabel;
	aboutLabel->setText(
		QString("<img src=\":preferences-system-windows.big\">"
			"<br><font size=\"16\"><b>%1</b></font>"
			"<br><br>%2")
				.arg(QCoreApplication::applicationName())
				.arg(getVersionString())

			/*+ QString("<br><br>&copy; 2010 %1 <a href=\"mailto:%2\">%3</a>")
				.arg(QCoreApplication::organizationName())
				.arg(QCoreApplication::organizationDomain())
				.arg(QCoreApplication::organizationDomain())*/
	);
	aboutLabel->setAlignment(Qt::AlignCenter);
	aboutLabel->setOpenExternalLinks(true);
	p->aboutText = new QTextBrowser;

	 p->aboutText->append("AUTHORS");
     QFile authorsFile(":AUTHORS"), copyingFile(":COPYING");

     if (authorsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		 p->aboutText->append(authorsFile.readAll());
	 }

	 p->aboutText->append("COPYING");
     if (copyingFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		 p->aboutText->append(copyingFile.readAll());
	 }

	p->aboutText->setReadOnly(true);
	p->aboutText->setOpenExternalLinks(true);
	layout->addWidget(aboutLabel);
	layout->addWidget(p->aboutText);
	//layout->addStretch();


	p->close = new QPushButton;
	connect(
		p->close,
		SIGNAL(clicked()),
		this,
		SLOT(accept())
	);
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(p->close);
	layout->addLayout(buttonLayout);

	resize(QSize(500, 400));
}

void AboutDialog::showEvent(QShowEvent *e) {
	p->aboutText->verticalScrollBar()->setValue(0);
	QDialog::showEvent(e);
}

void AboutDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		translateUi();
	} else {
		QDialog::changeEvent(e);
	}
}

void AboutDialog::translateUi() {
	setWindowTitle(tr("About"));
	p->close->setText(tr("Close"));
}

}
