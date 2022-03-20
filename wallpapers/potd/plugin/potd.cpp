/*
    SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "potd.h"

#include <chrono>

#include <QDate>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QTimer>

#include <KPluginFactory>
#include <KPluginMetaData>

#include "cachedprovider.h"

using namespace std::chrono_literals;

PotdProvidersModel::PotdProvidersModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadPluginMetaData();
}

PotdProvidersModel::~PotdProvidersModel() noexcept
{
}

int PotdProvidersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_providers.size();
}

QVariant PotdProvidersModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_providers.size())) {
        return QVariant();
    }

    const KPluginMetaData &item = m_providers.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return item.name();
    case Qt::DecorationRole:
        return item.iconName();
    case Roles::Id:
        return item.value(QLatin1String("X-KDE-PlasmaPoTDProvider-Identifier"));
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PotdProvidersModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {Qt::DecorationRole, "decoration"},
        {Roles::Id, "id"},
    };
}

void PotdProvidersModel::loadPluginMetaData()
{
    const QVector<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("potd"));

    beginResetModel();

    m_providers.clear();
    m_providers.reserve(10);

    std::copy_if(plugins.cbegin(), plugins.cend(), std::back_inserter(m_providers), [](const KPluginMetaData &metadata) {
        return !metadata.value(QLatin1String("X-KDE-PlasmaPoTDProvider-Identifier")).isEmpty();
    });

    endResetModel();
}

int PotdProvidersModel::indexOfProvider(const QString& identifier) const
{
    const auto it = std::find_if(m_providers.cbegin(), m_providers.cend(), [&identifier](const KPluginMetaData &metadata) {
        return metadata.value(QLatin1String("X-KDE-PlasmaPoTDProvider-Identifier")) == identifier;
    });

    if (it == m_providers.cend()) {
        return -1;
    }

    return std::distance(m_providers.cbegin(), it);
}

const KPluginMetaData &PotdProvidersModel::metadata(int index) const
{
    if (index >= static_cast<int>(m_providers.size()) || index < 0) {
        return m_providers.at(0);
    }

    return m_providers.at(index);
}

PotdEngine::PotdEngine(QObject *parent)
    : QObject(parent)
    , m_providersModel(new PotdProvidersModel(this))
    , m_identifier(QString())
    , m_currentIndex(-1)
    , m_args(QVariantList())
    , m_data(PotdProviderData())
    , m_checkDatesTimer(new QTimer(this)) // Change picture after 24 hours
{
    connect(m_checkDatesTimer, &QTimer::timeout, this, std::bind(&PotdEngine::forceUpdateSource, this));
    m_checkDatesTimer->setInterval(10min); // check every 10 minutes
}

PotdEngine::~PotdEngine()
{
}

bool PotdEngine::running() const
{
    return m_checkDatesTimer->isActive();
}

void PotdEngine::setRunning(bool flag)
{
    if (m_checkDatesTimer->isActive() == flag) {
        return;
    }

    if (flag) {
        m_checkDatesTimer->start();
    } else {
        m_checkDatesTimer->stop();
    }

    Q_EMIT runningChanged();
}

QString PotdEngine::identifier() const
{
    return m_identifier;
}

void PotdEngine::setIdentifier(const QString& identifier)
{
    if (m_identifier == identifier) {
        return;
    }

    m_identifier = identifier;
    m_currentIndex = m_providersModel->indexOfProvider(identifier);

    resetData();
    updateSource();

    Q_EMIT identifierChanged();
}

QVariantList PotdEngine::arguments() const
{
    return m_args;
}

void PotdEngine::setArguments(const QVariantList& args)
{
    if (m_args == args) {
        return;
    }

    m_args = args;

    resetData();
    forceUpdateSource();

    Q_EMIT argumentsChanged();
}

QImage PotdEngine::image() const
{
    return m_data.wallpaperImage;
}

void PotdEngine::setImage(const QImage& image)
{
    m_data.wallpaperImage = image;
    Q_EMIT imageChanged();
}

QString PotdEngine::localUrl() const
{
    return m_data.wallpaperLocalUrl;
}

void PotdEngine::setLocalUrl(const QString &urlString)
{
    if (m_data.wallpaperLocalUrl == urlString) {
        return;
    }

    m_data.wallpaperLocalUrl = urlString;
    Q_EMIT localUrlChanged();
}

void PotdEngine::resetData()
{
    setImage(QImage());
    setLocalUrl(QString());
}

bool PotdEngine::updateSource(bool refresh)
{
    // Check whether it is cached already...
    if (!refresh && CachedProvider::isCached(m_identifier, false)) {
        CachedProvider *provider = new CachedProvider(m_identifier, this);
        connect(provider, &PotdProvider::finished, this, &PotdEngine::slotFinished);
        connect(provider, &PotdProvider::error, this, &PotdEngine::slotError);
        return true;
    }

    if (m_currentIndex < 0) {
        qWarning() << "Invalid provider: " << m_identifier;
        return false;
    }

    const auto pluginResult = KPluginFactory::instantiatePlugin<PotdProvider>(m_providersModel->metadata(m_currentIndex), this, m_args);
    PotdProvider *provider = nullptr;

    if (pluginResult) {
        provider = pluginResult.plugin;
        connect(provider, &PotdProvider::finished, this, &PotdEngine::slotFinished);
        connect(provider, &PotdProvider::error, this, &PotdEngine::slotError);
        return true;
    }

    qWarning() << "Error loading PoTD plugin:" << pluginResult.errorString;
    return false;
}

bool PotdEngine::forceUpdateSource()
{
    return updateSource(true);
}

void PotdEngine::slotFinished(PotdProvider *provider)
{
    setImage(provider->image());

    // Store in cache if it's not the response of a CachedProvider
    if (qobject_cast<CachedProvider *>(provider) == nullptr && !m_data.wallpaperImage.isNull()) {
        SaveImageThread *thread = new SaveImageThread(m_identifier, m_data.wallpaperImage);
        connect(thread, &SaveImageThread::done, this, &PotdEngine::slotCachingFinished);
        QThreadPool::globalInstance()->start(thread);
    } else {
        setLocalUrl(CachedProvider::identifierToPath(m_identifier));
    }

    // Do not update until next day, and delay 1s to make sure last modified condition is satisfied.
    if (m_checkDatesTimer->isActive()) {
        m_checkDatesTimer->setInterval(QDateTime::currentDateTime().msecsTo(QDateTime(QDate::currentDate()).addDays(1)) + 1000);
        m_checkDatesTimer->start();
    }

    provider->deleteLater();
}

void PotdEngine::slotCachingFinished(const QString &source, const QString &path, const QImage &img)
{
    Q_UNUSED(source)
    setImage(img);
    setLocalUrl(path);
}

void PotdEngine::slotError(PotdProvider *provider)
{
    provider->disconnect(this);
    provider->deleteLater();

    // Retry 10min later
    if (m_checkDatesTimer->isActive()) {
        m_checkDatesTimer->setInterval(10min);
        m_checkDatesTimer->start();
    }
}
