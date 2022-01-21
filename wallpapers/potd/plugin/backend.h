/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class QUrl;

class Backend : public QObject
{
    Q_OBJECT

public:
    explicit Backend(QObject *parent = nullptr);

    /**
     * Copies the file from @p sourceUrl to @p destUrl.
     *
     * @param sourceUrl the URL of the file to be copied
     * @param destUrl the URL of the file to be copied to
     * @since 5.25
     */
    Q_INVOKABLE void copy(const QUrl &sourceUrl, const QUrl &destUrl);
};

#endif
