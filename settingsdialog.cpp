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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFontDialog>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setUrl(const QString &url)
{
    ui->urlEdit->setText(url);
}

QString SettingsDialog::url() const
{
    return ui->urlEdit->text();
}

void SettingsDialog::setApiKey(const QString &key)
{
    ui->apiKeyEdit->setText(key);
}

QString SettingsDialog::apiKey() const
{
    return ui->apiKeyEdit->text();
}

void SettingsDialog::setToken(const QString &token)
{
    ui->tokenEdit->setText(token);
}

QString SettingsDialog::token() const
{
    return ui->tokenEdit->text();
}

void SettingsDialog::setScriptViewFont(const QFont &f)
{
    ui->fontCombo->setCurrentFont(f);
    ui->fontSpin->setValue(f.pointSize());
}

QFont SettingsDialog::scriptViewFont() const
{
    QFont newFont = ui->fontCombo->currentFont();
    newFont.setPointSize(ui->fontSpin->value());
    return newFont;
}
