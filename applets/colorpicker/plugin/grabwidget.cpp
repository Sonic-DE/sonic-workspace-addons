/*
 * SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "grabwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusServiceWatcher>

Q_DECLARE_METATYPE(QColor)

QDBusArgument &operator<<(QDBusArgument &argument, const QColor &color)
{
    argument.beginStructure();
    argument << color.rgba();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QColor &color)
{
    argument.beginStructure();
    QRgb rgba;
    argument >> rgba;
    argument.endStructure();
    color = QColor::fromRgba(rgba);
    return argument;
}

GrabWidget::GrabWidget(QObject *parent)
    : QObject(parent)
    , m_serviceWatcher(new QDBusServiceWatcher(QStringLiteral("org.kde.KWin"),
                                               QDBusConnection::sessionBus(),
                                               QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration,
                                               this))
{
    qDBusRegisterMetaType<QColor>();

    initCompositorWatcher();
}

QColor GrabWidget::currentColor() const
{
    return m_currentColor;
}

void GrabWidget::setCurrentColor(const QColor &color)
{
    if (m_currentColor == color) {
        return;
    }
    m_currentColor = color;

    Q_EMIT currentColorChanged();
}

bool GrabWidget::isCompositingActive() const
{
    return m_isCompositingActive;
}

void GrabWidget::pick()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                      QStringLiteral("/ColorPicker"),
                                                      QStringLiteral("org.kde.kwin.ColorPicker"),
                                                      QStringLiteral("pick"));
    auto call = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QColor> reply = *watcher;
        if (!reply.isError()) {
            setCurrentColor(reply.value());
        }
    });
}

void GrabWidget::copyToClipboard(const QString &text)
{
    QApplication::clipboard()->setText(text);
}

void GrabWidget::slotPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &)
{
    if (interfaceName != QLatin1String("org.kde.kwin.Compositing")) {
        return;
    }

    const auto it = changedProperties.constFind(QStringLiteral("active"));
    if (it == changedProperties.cend()) {
        return;
    }

    m_isCompositingActive = it.value().toBool();
    Q_EMIT isCompositingActiveChanged();
}

void GrabWidget::initCompositorWatcher()
{
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &GrabWidget::queryCompositingActive);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, [this] {
        m_isCompositingActive = false;
        Q_EMIT isCompositingActiveChanged();
    });

    const bool connected = QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.KWin"),
                                                                 QStringLiteral("/Compositor"),
                                                                 QStringLiteral("org.freedesktop.DBus.Properties"),
                                                                 QStringLiteral("PropertiesChanged"),
                                                                 this,
                                                                 SLOT(slotPropertiesChanged(QString, QVariantMap, QStringList)));
    if (!connected) {
        return;
    }

    queryCompositingActive();
}

void GrabWidget::queryCompositingActive()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                      QStringLiteral("/Compositor"),
                                                      QStringLiteral("org.freedesktop.DBus.Properties"),
                                                      QStringLiteral("Get"));
    msg << QStringLiteral("org.kde.kwin.Compositing") << QStringLiteral("active");
    auto call = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QVariant> reply = *watcher;
        if (!reply.isError()) {
            m_isCompositingActive = reply.value().toBool();
            Q_EMIT isCompositingActiveChanged();
        }
    });
}