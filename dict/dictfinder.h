/*
    SPDX-FileCopyrightText: 2007 Jeff Cooper <weirdsox11@gmail.com>
    SPDX-FileCopyrightText: 2007 Thomas Georgiou <TAGeorgiou@gmail.com>
    SPDX-FileCopyrightText: 2022 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef DICTFINDER_H
#define DICTFINDER_H

#include <QEventLoop>
#include <QObject>
#include <QRunnable>
#include <QTcpSocket>

/**
 * A runnable that finds all available dicts.
 */
class DictFinder : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit DictFinder(const QString &serverName, QObject *parent = nullptr);

    void run() override;

Q_SIGNALS:
    void dictsRecieved(const QMap<QString, QString> &dicts);

private:
    QString m_serverName;
    QTcpSocket m_tcpSocket;

    QMap<QString /* id */, QString /* description */> m_availableDicts;

    // https://datatracker.ietf.org/doc/html/rfc2229
    std::array<QByteArray, 3> m_responses;
};

#endif // DICTFINDER_H
