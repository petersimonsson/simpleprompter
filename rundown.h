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

#ifndef RUNDOWN_H
#define RUNDOWN_H

#include <QString>

class Rundown
{
public:
    Rundown(qint32 id, const QString &title);

    qint32 id() const { return m_id; }
    QString title() const { return m_title; }

private:
    qint32 m_id;
    QString m_title;
};

#endif // RUNDOWN_H
