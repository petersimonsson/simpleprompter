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

#ifndef SCRIPTVIEW_H
#define SCRIPTVIEW_H

#include <QWidget>

class RundownCreator;

class ScriptView : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptView(QWidget *parent = nullptr);

    void setRundownCreator(RundownCreator *creator) { m_rundownCreator = creator; reset (); }

    void reset();

public slots:
    void forward();
    void back();
    void gotoRow(qint32 row);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void createPages();

    RundownCreator *m_rundownCreator;

    int m_currentPage;
    QStringList m_pages;
    QHash<qint32, int> m_rowPageHash;
};

#endif // SCRIPTVIEW_H
