/*****************************************************************************
 * PokerTH - The open source texas holdem engine                             *
 * Copyright (C) 2006-2011 Felix Hammer, Florian Thauer, Lothar May          *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/

#include "logfiledialog.h"
#include "ui_logfiledialog.h"
#include <QtCore>
#include "guilog.h"
#include "configfile.h"
#include "mymessagebox.h"

LogFileDialog::LogFileDialog(QWidget *parent, ConfigFile *c) :
QDialog(parent), myConfig(c),
ui(new Ui::LogFileDialog)
{
	ui->setupUi(this);

	connect( ui->pushButton_deleteLog, SIGNAL(clicked()), this, SLOT (deleteLogFile()));
	connect( ui->pushButton_exportLogHtml, SIGNAL(clicked()), this, SLOT (exportLogToHtml()));
	connect( ui->pushButton_exportLogTxt, SIGNAL(clicked()), this, SLOT (exportLogToTxt()));
	connect( ui->pushButton_saveLogAs, SIGNAL(clicked()), this, SLOT (saveLogFileAs()));
	connect( ui->treeWidget_logFiles, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(showLogFilePreview()));
}

LogFileDialog::~LogFileDialog()
{
	delete ui;
}

void LogFileDialog::exec()
{
	refreshLogFileList();
	QDialog::exec();
}

void LogFileDialog::refreshLogFileList()
{
	QDir logFileDir;
	logFileDir.setPath(QString::fromUtf8(myConfig->readConfigString("LogDir").c_str()));
	QStringList filters;
	filters << "*.pdb";
	QFileInfoList dbFilesList = logFileDir.entryInfoList(filters, QDir::Files, QDir::Time);

	QFileInfo currentSqliteLogFile(QString::fromStdString(myGuiLog->getMySqliteLogFileName()));

	ui->treeWidget_logFiles->clear();
	int i;
	for (i=0; i < dbFilesList.size(); i++) {

		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setText(0, dbFilesList.at(i).fileName());
		item->setData(0, Qt::UserRole, dbFilesList.at(i).absoluteFilePath());
		if(currentSqliteLogFile.fileName() == dbFilesList.at(i).fileName()) {
			item->setData(0, Qt::BackgroundColorRole, QColor(Qt::red));
			item->setData(0, Qt::TextColorRole, QColor(Qt::white));
			item->setData(0, Qt::UserRole+1, "current");
		}
		ui->treeWidget_logFiles->addTopLevelItem(item);
	}
	ui->treeWidget_logFiles->sortItems(0, Qt::DescendingOrder);
}

void LogFileDialog::deleteLogFile()
{
	QList<QTreeWidgetItem*> selectedItemsList = ui->treeWidget_logFiles->selectedItems();

	if(!selectedItemsList.isEmpty() && !( selectedItemsList.size() == 1 && selectedItemsList.front()->data(0, Qt::UserRole+1).toString() == "current" )) {

		int ret = MyMessageBox::warning(this, tr("PokerTH - Delete log files"),
									   tr("Do you really want to delete the selected log files?"),
									   QMessageBox::Yes | QMessageBox::No);

		if(ret == QMessageBox::Yes) {
			for (int i = 0; i < selectedItemsList.size(); ++i) {
				if(selectedItemsList.at(i)->data(0, Qt::UserRole+1).toString() != "current") {

					if(!QFile::remove(selectedItemsList.at(i)->data(0, Qt::UserRole).toString())) {
						MyMessageBox::warning(this, "Remove log file", "PokerTH cannot remove this log file, please verify that you have write access to this file!", QMessageBox::Close );
					}
				}
			}
			refreshLogFileList();
		}
	}
}

void LogFileDialog::exportLogToHtml()
{
	QTreeWidgetItem* selectedItem = ui->treeWidget_logFiles->currentItem();

	if(selectedItem) {
		QFileInfo fi(selectedItem->text(0));
		QString fileName = QFileDialog::getSaveFileName(this, tr("Export PokerTH log file to HTML"),
						   QDir::homePath()+"/"+fi.baseName()+".html",
						   tr("PokerTH HTML log (*.html)"));

		if(!fileName.isEmpty()) {
			myGuiLog->exportLogPdbToHtml(selectedItem->data(0, Qt::UserRole).toString(),fileName);
		}
	}
}

void LogFileDialog::exportLogToTxt()
{
	QTreeWidgetItem* selectedItem = ui->treeWidget_logFiles->currentItem();

	if(selectedItem) {
		QFileInfo fi(selectedItem->text(0));
		QString fileName = QFileDialog::getSaveFileName(this, tr("Export PokerTH log file to plain text"),
						   QDir::homePath()+"/"+fi.baseName()+".txt",
						   tr("PokerTH plain text log (*.txt)"));

		if(!fileName.isEmpty()) {
			myGuiLog->exportLogPdbToTxt(selectedItem->data(0, Qt::UserRole).toString(),fileName);
		}
	}
}

void LogFileDialog::saveLogFileAs()
{
	QTreeWidgetItem* selectedItem = ui->treeWidget_logFiles->currentItem();

	if(selectedItem) {
		QFileInfo fi(selectedItem->text(0));
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save PokerTH log file"),
						   QDir::homePath()+"/"+fi.baseName()+".pdb",
						   tr("PokerTH SQL log (*.pdb)"));

		if(!fileName.isEmpty()) {
			QFile::copy(selectedItem->data(0, Qt::UserRole).toString(), fileName);
		}
	}
}

void LogFileDialog::showLogFilePreview()
{
	QTreeWidgetItem* selectedItem = ui->treeWidget_logFiles->currentItem();

	if(selectedItem) {
		myGuiLog->showLog(selectedItem->data(0, Qt::UserRole).toString(), ui->textBrowser_logPreview);
	}

	QTextCursor cursor(ui->textBrowser_logPreview->textCursor());
	cursor.movePosition(QTextCursor::Start);
	ui->textBrowser_logPreview->setTextCursor(cursor);
}

void LogFileDialog::keyPressEvent ( QKeyEvent * event )
{
	if (event->key() == Qt::Key_Delete ) { /*Delete*/
		if(ui->treeWidget_logFiles->hasFocus()) {
			ui->pushButton_deleteLog->click();
		}
	}
}
