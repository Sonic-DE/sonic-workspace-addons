// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2021 Guo Yunhe <i@guoyunhe.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "potdprovider.h"

#include <QDate>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRunnable>
#include <QStandardPaths>
#include <QThreadPool>

namespace PotdProviderUtils
{
QString identifierToPath(const QString &identifier, const QVariantList &args)
{
    const QString argString = std::accumulate(args.cbegin(), args.cend(), QString(), [](const QString &s, const QVariant &arg) {
        if (arg.canConvert(QMetaType::QString)) {
            return s + QStringLiteral(":%1").arg(arg.toString());
        }

        return s;
    });

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/plasma_engine_potd/");
    QDir d;
    d.mkpath(dataDir);
    return QStringLiteral("%1%2%3").arg(dataDir, identifier, argString);
}

}

class SaveImageThread : public QObject, public QRunnable
{
    Q_OBJECT

public:
    SaveImageThread(const QString &identifier, const QVariantList &args, const QVariantMap &data);
    void run() override;

Q_SIGNALS:
    void done(const QString &localPath);

private:
    QString m_identifier;
    QVariantList m_args;
    QUrl m_remoteUrl;
    QUrl m_infoUrl;
    QString m_title;
    QString m_author;
    QImage m_image;
};

class PotdProviderPrivate
{
public:
    explicit PotdProviderPrivate();
    ~PotdProviderPrivate();

    QString name;
    QDate date;
    QString identifier;
    QString localPath;
};

SaveImageThread::SaveImageThread(const QString &identifier, const QVariantList &args, const QVariantMap &data)
    : m_identifier(identifier)
    , m_args(args)
    , m_remoteUrl(data[QStringLiteral("RemoteUrl")].toUrl())
    , m_infoUrl(data[QStringLiteral("InfoUrl")].toUrl())
    , m_title(data[QStringLiteral("Title")].toString())
    , m_author(data[QStringLiteral("Author")].toString())
    , m_image(data[QStringLiteral("Image")].value<QImage>())
{
}

void SaveImageThread::run()
{
    const QString localPath = PotdProviderUtils::identifierToPath(m_identifier, m_args);
    m_image.save(localPath, "JPEG");

    const QString infoPath = localPath + ".json";
    QFile infoFile(infoPath);
    if (infoFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;

        jsonObject.insert(QStringLiteral("InfoUrl"), m_infoUrl.url());
        jsonObject.insert(QStringLiteral("RemoteUrl"), m_remoteUrl.url());
        jsonObject.insert(QStringLiteral("Title"), m_title);
        jsonObject.insert(QStringLiteral("Author"), m_author);

        infoFile.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        infoFile.close();
    } else {
        qWarning() << "Failed to save the wallpaper information!";
    }

    Q_EMIT done(localPath);
}

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
    Q_D(const PotdProvider);
    return d->localPath;
}

QUrl PotdProvider::remoteUrl() const
{
    return QUrl();
}

QUrl PotdProvider::infoUrl() const
{
    return QUrl();
}

QString PotdProvider::title() const
{
    return QString();
}

QString PotdProvider::author() const
{
    return QString();
}

QDate PotdProvider::date() const
{
    Q_D(const PotdProvider);
    return d->date.isNull() ? QDate::currentDate() : d->date;
}

void PotdProvider::save(const QImage &image, const QVariantList &args)
{
    QVariantMap data;
    data.insert(QStringLiteral("RemoteUrl"), remoteUrl());
    data.insert(QStringLiteral("InfoUrl"), infoUrl());
    data.insert(QStringLiteral("Title"), title());
    data.insert(QStringLiteral("Author"), author());
    data.insert(QStringLiteral("RemoteUrl"), remoteUrl());
    data.insert(QStringLiteral("Image"), image);

    SaveImageThread *thread = new SaveImageThread(identifier(), args, data);
    connect(thread, &SaveImageThread::done, this, [this](const QString &localPath) {
        Q_D(PotdProvider);
        d->localPath = localPath;
        cached(this, localPath);
    });
    QThreadPool::globalInstance()->start(thread);
}

#include "potdprovider.moc"