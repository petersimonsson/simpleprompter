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

#ifndef RUNDOWNFOLDER_H
#define RUNDOWNFOLDER_H

#include <QString>
#include <QList>

class Rundown;

class RundownFolder
{
public:
    RundownFolder(qint32 id, const QString &name);
    ~RundownFolder();

    qint32 id() const { return m_id; }
    QString name() const { return m_name; }

    void addRundown(Rundown *rundown);
    QList<Rundown*> rundowns() const { return m_rundowns; }

private:
    qint32 m_id;
    QString m_name;

    QList<Rundown*> m_rundowns;
};

#endif // RUNDOWNFOLDER_H
