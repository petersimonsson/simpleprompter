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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->fontButton, &QToolButton::clicked,
            this, &SettingsDialog::showFontDialog);
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

void SettingsDialog::setScriptViewFont(const QFont &font)
{
    m_scriptViewFont = font;

    QFontInfo info(m_scriptViewFont);
    ui->fontLabel->setText(tr("%1, %2pt").arg(info.family()).arg(info.pointSize()));
    ui->fontLabel->setFont(m_scriptViewFont);
}

QFont SettingsDialog::scriptViewFont() const
{
    return m_scriptViewFont;
}

void SettingsDialog::showFontDialog()
{
    QFontDialog *dialog = new QFontDialog(m_scriptViewFont, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog, &QFontDialog::fontSelected,
            this, &SettingsDialog::setScriptViewFont);

    dialog->show();
}
