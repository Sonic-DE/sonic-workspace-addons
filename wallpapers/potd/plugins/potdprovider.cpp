// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2021 Guo Yunhe <i@guoyunhe.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "potdprovider.h"

#include <QDate>
#include <QThreadPool>

class PotdProviderPrivate
{
public:
    explicit PotdProviderPrivate();
    ~PotdProviderPrivate();

    QString name;
    QDate date;
    QString identifier;
};

PotdProviderPrivate::PotdProviderPrivate()
{
}

PotdProviderPrivate::~PotdProviderPrivate()
{
}

PotdProvider::PotdProvider(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : QObject(parent)
    , d_ptr(new PotdProviderPrivate)
{
    if (const QString name = data.name(); !name.isEmpty()) {
        d_ptr->name = name;
    } else {
        d_ptr->name = QStringLiteral("Unknown");
    }

    if (const QString identifier = data.value(QStringLiteral("X-KDE-PlasmaPoTDProvider-Identifier")); !identifier.isEmpty()) {
        d_ptr->identifier = identifier;
    } else {
        d_ptr->identifier = d_ptr->name;
    }

    if (!args.empty()) {
        for (const auto &arg : args) {
            const QDate date = QDate::fromString(arg.toString(), Qt::ISODate);
            if (date.isValid()) {
                d_ptr->date = date;
                break;
            }
        }
    }
}

PotdProvider::~PotdProvider()
{
}

QString PotdProvider::name() const
{
    Q_D(const PotdProvider);
    return d->name;
}

QString PotdProvider::identifier() const
{
    Q_D(const PotdProvider);
    return d->identifier;
}

QString PotdProvider::localPath() const
{
    return QString();
}

QUrl PotdProvider::remoteUrl() const
{
    return m_remoteUrl;
}

QUrl PotdProvider::infoUrl() const
{
    return m_infoUrl;
}

QString PotdProvider::title() const
{
    return m_title;
}

QString PotdProvider::author() const
{
    return m_author;
}

QDate PotdProvider::date() const
{
    Q_D(const PotdProvider);
    return d->date.isNull() ? QDate::currentDate() : d->date;
}
