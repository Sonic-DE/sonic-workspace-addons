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
    enum FileOperationStatus {
        None,
        Succeeded,
        Failed,
    };
    Q_ENUM(FileOperationStatus)

    explicit Backend(QObject *parent = nullptr);

    Q_PROPERTY(FileOperationStatus saveStatus READ saveStatus NOTIFY saveStatusChanged)

    /**
     * Copies the file from @p sourceUrl to @p destUrl.
     *
     * @param sourceUrl the URL of the file to be copied
     * @param destUrl the URL of the file to be copied to
     * @since 5.25
     */
    Q_INVOKABLE void copy(const QUrl &sourceUrl, const QUrl &destUrl);

    /**
     * Returns the result of the file operation.
     *
     * @return FileOperationStatus::None if there is no result, FileOperationStatus::Succeeded
     *         if the file operation succeeds, otherwise FileOperationStatus::Failed.
     * @since 5.25
     */
    FileOperationStatus saveStatus() const;

Q_SIGNALS:
    void saveStatusChanged();

private:
    FileOperationStatus m_saveStatus;
};

#endif
