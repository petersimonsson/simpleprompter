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
#include "groupedcomboboxdelegate.h"

GroupedComboBoxDelegate::GroupedComboBoxDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void GroupedComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString type = index.data( Qt::AccessibleDescriptionRole ).toString();
    QStyleOptionViewItem itemOption = option;

    if ( type == QLatin1String( "parent" ) ) {
        itemOption.state |= QStyle::State_Enabled;
    } else if ( type == QLatin1String( "child" ) ) {
        int indent = option.fontMetrics.width( QString( 4, QChar( ' ' ) ) );
        itemOption.rect.adjust( indent, 0, 0, 0 );
        itemOption.textElideMode = Qt::ElideNone;
    }

    QStyledItemDelegate::paint(painter, itemOption, index);
}
