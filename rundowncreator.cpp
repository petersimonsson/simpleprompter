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

#include "rundowncreator.h"
#include "rundownfolder.h"
#include "rundown.h"
#include "rundownrow.h"
#include "script.h"

#include <QSettings>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

RundownCreator::RundownCreator(QObject *parent) : QObject(parent)
{
    QSettings settings;
    m_url = settings.value("RundownCreator/Url").toString();
    m_apiKey = settings.value("RundownCreator/ApiKey").toString();
    m_apiToken = settings.value("RundownCreator/Token").toString();

    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, &QNetworkAccessManager::finished,
            this, &RundownCreator::handleFinished);
}

RundownCreator::~RundownCreator()
{
    QSettings settings;
    settings.setValue("RundownCreator/Url", m_url);
    settings.setValue("RundownCreator/ApiKey", m_apiKey);
    settings.setValue("RundownCreator/Token", m_apiToken);

    deleteFolders();
    deleteRows();
}

bool RundownCreator::isValid() const
{
    return !m_url.isEmpty() && !m_apiKey.isEmpty() && !m_apiToken.isEmpty();
}

void RundownCreator::getFoldersAndRundowns()
{
    if(!isValid())
        return;

    sendRequest("getFolders");
}

void RundownCreator::getRows(qint32 rundownId)
{
    if(!isValid())
        return;

    QList<QueryItem> parameters;
    parameters.append(QueryItem("RundownID", QString::number(rundownId)));
    parameters.append(QueryItem("GetScript", "true"));
    parameters.append(QueryItem("ScriptHasContent", "true"));
    parameters.append(QueryItem("RemoveCarets", "true"));

    sendRequest("getRows", parameters);
}

void RundownCreator::handleFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    switch(statusCode)
    {
    case 200:
    {
        QString query = reply->url().query();
        QRegExp regexp("Action\\=([a-zA-Z]*)");

        if(regexp.indexIn(query) >= 0)
        {
            QString action = regexp.cap(1);

            if(action == "getFolders")
            {
                handleFolders(data);
            }
            else if(action == "getRundowns")
            {
                handleRundowns(data);
            }
            else if(action == "getRows")
            {
                handleRows(data);
            }
        }
        break;
    }
    }
}

void RundownCreator::sendRequest(const QString &action, const QList<QueryItem> &parameters)
{
    QList<QueryItem> queryItems;
    queryItems.append(QueryItem("APIKey", m_apiKey));
    queryItems.append(QueryItem("APIToken", m_apiToken));
    queryItems.append(QueryItem("Action", action));
    queryItems.append(parameters);

    QUrlQuery query;
    query.setQueryItems(queryItems);

    QUrl url = QUrl::fromUserInput(m_url);
    url.setQuery(query);

    QNetworkRequest request(url);
    m_netManager->get(request);
}

void RundownCreator::deleteFolders()
{
    qDeleteAll(m_rundownFolders);
    m_rundownFolders.clear();
}

void RundownCreator::handleFolders(const QByteArray &data)
{
    deleteFolders();

    QJsonDocument rowsDocument = QJsonDocument::fromJson(data);
    QJsonArray array = rowsDocument.array();

    foreach(const QJsonValue &value, array)
    {
        QJsonObject object = value.toObject();
        RundownFolder *folder = new RundownFolder(object.value("FolderID").toInt(),
                                                  object.value("Name").toString());

        m_rundownFolders.insert(folder->id(), folder);
    }

    getRundowns();
}

void RundownCreator::getRundowns()
{
    QList<QueryItem> parameters;
    parameters.append(QueryItem("Template", "false"));
    parameters.append(QueryItem("Archived", "false"));
    parameters.append(QueryItem("GetDeletedRundowns", "false"));

    sendRequest("getRundowns", parameters);
}

void RundownCreator::handleRundowns(const QByteArray &data)
{
    QJsonDocument rowsDocument = QJsonDocument::fromJson(data);
    QJsonArray array = rowsDocument.array();

    foreach(const QJsonValue &value, array)
    {
        QJsonObject object = value.toObject();
        RundownFolder *folder = m_rundownFolders.value(object.value("FolderID").toInt());

        if(folder)
        {
            Rundown *rundown = new Rundown(object.value("RundownID").toInt(),
                                           object.value("Title").toString());
            folder->addRundown(rundown);
        }
    }

    emit rundownsReceived();
}

void RundownCreator::deleteRows()
{
    qDeleteAll(m_rundownRows);
    m_rundownRows.clear();
}

void RundownCreator::handleRows(const QByteArray &data)
{
    deleteRows();

    QJsonDocument rowsDocument = QJsonDocument::fromJson(data);
    QJsonArray array = rowsDocument.array();

    foreach(const QJsonValue &value, array)
    {
        QJsonObject object = value.toObject();
        RundownRow *row = new RundownRow(object.value("RundownID").toInt(),
                                         object.value("RowID").toInt());
        row->setPageNumber(object.value("PageNumber").toString().trimmed());
        row->setStorySlug(object.value("StorySlug").toString().trimmed());

        QJsonObject scriptObject = object.value("Script").toArray().first().toObject();

        Script *script = new Script(scriptObject.value("ScriptID").toInt());
        script->setScript(scriptObject.value("Script").toString().trimmed());
        row->setScript(script);

        m_rundownRows.append(row);
    }

    emit rowsReceived();
}
