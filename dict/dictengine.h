/*
    SPDX-FileCopyrightText: 2007 Jeff Cooper <weirdsox11@gmail.com>
    SPDX-FileCopyrightText: 2007 Thomas Georgiou <TAGeorgiou@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once
#include <QCache>
#include <QMap>
#include <QVariantMap>
#include <qobject.h>
class QTcpSocket;

/**
 * This class evaluates the basic expressions given in the interface.
 */

class DictEngine : public QObject
{
    Q_OBJECT

public:
    DictEngine(QObject *parent = nullptr);
    ~DictEngine() override;

Q_SIGNALS:
    void dictsRecieved(const QMap<QString, QString> &dicts);
    void definitionRecieved(const QString &query, const QString &html);
public Q_SLOTS:
    void requestDicts();
    void requestDefinition(const QString &query);

private:
    void getDefinition();
    void socketClosed();
    void getDicts();
    void setDict(const QString &dict);
    void setServer(const QString &server);

    QHash<QString, QString> m_dictNameToDictCode;
    QTcpSocket *m_tcpSocket;
    QString m_currentWord;
    QString m_currentQuery;
    QString m_dictName;
    QString m_serverName;
};
