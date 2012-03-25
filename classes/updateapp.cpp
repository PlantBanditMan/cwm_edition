/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


#include "updateapp.h"

UpdateApp::UpdateApp(QObject *parent) :
    QObject(parent)
{
    this->updateMan = new QNetworkAccessManager(this);
    this->reply = NULL;

    connect(this->updateMan, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotWWW(QNetworkReply*)));
}

//This function is updated for my build, keep the original updateapp.cpp for the master QtAdb branch
void UpdateApp::gotWWW(QNetworkReply * pReply)
{
    if (pReply->error() == QNetworkReply::NoError)
    {
        int start, end, startNew, endNew;
        QString newVersion, oldVersion, myVersion, changes;
        QByteArray data = pReply->readAll();
        start = data.indexOf("<p>Latest version is:");
        start+=21;
        end = data.indexOf("_cwm_edition", start);

        newVersion = data.mid(start, end - start);
        myVersion = QCoreApplication::applicationVersion();
        int i = myVersion.indexOf("_");
        oldVersion = myVersion.left(i);
        QStringList newVersionList, oldVersionList;
        newVersionList = newVersion.split(".");
        oldVersionList = oldVersion.split(".");
        startNew = data.indexOf("<p>What's new:");
        startNew+=3;
        endNew = data.indexOf(";</p>", start);
        changes = data.mid(startNew, endNew+1 - startNew);
        if (newVersionList[0].toInt() < oldVersionList[0].toInt())
        {
            emit this->updateState(false, oldVersion, newVersion, changes);
            return;
        }
        if (newVersionList[1].toInt() < oldVersionList[1].toInt())
        {
            emit this->updateState(false, oldVersion, newVersion, changes);
            return;
        }
        if (newVersionList[2].toInt() < oldVersionList[2].toInt())
        {
            emit this->updateState(false, oldVersion, newVersion, changes);
            return;
        }
        if ((newVersionList[0].toInt() == oldVersionList[0].toInt()) && (newVersionList[1].toInt() == oldVersionList[1].toInt()) && (newVersionList[2].toInt() == oldVersionList[2].toInt()))
            emit this->updateState(false, oldVersion, newVersion, changes);
        else
            emit this->updateState(true, oldVersion, newVersion, changes);
    }
    else
    {
        emit this->updateState(false, "failed", "failed", "");
    }
}

void UpdateApp::checkUpdates()
{
    this->reply = this->updateMan->get(QNetworkRequest(QUrl("http://dl.dropbox.com/u/4141888/version.html")));
}
