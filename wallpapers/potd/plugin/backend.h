/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class QFileDialog;
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
     * Opens a Save dialog to choose the save location, and copies the source file to the
     * selected destination.
     *
     * @param sourceUrl the URL of the file to be saved
     * @param defaultName the default file name, defined in WallpaperPreview.qml
     * @return QString the path of the saved image
     * @since 5.25
     */
    Q_INVOKABLE QString saveImage(const QUrl &sourceUrl, const QString &defaultName = QString());

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
    /**
     * Sets the status of the file operation.
     *
     * @param the status of the file operation to be set
     * @since 5.25
     */
    void setSaveStatus(FileOperationStatus status);

    /**
     * Checks and replaces all invalid characters with '-' in the file name
     *
     * @param name the original file name
     * @return the sanitized file name
     * @see qt-creator/src/libs/utils/filenamevalidatinglineedit.cpp
     * @since 5.25
     */
    QString sanitizeFileName(const QString &name) const;

    FileOperationStatus m_saveStatus;
};

#endif
