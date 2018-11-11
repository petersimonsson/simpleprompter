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
#ifndef GROUPEDCOMBOBOX_H
#define GROUPEDCOMBOBOX_H

#include <QComboBox>

class GroupedComboBox : public QComboBox
{
public:
    GroupedComboBox(QWidget *parent = nullptr);

    void addParentItem(const QString &text);
    void addChildItem(const QString &text, const QVariant &data);
};

#endif // GROUPEDCOMBOBOX_H
