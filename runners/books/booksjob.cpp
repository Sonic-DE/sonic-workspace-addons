/******************************************************************************
 *  Copyright (C) 2012 by Shaun Reich <shaun.reich@blue-systems.com>          *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as published  *
 *  by the Free Software Foundation; either version 2 of the License or (at   *
 *  your option) any later version.                                           *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Library General Public License for more details.                          *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this library; see the file COPYING.LIB.                        *
 *  If not, see <http://www.gnu.org/licenses/>.                               *
 *****************************************************************************/

#include "booksjob.h"

#include <KDebug>
#include <KToolInvocation>

#include <QtCore/qurl.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

BooksJob::BooksJob(const QString& term)
  : QObject()
  , m_manager(0)
{
    kDebug() << "%%%%%% TubeJob ctor hit! QUERY TERM: " + term;

    m_manager = new QNetworkAccessManager(this);

    QUrl url = QUrl("http://api.bing.net/json.aspx?AppId=340D9148BE10A564ABFC17937FFB623836112FBB&Query=" + term + "&Sources=Image&Version=2.0&Image.Count=10&Image.Offset=0");
    QNetworkRequest request = QNetworkRequest(url);

    m_manager->get(request);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(jobCompleted(QNetworkReply*)));
}

void BooksJob::jobCompleted(QNetworkReply* reply)
{
    m_data = reply->readAll();
    kDebug() << "JOBCOMPLETED";

    emit finished();
}

QByteArray BooksJob::data()
{
    return m_data;
}


#include "booksjob.moc"
