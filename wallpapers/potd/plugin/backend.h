/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QUrl>

class QFileDialog;

class Backend : public QObject
{
    Q_OBJECT

public:
    enum class FileOperationStatus {
        None,
        Succeeded,
        Failed,
    };
    Q_ENUM(FileOperationStatus)

    /**
     * Returns the result of the file operation.
     */
    Q_PROPERTY(FileOperationStatus saveStatus MEMBER m_saveStatus NOTIFY saveStatusChanged)

    /**
     * Returns the status message after a save operation.
     */
    Q_PROPERTY(QString saveStatusMessage MEMBER m_saveStatusMessage CONSTANT)

    /**
     * Returns the folder path of the saved image file.
     */
    Q_PROPERTY(QUrl savedFolder MEMBER m_savedFolder CONSTANT)

    /**
     * Returns the path of the saved image file.
     */
    Q_PROPERTY(QUrl savedUrl MEMBER m_savedUrl CONSTANT)

    explicit Backend(QObject *parent = nullptr);
    ~Backend() override;

    /**
     * Opens a Save dialog to choose the save location, and copies the source file to the
     * selected destination.
     *
     * @param sourceUrl the URL of the file to be saved
     * @param defaultName the default file name, defined in WallpaperPreview.qml
     * @return the path of the saved image file
     */
    Q_INVOKABLE QUrl saveImage(const QUrl &sourceUrl, const QString &defaultName = QString());

Q_SIGNALS:
    void saveStatusChanged();

private:
    /**
     * Sets the status of the file operation.
     *
     * @param the status of the file operation to be set
     */
    void setSaveStatus(FileOperationStatus status);

    /**
     * Checks and replaces all invalid characters with '-' in the file name
     *
     * @param name the original file name
     * @return the sanitized file name
     * @see qt-creator/src/libs/utils/filenamevalidatinglineedit.cpp
     */
    QString sanitizeFileName(const QString &name) const;

    QUrl m_savedFolder;
    QUrl m_savedUrl;
    FileOperationStatus m_saveStatus;
    QString m_saveStatusMessage;
};

#endif
