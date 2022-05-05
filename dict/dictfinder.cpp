/*
    SPDX-FileCopyrightText: 2007 Jeff Cooper <weirdsox11@gmail.com>
    SPDX-FileCopyrightText: 2007 Thomas Georgiou <TAGeorgiou@gmail.com>
    SPDX-FileCopyrightText: 2022 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "dictfinder.h"

DictFinder::DictFinder(const QString &serverName, [[maybe_unused]] QObject *parent)
    : m_serverName(serverName)
    , m_responses{
          QByteArrayLiteral("250"), /**< ok (optional timing information here) */
          QByteArrayLiteral("420"), /**< Server temporarily unavailable */
          QByteArrayLiteral("421"), /**< Server shutting down at operator request */
      }
{
}

void DictFinder::run()
{
    QEventLoop loop;

    loop.connect(&m_tcpSocket, &QTcpSocket::disconnected, &loop, &QEventLoop::quit);
    loop.connect(&m_tcpSocket, &QTcpSocket::errorOccurred, this, [this, &loop](QAbstractSocket::SocketError error) {
        Q_EMIT errorOccurred(error, m_tcpSocket.errorString());
        loop.quit();
    });
    loop.connect(&m_tcpSocket, &QTcpSocket::readyRead, this, [this, &loop] {
        m_tcpSocket.readAll();
        QByteArray ret;

        m_tcpSocket.write(QByteArrayLiteral("SHOW DB\n"));
        m_tcpSocket.flush();

        if (m_tcpSocket.waitForReadyRead()) {
            while (std::none_of(m_responses.cbegin(),
                                m_responses.cend(),
                                [&ret](const QByteArray &code) {
                                    return ret.contains(code);
                                })
                   && m_tcpSocket.waitForReadyRead()) {
                ret += m_tcpSocket.readAll();
            }
        }

        const QList<QByteArray> retLines = ret.split('\n');

        for (const QByteArray &curr : retLines) {
            if (curr.endsWith("420") || curr.startsWith("421")) {
                // TODO: what happens if the server is down
            }
            if (curr.startsWith("554")) {
                // TODO: What happens if no DB available?
                // TODO: Eventually there will be functionality to change the server...
                break;
            }

            // ignore status code and empty lines
            if (curr.startsWith("250") || curr.startsWith("110") || curr.isEmpty()) {
                continue;
            }

            if (!curr.startsWith('-') && !curr.startsWith('.')) {
                const QString line = QString::fromUtf8(curr).trimmed();
                const QString id = line.section(' ', 0, 0);
                QString description = line.section(' ', 1);
                if (description.startsWith('"') && description.endsWith('"')) {
                    description.remove(0, 1);
                    description.chop(1);
                }
                m_availableDicts.insert(id, description);
            }
        }

        m_tcpSocket.disconnectFromHost();
        loop.quit();
    });

    m_tcpSocket.connectToHost(m_serverName, 2628);
    loop.exec();

    Q_EMIT dictsRecieved(m_availableDicts);
}
