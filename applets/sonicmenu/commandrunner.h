/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QQmlEngine>

/*!
 * Runs the shell commands of Sonic Menu's items. Replaces the Plasma5Support
 * "executable" data engine kMenu used: like that engine, a command goes
 * through /bin/sh, so pipes, quoting and variables keep working.
 */
class CommandRunner : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    //! Starts \a command detached from the shell; false if it could not be started
    Q_INVOKABLE static bool run(const QString &command);
};
