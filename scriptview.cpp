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
#include <QRegularExpression>
#include <QDateTime>
#include <QLocale>
#include <QTimeZone>
#include <QTimer>

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

    m_clockType = settings.value("ScriptView/ClockType", Clock12h).toInt();
    m_showSeconds = settings.value("ScriptView/ShowSeconds", false).toBool();

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(1000);
    m_refreshTimer->start();

    connect(m_refreshTimer, &QTimer::timeout, this, [this]{ update(); });
}

ScriptView::~ScriptView()
{
    qDeleteAll(m_pages);
    QSettings settings;
    settings.setValue("ScriptView/Font", font().toString());
    settings.setValue("ScriptView/ClockType", m_clockType);
    settings.setValue("ScriptView/ShowSeconds", m_showSeconds);
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
        emit currentRowChanged(m_pages[m_currentPage]->rowId);
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
        emit currentRowChanged(m_pages[m_currentPage]->rowId);
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
    painter.fillRect(rect(), QBrush(Qt::black));

    if(!m_pages.isEmpty())
    {
        painter.setBrush(QBrush(Qt::white));
        painter.setPen(Qt::white);

        QFontMetrics fm(font());
        QRect drawRect = rect();
        drawRect.setHeight(fm.height());
        painter.fillRect(drawRect, QBrush(Qt::darkGray));
        drawRect.adjust(10, 0, -10, 0);
        QString text = QString::number(m_currentPage + 1) + "/" + QString::number(m_pages.count()) + " " + m_pages[m_currentPage]->title;
        painter.drawText(drawRect, text);

        text = replaceVariables(m_pages[m_currentPage]->body);

        painter.setBackground(QBrush(Qt::black));
        drawRect = rect();
        drawRect.adjust(10, fm.height() + 10, -10, 0);
        painter.drawText(drawRect, text);
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
    qDeleteAll(m_pages);
    m_pages.clear();
    m_rowPageHash.clear();

    foreach(RundownRow *row, m_rundownCreator->rundownRows())
    {
        if (row->script() && !row->script()->script().isEmpty())
        {
            m_rowPageHash.insert(row->rowId(), m_pages.count());
            QString text = resizeVariables(row->script()->script());
            QFontMetrics fm(font());
            QRect drawRect = rect();
            drawRect.adjust(10, fm.height() + 10, -10, 0);
            QRect brect = metrics.boundingRect(drawRect, Qt::AlignLeft | Qt::TextWordWrap, text);

            if (brect.height() <= drawRect.height())
            {
                auto page = new Page;
                page->rowId = row->rowId();
                page->title = row->storySlug().toUpper();
                page->body = text.toUpper();

                m_pages.append(page);
            }
            else
            {
                QStringList remain;
                int prevCount = 0; // Use this to avoid lock ups due to text not fitting at all
                QRegularExpression re("\\S*|\\s");
                QRegularExpressionMatchIterator it = re.globalMatch(text);
                while(it.hasNext())
                {
                    QRegularExpressionMatch match = it.next();
                    if(!match.captured(0).isEmpty())
                        remain.append(match.captured(0));
                }

                while(!remain.isEmpty() && prevCount != remain.count())
                {
                    prevCount = remain.count();
                    text = remain.takeFirst();
                    brect = metrics.boundingRect(drawRect, Qt::AlignLeft | Qt::TextWordWrap, text);
                    int len = 0;

                    while(brect.height() <= drawRect.height() && !remain.isEmpty())
                    {
                        len = remain.first().count();
                        text += remain.takeFirst();
                        brect = metrics.boundingRect(drawRect, Qt::AlignLeft | Qt::TextWordWrap, text);
                    }

                    if (brect.height() > drawRect.height())
                    {
                        remain.prepend (text.right(len));
                        text = text.left (text.count() - len);
                    }

                    auto page = new Page;
                    page->rowId = row->rowId();
                    page->title = row->storySlug().toUpper();
                    page->body = unresizeVariables(text.toUpper());
                    m_pages.append(page);
                }
            }
        }
    }
}

QString
ScriptView::replaceVariables(const QString &text)
{
    QString newString = text;
    QRegularExpression re("\\%(TIME|DATE)(:([^\\%]*))?\\%");
    QRegularExpressionMatchIterator it = re.globalMatch(newString);
    int offset = 0;
    QString insert;
    QString clockFormat = generateClockFormat();

    while(it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QDateTime dt;

        if(match.lastCapturedIndex() == 3)
            dt.setTimeZone(QTimeZone(match.captured(3).toLatin1()));

        dt.setSecsSinceEpoch(QDateTime::currentSecsSinceEpoch());

        if(match.captured(1) == "TIME")
        {
            insert = dt.time().toString(clockFormat);
        }
        else if (match.captured(1) == "DATE")
        {
            insert = QLocale().toString(dt.date(), QLocale::LongFormat).toUpper();
        }

        newString = newString.replace(match.capturedStart(0) + offset, match.capturedLength(0), insert);
        offset += insert.count() - match.capturedLength(0);
    }

    return newString;
}

QString
ScriptView::resizeVariables(const QString &text)
{
    QString newString = text;
    QRegularExpression re("\\%(TIME|DATE)(:([^\\%]*))?\\%");
    QRegularExpressionMatchIterator it = re.globalMatch(newString);
    int offset = 0;
    QString insert;
    QString clockFormat = generateClockFormat();

    while(it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QDateTime dt = QDateTime::currentDateTime();

        if(match.captured(1) == "TIME")
        {
            insert = dt.time().toString(clockFormat);
        }
        else if (match.captured(1) == "DATE")
        {
            insert = QLocale().toString(dt.date(), QLocale::LongFormat).toUpper();
        }

        int coff = insert.count() - match.capturedLength(0);
        insert = match.captured(0);

        if(coff > 0)
        {
            insert += + "<";
            for(int i = 0; i < (coff - 2); ++i)
                insert += "¤";
            insert += ">";
        }

        newString = newString.replace(match.capturedStart(0) + offset, match.capturedLength(0), insert);
        offset += insert.count() - match.capturedLength(0);
    }

    return newString;
}

QString ScriptView::unresizeVariables(const QString &text)
{
    QString newString = text;
    newString = newString.remove(QRegularExpression("<¤*>"));

    return newString;
}

QString ScriptView::generateClockFormat() const
{
    QString format;

    if(m_clockType == Clock24h)
    {
        format = "HH:mm";

        if(m_showSeconds)
            format += ":ss";
    }
    else
    {
        format = "hh:mm";

        if(m_showSeconds)
            format += ":ss";

        format += " AP";
    }


    return format;
}
