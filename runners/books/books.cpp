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
#include <QXmlStreamReader>
#include <QDomDocument>

Books::Books(QObject *parent, const QVariantList& args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args);
    setObjectName(QLatin1String("Books"));
    setIgnoredTypes(Plasma::RunnerContext::FileSystem | Plasma::RunnerContext::Directory | Plasma::RunnerContext::NetworkLocation);

    Plasma::RunnerSyntax s(QLatin1String( ":q:" ), i18n("Uses Goodreads to find books matching :q:."));
    s.addExampleQuery(QLatin1String("book :q:"));
    addSyntax(s);

    addSyntax(Plasma::RunnerSyntax(QLatin1String( "image" ), i18n("Lists the books matching the query, using Goodreads search")));
    setSpeed(SlowSpeed);
    setPriority(LowPriority);
}

Books::~Books()
{
}

void Books::match(Plasma::RunnerContext &context)
{
    kDebug() << "Books Runner, MATCH MADE";

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

    parseXML(booksJob.data());//, context);
}

void Books::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    KRun *opener = new KRun(match.data().toString(), 0);
    opener->setRunExecutables(false);
}

void Books::parseXML(QByteArray data)
{
//    QXmlStreamReader xml(data);
//    
//    if (xml.hasError()) {
//        kError() << "Books Runner xml parse failure";
//        return;
//    }
//    
//    while (!xml.atEnd()) {
//        QXmlStreamReader::TokenType token = xml.readNext();
//        
//        if (token == QXmlStreamReader::StartDocument) {
//            continue;
//        }
//        
//        if (token == QXmlStreamReader::StartElement) {
//            kDebug() << "NAME!" << xml.name() ;
//           // if (xml.name() == "results") {
//                parseBook(xml);
//          //      return;
//         //   }
//                xml.readNext();
//        }
//    }


    QXmlStreamReader xml(data);

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

            kDebug() << "TEXT!" << xml.name();
 /////       xml.readNext();
    }
}

void Books::parseBook(QXmlStreamReader& xml)
{
    QStringRef name = xml.name();
    QString currentElement;
    
    QStringList videoTitles;
    QStringList videoLinks;
    
    kDebug() << "NAME: " << name;
    
    QXmlStreamAttributes attributes = xml.attributes();
    while (!xml.atEnd()) {// && xml.tokenType() != QXmlStreamReader::EndDocument) {
        kDebug() << "WHILE LOOP(((((((((((((((((()))))))))))))))))), name: " << xml.name();
        kDebug() << "ELEMTN: " << xml.readElementText();
        kDebug() << "CURRENTELEMENT: " << currentElement;

        if (xml.name() == "title") {
            kDebug() << attributes.value("plain").toString();
        }

        if (xml.name() == "thumbnail") {
            QStringRef attribute = attributes.value("url");
            kDebug() << "ATTRIBUTE: " << attribute;
        }
        //            if (name == "title") {
            //                kDebug() << "GOT TITLE: " << name;
            //                videoTitles.append(xml.readElementText());
            //
            //            } else if (name == "link") {
                //
                //                if (xml.attributes().value("rel").toString() == "alternate") {
                    //                    kDebug() << "ATTRIBUTES: " << xml.attributes().value("href");
                    //                    const QString& link = xml.attributes().value("href").toString();
                    //                    if (link != "http://www.youtube.com") {
                        //                        videoLinks.append(link);
                        //                    }
                        //                }
                        //            }
                        
                        xml.readNext();
                        attributes = xml.attributes();
                        currentElement = xml.readElementText();
                        kDebug() << currentElement;
        }
        
        if (!videoTitles.isEmpty() && !videoLinks.isEmpty()) {
            kDebug() << "TITLE WAS: " << videoTitles;
            kDebug() << "LINK WAS: " << videoLinks;
        }
    }

#include "books.moc"
