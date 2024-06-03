/*
 * SPDX-FileCopyrightText: 2014 David Edmundson <david@davidedmundson.co.uk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include "notemanager.h"

#include <QDebug>
#include <QMutex>
#include <QQmlEngine>

#include <KLocalizedString>
#include <KMessageBox>

#include "filesystemnoteloader.h"
#include "note.h"

NoteManager::NoteManager(QObject *parent)
    : QObject(parent)
    , m_backend(loadBackend())
{
}

Note *NoteManager::loadNote(const QString &id)
{
    Note *note = m_backend->loadNote(id);
    QQmlEngine::setObjectOwnership(note, QQmlEngine::JavaScriptOwnership);
    return note;
}

void NoteManager::deleteNoteResources(const QString &id)
{
    m_backend->deleteNoteResources(id);
}

void NoteManager::showRemoveConfirmationDialog()
{
    int removeDialog = KMessageBox::questionTwoActions(nullptr,
                                                       i18n("Are you sure you want to discard this note?"),
                                                       i18n("Discard this note?"),
                                                       KStandardGuiItem::del(),
                                                       KStandardGuiItem::cancel(),
                                                       QString(),
                                                       KMessageBox::Dangerous);

    Q_EMIT removeConfirmationDialog(removeDialog == KStandardGuiItem::Delete || removeDialog == KStandardGuiItem::Discard);
}

QSharedPointer<AbstractNoteLoader> NoteManager::loadBackend()
{
    static QMutex mutex;
    static QWeakPointer<AbstractNoteLoader> s_backend;

    mutex.lock();
    QSharedPointer<AbstractNoteLoader> manager = s_backend.toStrongRef();
    if (manager.isNull()) {
        manager.reset(new FileSystemNoteLoader);
        s_backend = manager;
    }
    mutex.unlock();
    return manager;
}
