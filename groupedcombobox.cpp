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
#include "groupedcombobox.h"
#include "groupedcomboboxdelegate.h"

#include <QStandardItem>
#include <QStandardItemModel>
#include <QFont>
#include <QDebug>

GroupedComboBox::GroupedComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setItemDelegate(new GroupedComboBoxDelegate(this));
}

void GroupedComboBox::addParentItem(const QString &text)
{
    QStandardItem* item = new QStandardItem(text);
    item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
    item->setData("parent", Qt::AccessibleDescriptionRole);

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);

    QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}

void GroupedComboBox::addChildItem(const QString &text, const QVariant &data)
{
    QStandardItem* item = new QStandardItem(text);
    item->setData(data, Qt::UserRole);
    item->setData("child", Qt::AccessibleDescriptionRole);

    QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}
