/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "commandrunner.h"

#include <QProcess>

#include <Sonic/Logging/Log>

bool CommandRunner::run(const QString &command)
{
    if (command.trimmed().isEmpty()) {
        return false;
    }
    if (!QProcess::startDetached(QStringLiteral("/bin/sh"), {QStringLiteral("-c"), command})) {
        sonicWarning() << "cannot start the menu command" << command;
        return false;
    }
    return true;
}

#include "moc_commandrunner.cpp"
