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
#include "windowpicker/config_dialog.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QEvent>
#include <QAbstractTableModel>
#include <QTableView>
#include <QDataStream>

#include "windowpicker/config.h"
#include "windowpicker/window_controller.h"
#include "windowpicker/keysequence_edit.h"
#include "windowpicker/i18n_controller.h"


Q_DECLARE_METATYPE(WindowPicker::WindowInfo)

namespace WindowPicker {

class IgnoredWindowsModel : public QAbstractTableModel
{
public:
	void setIgnoredWindows(const QList<QVariant> &ignoredWindows) 
	{
		beginResetModel();
		m_ignoredWindows = ignoredWindows;
		endResetModel();
	};
	QList<QVariant> ignoredWindows() const 
	{
		return m_ignoredWindows;
	};

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_ignoredWindows.size();
	};
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const
	{
		return 2;
	};
	virtual QVariant data(const QModelIndex & index, 
		int role = Qt::DisplayRole) const
	{
		if(!index.isValid())
		{
			return QVariant();
		}

		switch(role)
		{
			case Qt::DisplayRole:
				{
					QVariant windowVariant = m_ignoredWindows.at(index.row());
					WindowInfo info = windowVariant.value<WindowPicker::WindowInfo>();
					return index.column() == 0 ? info.caption : info.className;
				}
				break;
			default:
				return QVariant();
		}

	};
		/*
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	*/
	QVariant headerData(int /*section*/, Qt::Orientation orientation, int role) const 
	{
		if(orientation == Qt::Horizontal) {
			if(role == Qt::SizeHintRole) {
				return QSize(0, 0);
			}
		} else if (orientation == Qt::Vertical) {
			if(role == Qt::SizeHintRole) {
				return QSize(0, 0);
			}
		}
		return QVariant();
	}

private:
	QList<QVariant> m_ignoredWindows;
};


struct ConfigDialogPrivate {
	ConfigDialogPrivate() :listWidget(0), stackedWidget(0),
		selectWithSingleClickGroup(0), defaultKeySequenceEdit(0),
		replaceDefaultSwitcherBox(0), showHotkeyLabelsBox(0),
		languageCombo(0),
		config(0),
		ok(0), cancel(0),
		keyboardItem(0), mouseItem(0), i18nItem(0),
		compatibilityItem(0), updateItem(0),
		languageLabel(0), defaultHotkeyLabel(0),
		r1(0), r2(0), defaultHitCornerBox(0), defaultHitCornerLabel(0),
		removeIgnoredButton(0), ignoredWindowsBox(0), ignoredWindowsView(0) {};
	QListWidget *listWidget;
	QStackedWidget *stackedWidget;
	QButtonGroup *selectWithSingleClickGroup;
	KeySequenceEdit *defaultKeySequenceEdit;
	QCheckBox *replaceDefaultSwitcherBox, *showHotkeyLabelsBox;
	QComboBox *languageCombo;


	Config *config;

	QPushButton *ok, *cancel;
	QListWidgetItem *keyboardItem, *mouseItem, *i18nItem, *compatibilityItem, *updateItem;
	QLabel *languageLabel, *defaultHotkeyLabel;
	QRadioButton *r1, *r2;
	QComboBox *defaultHitCornerBox;
	QLabel *defaultHitCornerLabel;
	QPushButton *removeIgnoredButton;
	QGroupBox *ignoredWindowsBox;
	IgnoredWindowsModel ignoredWindowsModel;
	QAbstractItemView *ignoredWindowsView;
};

ConfigDialog::ConfigDialog(Config *config, QWidget *parent) 
	:QDialog(parent), p(new ConfigDialogPrivate) 
{
	p->config = config;
	setupUi();
	translateUi();
}

ConfigDialog::~ConfigDialog() {
	delete p;
}

