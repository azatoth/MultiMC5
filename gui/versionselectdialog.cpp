/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "versionselectdialog.h"
#include "ui_versionselectdialog.h"

#include <QHeaderView>

#include <QDebug>

#include <gui/taskdialog.h>

#include <instversionlist.h>
#include <instversion.h>
#include <task.h>

VersionSelectDialog::VersionSelectDialog(InstVersionList *vlist, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VersionSelectDialog)
{
	ui->setupUi(this);
	
	m_vlist = vlist;
	
	m_proxyModel = new QSortFilterProxyModel(this);
	m_proxyModel->setSourceModel(vlist);
	
	ui->listView->setModel(m_proxyModel);
	ui->listView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->listView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	
	connect(ui->filterSnapshotsCheckbox, SIGNAL(clicked()), SLOT(updateFilterState()));
	connect(ui->filterMCNostalgiaCheckbox, SIGNAL(clicked()), SLOT(updateFilterState()));
	
	updateFilterState();
}

VersionSelectDialog::~VersionSelectDialog()
{
	delete ui;
}

int VersionSelectDialog::exec()
{
	QDialog::open();
	if (!m_vlist->isLoaded())
		loadList();
	return QDialog::exec();
}

void VersionSelectDialog::loadList()
{
	TaskDialog *taskDlg = new TaskDialog(this);
	Task *loadTask = m_vlist->getLoadTask();
	loadTask->setParent(taskDlg);
	taskDlg->exec(loadTask);
}

const InstVersion *VersionSelectDialog::selectedVersion() const
{
	const InstVersion *versionPtr = (const InstVersion *)
			m_proxyModel->data(ui->listView->selectionModel()->currentIndex(),
							   InstVersionList::VersionPointerRole).value<void *>();
	
	return versionPtr;
}

void VersionSelectDialog::on_refreshButton_clicked()
{
	loadList();
}

void VersionSelectDialog::updateFilterState()
{
	m_proxyModel->setFilterKeyColumn(InstVersionList::TypeColumn);
	
	QStringList filteredTypes;
	if (!ui->filterSnapshotsCheckbox->isChecked())
		filteredTypes += "Snapshot";
	if (!ui->filterMCNostalgiaCheckbox->isChecked())
		filteredTypes += "MCNostalgia";
	
	QString regexStr = "^.*$";
	if (filteredTypes.length() > 0)
		regexStr = QString("^((?!%1).)*$").arg(filteredTypes.join('|'));
	
	qDebug() << "Filter:" << regexStr;
	
	m_proxyModel->setFilterRegExp(regexStr);
}
