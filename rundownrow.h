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

#ifndef RUNDOWNROW_H
#define RUNDOWNROW_H

#include <QString>

class Script;

class RundownRow
{
public:
    RundownRow(qint32 rundownId, qint32 rowId);
    ~RundownRow();

    qint32 rundownId() const { return m_rundownId; }
    qint32 rowId() const { return m_rowId; }

    void setPageNumber(const QString &pn) { m_pageNumber = pn; }
    QString pageNumber() const { return m_pageNumber; }

    void setStorySlug(const QString &slug) { m_storySlug = slug; }
    QString storySlug() const { return m_storySlug; }

    void setScript(Script *script) { m_script = script; }
    Script *script() const { return m_script; }

private:
    qint32 m_rundownId;
    qint32 m_rowId;

    QString m_pageNumber;
    QString m_storySlug;

    Script *m_script;
};

#endif // RUNDOWNROW_H
