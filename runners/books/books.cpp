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

#include "books.h"
#include "booksjob.h"
#include "imageiconengine.h"

#include <KDebug>
#include <KToolInvocation>
#include <KRun>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QTimer>
#include <QtCore/QWaitCondition>
#include <QtCore/QEventLoop>
#include <QtCore/QMap>
#include <qjson/parser.h>

Books::Books(QObject *parent, const QVariantList& args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args);
    setObjectName(QLatin1String("Bing"));
    setIgnoredTypes(Plasma::RunnerContext::FileSystem | Plasma::RunnerContext::Directory | Plasma::RunnerContext::NetworkLocation);

    Plasma::RunnerSyntax s(QLatin1String( ":q:" ), i18n("Finds Bing images search matching :q:."));
    s.addExampleQuery(QLatin1String("image :q:"));
    addSyntax(s);

    addSyntax(Plasma::RunnerSyntax(QLatin1String( "image" ), i18n("Lists the search entries matching the query, using Bing search")));
    setSpeed(SlowSpeed);
    setPriority(LowPriority);
}

Books::~Books()
{
}

void Books::match(Plasma::RunnerContext &context)
{
    kDebug() << "Bing Runner, MATCH MADE";

    const QString term = context.query();

    if (term.length() < 3) {
        return;
    }

    if (!context.isValid()) {
        return;
    }

    QEventLoop loop;

    BooksJob booksJob(term);
    connect(&booksJob, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    parseJson(booksjob.data(), context);
}

void Books::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    KRun *opener = new KRun(match.data().toString(), 0);
    opener->setRunExecutables(false);
}

void Books::parseJson(const QByteArray& data, Plasma::RunnerContext &context)
{
    kDebug() << "XML PARSER ONLINE" << data;
}

#include "books.moc"
