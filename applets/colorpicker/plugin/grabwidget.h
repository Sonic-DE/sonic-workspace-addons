/*
 * SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef GRABWIDGET_H
#define GRABWIDGET_H

#include <QColor>
#include <QObject>

class QDBusServiceWatcher;

class GrabWidget : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor currentColor READ currentColor NOTIFY currentColorChanged)

    /**
     * Is KWin compositing enabled?
     */
    Q_PROPERTY(bool isCompositingActive READ isCompositingActive NOTIFY isCompositingActiveChanged)

public:
    explicit GrabWidget(QObject *parent = nullptr);

    QColor currentColor() const;
    void setCurrentColor(const QColor &color);

    bool isCompositingActive() const;

    Q_INVOKABLE void pick();
    Q_INVOKABLE void copyToClipboard(const QString &text);

Q_SIGNALS:
    void currentColorChanged();
    void isCompositingActiveChanged();

private Q_SLOTS:
    void slotPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void queryCompositingActive();

private:
    void initCompositorWatcher();

    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    bool m_isCompositingActive = false;

    QColor m_currentColor;
};

#endif // GRABWIDGET_H
