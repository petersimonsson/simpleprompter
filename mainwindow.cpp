/**************************************************************************
  SimplePrompter
  Copyright (C) 2018  Peter Simonsson <peter.simonsson@gmail.com>

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
**************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "rundowncreator.h"
#include "rundownfolder.h"
#include "rundown.h"
#include "rundownrow.h"
#include "script.h"

#include <QSettings>
#include <QScopedPointer>
#include <QCollator>
#include <QAction>
#include <QShortcut>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->settingsButton, &QToolButton::clicked,
            this, &MainWindow::showSettingsDialog);
    connect(ui->rundownCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::getRundownRows);
    connect(ui->sectionCombo, QOverload<int>::of(&QComboBox::activated),
            this, &MainWindow::gotoRow);

    m_rundownCreator = new RundownCreator(this);

    connect(m_rundownCreator, &RundownCreator::rundownsReceived,
            this, &MainWindow::updateRundowns);
    connect(m_rundownCreator, &RundownCreator::rowsReceived,
            this, &MainWindow::updateRows);
    connect(ui->reloadButton, &QToolButton::clicked,
            m_rundownCreator, &RundownCreator::getFoldersAndRundowns);

    ui->scriptView->setRundownCreator(m_rundownCreator);
    (void) new QShortcut(Qt::Key_Left, ui->scriptView, SLOT(back()), SLOT(back()), Qt::ApplicationShortcut);
    (void) new QShortcut(Qt::Key_Right, ui->scriptView, SLOT(forward()), SLOT(forward()), Qt::ApplicationShortcut);

    m_rundownCreator->getFoldersAndRundowns();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showSettingsDialog()
{
    QSettings settings;
    QScopedPointer<SettingsDialog> dialog(new SettingsDialog(this));

    dialog->setUrl(m_rundownCreator->url());
    dialog->setApiKey(m_rundownCreator->apiKey());
    dialog->setToken(m_rundownCreator->apiToken());

    if(dialog->exec() == QDialog::Accepted)
    {
        m_rundownCreator->setUrl(dialog->url());
        m_rundownCreator->setApiKey(dialog->apiKey());
        m_rundownCreator->setApiToken(dialog->token());

        m_rundownCreator->getFoldersAndRundowns();
    }
}

void MainWindow::updateRundowns()
{
    QString oldSelection = ui->rundownCombo->currentText();
    bool block = ui->rundownCombo->blockSignals(true);
    ui->rundownCombo->clear();
    QMap<QString, qint32> sortMap;
    QHash<qint32, RundownFolder*> folderHash = m_rundownCreator->rundownFolders();
    QString firstRundown;

    foreach(RundownFolder *folder, folderHash)
    {
        sortMap.insert(folder->name(), folder->id());
    }

    foreach(qint32 id, sortMap)
    {
        RundownFolder *folder = folderHash.value(id);
        ui->rundownCombo->addParentItem(folder->name());
        QList<Rundown*> rundowns = folder->rundowns();
        QCollator collator;
        collator.setNumericMode(true);

        std::sort(rundowns.begin(), rundowns.end(), [&collator](Rundown *r1, Rundown *r2) {
            return collator.compare(r1->title(), r2->title()) > 0;
        });

        foreach(Rundown *rundown, rundowns)
        {
            ui->rundownCombo->addChildItem(rundown->title(), rundown->id());

            if(firstRundown.isEmpty())
                firstRundown = rundown->title();
        }
    }

    ui->rundownCombo->blockSignals(block);

    if(ui->rundownCombo->findText(oldSelection) != -1)
    {
        ui->rundownCombo->setCurrentText(oldSelection);
    }
    else
    {
        ui->rundownCombo->setCurrentText(firstRundown);
    }
}

void MainWindow::getRundownRows(int index)
{
    if(ui->rundownCombo->count() > 0)
    {
        m_rundownCreator->getRows(ui->rundownCombo->itemData(index).toInt());
    }
}

void MainWindow::updateRows()
{
    ui->sectionCombo->clear();

    foreach(RundownRow *row, m_rundownCreator->rundownRows())
    {
        if(row->script() && !row->script()->script().isEmpty())
            ui->sectionCombo->addItem(row->storySlug(), row->rowId());
    }

    ui->scriptView->reset();
}

void MainWindow::gotoRow(int index)
{
    ui->scriptView->gotoRow(ui->sectionCombo->itemData(index).toInt());
}
