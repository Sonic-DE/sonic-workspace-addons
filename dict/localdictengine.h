/*
    SPDX-FileCopyrightText: 2025 Anirudh Vempati <rudy.vempati@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once

#include <QObject>


class LocalDictEngine : public QObject
{
    Q_OBJECT

public:
    LocalDictEngine(QObject *parent = nullptr);
    ~LocalDictEngine() override;

Q_SIGNALS:
    void definitionRecieved(const QStringList &matches);

public Q_SLOTS:
    void requestDefinition(const QString &query);

private:
    QString m_dictPath;
    const std::array<QString, 4> m_defTypes;
};
