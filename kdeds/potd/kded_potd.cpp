#include "kded_potd.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(PotdModule, "kded_potd.json")

PotdModule::PotdModule(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
    , config(KSharedConfig::openConfig(QStringLiteral("kscreenlockerrc"), KConfig::CascadeConfig))
    , configWatcher(KConfigWatcher::create(config))
{
    connect(configWatcher.data(), &KConfigWatcher::configChanged,
            this, [this] {
        engine->disconnectSource(previousSource, this);
        previousSource = getSource();
        engine->connectSource(previousSource, this);
    });

    consumer = new Plasma::DataEngineConsumer();
    engine = consumer->dataEngine(QStringLiteral("potd"));

    previousSource = getSource();
    engine->connectSource(previousSource, this); // trigger caching, no need to handle data
}

PotdModule::~PotdModule()
{
    delete consumer;
}

void PotdModule::fileChanged(const QString &path)
{
    Q_UNUSED(path);
}

QString PotdModule::getSource()
{
    KConfigGroup greeterGroup = config->group(QStringLiteral("Greeter"));
    QString plugin = greeterGroup.readEntry(QStringLiteral("WallpaperPlugin"), QString());
    if (plugin != QStringLiteral("org.kde.potd")) {
        return QStringLiteral("");
    }

    KConfigGroup potdGroup = greeterGroup
                               .group(QStringLiteral("Wallpaper"))
                               .group(QStringLiteral("org.kde.potd"))
                               .group(QStringLiteral("General"));
    QString provider = potdGroup.readEntry(QStringLiteral("Provider"), QString());
    if (provider == QStringLiteral("unsplash")) {
        QString category = potdGroup.readEntry(QStringLiteral("Category"), QString());
        return provider + QStringLiteral(":") + category;
    } else {
        return provider;
    }
}

#include "kded_potd.moc"
