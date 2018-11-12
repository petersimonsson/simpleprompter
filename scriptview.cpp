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

#include "scriptview.h"
#include "rundowncreator.h"
#include "rundownrow.h"
#include "script.h"

#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include <QSettings>

ScriptView::ScriptView(QWidget *parent) :
    QWidget(parent), m_rundownCreator(nullptr), m_currentPage(0)
{
    QSettings settings;
#ifdef Q_OS_DARWIN
    QFont newFont("Helvetica Neue", 20);
#else
    QFont newFont = font();
#endif
    newFont.fromString(settings.value("ScriptView/Font", newFont.toString()).toString());
    setFont(newFont);
}

ScriptView::~ScriptView()
{
    QSettings settings;
    settings.setValue("ScriptView/Font", font().toString());
}

void ScriptView::reset()
{
    m_currentPage = 0;
    createPages();
    repaint();
}

void ScriptView::forward()
{
    if(!m_pages.isEmpty() && m_currentPage < (m_pages.count() - 1))
    {
        ++m_currentPage;
        repaint();
    }
    else if (m_currentPage >= m_pages.count())
    {
        m_currentPage = 0;
        repaint();
    }
}

void ScriptView::back()
{
    if(!m_pages.isEmpty() && m_currentPage > 0)
    {
        --m_currentPage;
        repaint();
    }
    else if (m_currentPage >= m_pages.count())
    {
        m_currentPage = 0;
        repaint();
    }
}

void ScriptView::gotoRow(qint32 row)
{
    if(m_rowPageHash.contains(row))
    {
        m_currentPage = m_rowPageHash.value(row);
        repaint();
    }
}

void ScriptView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    if(!m_pages.isEmpty())
    {
        QString text = m_pages[m_currentPage];
        painter.drawText(rect(), text);
    }
}

void ScriptView::resizeEvent(QResizeEvent *event)
{
    reset();
    QWidget::resizeEvent(event);
}

void ScriptView::createPages()
{
    QFontMetrics metrics(font());
    m_pages.clear();
    m_rowPageHash.clear();

    foreach(RundownRow *row, m_rundownCreator->rundownRows())
    {
        if (row->script() && !row->script()->script().isEmpty())
        {
            m_rowPageHash.insert(row->rowId(), m_pages.count());
            QString text = "==>" + row->storySlug() + "<==\n\n" + row->script()->script();
            QRect brect = metrics.boundingRect(rect(), Qt::AlignLeft | Qt::TextWordWrap, text);

            if (brect.height() <= rect().height())
            {
                m_pages.append(text);
            }
            else
            {
                QStringList remain = text.split(" ");
                int prevCount = 0; // Use this to avoid lock ups due to text not fitting at all

                while(!remain.isEmpty() && prevCount != remain.count())
                {
                    prevCount = remain.count();
                    text = remain.takeFirst();
                    brect = metrics.boundingRect(rect(), Qt::AlignLeft | Qt::TextWordWrap, text);

                    while(brect.height() <= rect().height() && !remain.isEmpty())
                    {
                        text += " " + remain.takeFirst();
                        brect = metrics.boundingRect(rect(), Qt::AlignLeft | Qt::TextWordWrap, text);
                    }

                    if (brect.height() > rect().height())
                    {
                        int index = text.lastIndexOf(' ');
                        remain.prepend (text.mid(index + 1));
                        text = text.left (index);
                    }

                    m_pages.append(text);
                }
            }
        }
    }
}
