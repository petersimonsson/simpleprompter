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

#ifndef RUNDOWNCREATOR_H
#define RUNDOWNCREATOR_H

#include <QObject>
#include <QHash>
#include <QList>

class QNetworkAccessManager;
class QNetworkReply;
class RundownFolder;
class RundownRow;

class RundownCreator : public QObject
{
    Q_OBJECT
public:
    explicit RundownCreator(QObject *parent = nullptr);
    ~RundownCreator();

    void setUrl(const QString &url) { m_url = url; }
    QString url() const { return m_url; }
    void setApiKey(const QString &key) { m_apiKey = key; }
    QString apiKey() const { return m_apiKey; }
    void setApiToken(const QString &token) { m_apiToken = token; }
    QString apiToken() const { return m_apiToken; }

    bool isValid() const;

    QHash<qint32, RundownFolder*> rundownFolders() const { return m_rundownFolders; }
    QList<RundownRow*> rundownRows() const { return m_rundownRows; }

public slots:
    void getFoldersAndRundowns();
    void getRows(qint32 rundownId);

private slots:
    void handleFinished(QNetworkReply *reply);

private:
    typedef QPair<QString, QString> QueryItem;

    void sendRequest(const QString &action, const QList<QueryItem> &parameters = QList<QueryItem>());

    void deleteFolders();
    void handleFolders(const QByteArray &data);

    void getRundowns();
    void handleRundowns(const QByteArray &data);

    void deleteRows();
    void handleRows(const QByteArray &data);

    QString m_url;
    QString m_apiKey;
    QString m_apiToken;

    QNetworkAccessManager *m_netManager;

    QHash<qint32, RundownFolder*> m_rundownFolders;
    QList<RundownRow*> m_rundownRows;

signals:
    void rundownsReceived();
    void rowsReceived();
};

#endif // RUNDOWNCREATOR_H