void ConfigDialog::setupUi() {

	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *widgetLayout = new QHBoxLayout, *buttonLayout = new QHBoxLayout;

	layout->addLayout(widgetLayout);
	layout->addLayout(buttonLayout);

	p->listWidget = new QListWidget(this);
    p->listWidget->setViewMode(QListView::IconMode);
    p->listWidget->setIconSize(QSize(32, 32));
    p->listWidget->setMovement(QListView::Static);
    p->listWidget->setFixedWidth(100);
    p->listWidget->setGridSize(QSize(95, 55));
    p->listWidget->setSpacing(12);


	widgetLayout->addWidget(p->listWidget);

	p->stackedWidget = new QStackedWidget(this);
	widgetLayout->addWidget(p->stackedWidget);

	/*
	p->stackedWidget->addWidget(new QLabel("Accessability"));
	*/
	p->stackedWidget->addWidget(createKeyboardWidget());
	p->stackedWidget->addWidget(createMouseWidget());
	p->stackedWidget->addWidget(createComatibilityWidget());
	p->stackedWidget->addWidget(createUpdateWidget());
	p->stackedWidget->addWidget(createI18nWidget());

	createItems(p->listWidget);
    connect(p->listWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, 
			SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
	p->listWidget->setCurrentRow(0);

	buttonLayout->addStretch();
	p->ok = new QPushButton( this);
	connect(p->ok, SIGNAL(clicked()), this, SLOT(accept()));
	buttonLayout->addWidget(p->ok);
	p->ok->setDefault(true);

	p->cancel = new QPushButton(this);
	connect(p->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	buttonLayout->addWidget(p->cancel);

	resize(QSize(400, 400));
}

void ConfigDialog::createItems(QListWidget *listWidget) {
	/*
    QListWidgetItem *accessibilityItem = new QListWidgetItem(listWidget);
	accessibilityItem->setIcon(QIcon(":preferences-desktop-accessibility.big"));
    accessibilityItem->setText(tr("Accessibility"));
    accessibilityItem->setTextAlignment(Qt::AlignHCenter);
    accessibilityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	*/

    p->keyboardItem = new QListWidgetItem(listWidget);
	p->keyboardItem->setIcon(QIcon(":preferences-desktop-keyboard.big"));
    p->keyboardItem->setTextAlignment(Qt::AlignHCenter);
    p->keyboardItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    p->mouseItem = new QListWidgetItem(listWidget);
	p->mouseItem->setIcon(QIcon(":preferences-desktop-mouse.big"));
    p->mouseItem->setTextAlignment(Qt::AlignHCenter);
    p->mouseItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    p->compatibilityItem = new QListWidgetItem(listWidget);
	p->compatibilityItem->setIcon(QIcon(":tools-report-bug.big"));
    p->compatibilityItem->setTextAlignment(Qt::AlignHCenter);
    p->compatibilityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    p->updateItem = new QListWidgetItem(listWidget);
	p->updateItem->setIcon(QIcon(":system-software-update.big"));
    p->updateItem->setTextAlignment(Qt::AlignHCenter);
    p->updateItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    p->i18nItem = new QListWidgetItem(listWidget);
	p->i18nItem->setIcon(QIcon(":applications-education-language.big"));
    p->i18nItem->setTextAlignment(Qt::AlignHCenter);
    p->i18nItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current)
        current = previous;
    p->stackedWidget->setCurrentIndex(p->listWidget->row(current));
}


void ConfigDialog::ignoreWindow(const QString &caption, const QString &className)
{
	QList<QVariant> ignoredWindows = p->ignoredWindowsModel.ignoredWindows();
	WindowInfo info;
	info.caption = caption;
	info.className = className;

	QVariant windowVariant;
	windowVariant.setValue(info);
	ignoredWindows.append(windowVariant);

	p->ignoredWindowsModel.setIgnoredWindows(ignoredWindows);
	p->config->setLater(Config::c_ignoredWindows, ignoredWindows);

	if(!isVisible())
	{
		p->config->commit();
	}
}

void ConfigDialog::removeSelectedIgnoredWindow()
{

	QModelIndex index = p->ignoredWindowsView->selectionModel()->currentIndex();
	if(index.isValid())
	{
		QList<QVariant> ignoredWindows = p->ignoredWindowsModel.ignoredWindows();
		ignoredWindows.removeAt(index.row());
		p->ignoredWindowsModel.setIgnoredWindows(ignoredWindows);
		p->config->setLater(Config::c_ignoredWindows, ignoredWindows);
	}
}

QWidget *ConfigDialog::createComatibilityWidget()
{
	QWidget *w = new QWidget;

	QVBoxLayout *layout = new QVBoxLayout(w);

	p->ignoredWindowsBox = new QGroupBox();

	QVBoxLayout *boxLayout = new QVBoxLayout(p->ignoredWindowsBox);
	QTableView *view = new QTableView;
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setModel(&p->ignoredWindowsModel);
	p->ignoredWindowsView = view;
	QHBoxLayout *buttons = new QHBoxLayout;
	buttons->addStretch();
	p->removeIgnoredButton = new QPushButton();

	connect(p->removeIgnoredButton, SIGNAL(clicked()),
		this, SLOT(removeSelectedIgnoredWindow()));

	buttons->addWidget(p->removeIgnoredButton);
	boxLayout->addWidget(view);
	boxLayout->addLayout(buttons);

	layout->addWidget(p->ignoredWindowsBox);

	layout->addStretch();
	return w;
}

QWidget *ConfigDialog::createUpdateWidget()
{
	QWidget *w = new QWidget;
	return w;
}

QWidget * ConfigDialog::createMouseWidget() {
	QWidget *w = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(w);

	

	p->defaultHitCornerBox = new QComboBox;
	/*
	QStringList i18nFiles = I18nController::instance()->findI18nFiles();
	for(QStringList::const_iterator it = i18nFiles.begin(); it != i18nFiles.end(); ++it) {
		p->languageCombo->addItem(
			I18nController::instance()->languageName(*it),
			QVariant(*it)
		);
	}*/
	p->defaultHitCornerBox->addItem(
		tr("None"),
		0
	);
	p->defaultHitCornerBox->addItem(
		tr("Top right"),
		1	
	);
	p->defaultHitCornerBox->addItem(
		tr("Top left"),
		2	
	);
	p->defaultHitCornerBox->addItem(
		tr("Bottom right"),
		3	
	);
	p->defaultHitCornerBox->addItem(
		tr("Bottom left"),
		4	
	);

	QFormLayout *formLayout = new QFormLayout;
	layout->addLayout(formLayout);
	connect(
		p->defaultHitCornerBox, 
		SIGNAL(currentIndexChanged(int)), 
		this, 
		SLOT(setDefaultHitCorner(int))
	);
	p->defaultHitCornerLabel = new QLabel;
	formLayout->addRow(p->defaultHitCornerLabel, p->defaultHitCornerBox);

	p->r1 = new QRadioButton;
	p->r2 = new QRadioButton;
	p->selectWithSingleClickGroup = new QButtonGroup(this);
	p->selectWithSingleClickGroup->addButton(p->r1, int(false));
	p->selectWithSingleClickGroup->addButton(p->r2, int(true));
	connect(
		p->selectWithSingleClickGroup, 
		SIGNAL(buttonClicked(int)), 
		this, 
		SLOT(setSelectWithSingleClick(int))
	);

	QGroupBox *box = new QGroupBox(w);
	QVBoxLayout *boxLayout = new QVBoxLayout(box);
	boxLayout->addWidget(p->r1);
	boxLayout->addWidget(p->r2);
	boxLayout->addStretch();

	layout->addWidget(box);

	layout->addStretch();
	return w;
}

QWidget *ConfigDialog::createI18nWidget() {
	QWidget *w = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(w);
	QFormLayout *formLayout = new QFormLayout;
	p->languageLabel = new QLabel;
	p->languageCombo = new QComboBox;

	QStringList i18nFiles = I18nController::instance()->findI18nFiles();
	for(QStringList::const_iterator it = i18nFiles.begin(); it != i18nFiles.end(); ++it) {
		p->languageCombo->addItem(
			I18nController::instance()->languageName(*it),
			QVariant(*it)
		);
	}

	connect(
		p->languageCombo, 
		SIGNAL(currentIndexChanged(int)), 
		this, 
		SLOT(setI18nFileByIndex(int))
	);

	layout->addLayout(formLayout);
	formLayout->addRow(p->languageLabel, p->languageCombo);
	return w;
}

QWidget * ConfigDialog::createKeyboardWidget() {
	QWidget *w = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(w);
	QFormLayout *formLayout = new QFormLayout;

	p->defaultKeySequenceEdit = new KeySequenceEdit;
	p->defaultKeySequenceEdit->setFixedWidth(100);
	connect(
		p->defaultKeySequenceEdit, 
		SIGNAL(keySequenceChanged(const QKeySequence &)),
		this,
		SLOT(setDefaultKeySequence(const QKeySequence &))
	);
	p->defaultHotkeyLabel = new QLabel;
	formLayout->addRow(p->defaultHotkeyLabel, p->defaultKeySequenceEdit);
	layout->addLayout(formLayout);

	p->replaceDefaultSwitcherBox = new QCheckBox( w);
	connect(
		p->replaceDefaultSwitcherBox, 
		SIGNAL(toggled(bool)), 
		this, 
		SLOT(setReplaceDefaultSwitcher(bool))
	);
	layout->addWidget(p->replaceDefaultSwitcherBox);

	p->showHotkeyLabelsBox = new QCheckBox(w);
	connect(
		p->showHotkeyLabelsBox, 
		SIGNAL(toggled(bool)), 
		this, 
		SLOT(setShowHotkeyLabels(bool))
	);
	layout->addWidget(p->showHotkeyLabelsBox);

	layout->addStretch();
	return w;
}

void ConfigDialog::setSelectWithSingleClick(int value) {
	p->config->setLater(Config::c_selectWithSingleClick, value);
}

void ConfigDialog::setDefaultKeySequence(const QKeySequence &value) {
	p->config->setLater(Config::c_defaultKeySequence, value);
}

void ConfigDialog::setReplaceDefaultSwitcher(bool value) {
	p->config->setLater(Config::c_replaceDefaultSwitcher, value);
}

void ConfigDialog::setShowHotkeyLabels(bool value) {
	p->config->setLater(Config::c_showHotkeyLabels, value);
}

void ConfigDialog::setI18nFileByIndex(int value) {
	QString i18nFile = p->languageCombo->itemData(value).toString();
	p->config->setLater(Config::c_i18nFile, i18nFile);
}

void ConfigDialog::setDefaultHitCorner(int value) {
	int itemValue = p->defaultHitCornerBox->itemData(value).toInt();
	p->config->setLater(Config::c_defaultHitCorner, itemValue);
}

void ConfigDialog::accept() {
	p->config->commit();
	QDialog::accept();
}

void ConfigDialog::reject() {
	p->config->revert();
	QDialog::reject();
}

void ConfigDialog::showEvent(QShowEvent *e) {
	p->selectWithSingleClickGroup->button(
		int(p->config->selectWithSingleClick())
	)->setChecked(true);
	p->defaultKeySequenceEdit->setKeySequence(
		p->config->defaultKeySequence()
	);
	p->replaceDefaultSwitcherBox->setChecked(
		p->config->replaceDefaultSwitcher()
	);
	p->showHotkeyLabelsBox->setChecked(
		p->config->showHotkeyLabels()
	);

	int index = p->languageCombo->findData(
		p->config->i18nFile()
	);
	p->languageCombo->setCurrentIndex(index);
	
	p->defaultHitCornerBox->setCurrentIndex(p->config->defaultHitCorner());
	p->ignoredWindowsModel.setIgnoredWindows(p->config->ignoredWindows());

	QDialog::showEvent(e);
}

void ConfigDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		translateUi();
	} else {
		QDialog::changeEvent(e);
	}
}

void ConfigDialog::translateUi() {
	setWindowTitle(tr("Options"));
	p->ok->setText(tr("Ok"));
	p->cancel->setText(tr("Cancel"));

    p->keyboardItem->setText(tr("Keyboard"));
    p->mouseItem->setText(tr("Mouse"));
    p->i18nItem->setText(tr("Language"));
    p->compatibilityItem->setText(tr("Compatibility"));
    p->updateItem->setText(tr("Update"));
	p->languageLabel->setText(tr("Language"));

	p->r1->setText(tr("Pick windows using double click"));
	p->r2->setText(tr("Pick windows using single click"));

	p->defaultHotkeyLabel->setText(tr("Show windowpicker hotkey"));
	p->replaceDefaultSwitcherBox->setText(tr("Replace default Alt+Tab window switcher"));
	p->showHotkeyLabelsBox->setText(tr("Enable window hotkeys"));

	p->defaultHitCornerLabel->setText(tr("Show windowpicker hot corner"));

	p->ignoredWindowsBox->setTitle(tr("Ignored windows"));
	p->removeIgnoredButton->setText(tr("Remove"));
}

}
