#include "kded_potd.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(PotdModule, "kded_potd.json")

PotdModule::PotdModule(QObject* parent, const QList<QVariant>&): KDEDModule(parent)
{
    consumer = new Plasma::DataEngineConsumer();
    engine = consumer->dataEngine(QStringLiteral("potd"));
    configPath = QStandardPaths::locate(QStandardPaths::ConfigLocation,  QStringLiteral("kscreenlockerrc"));

    previousSource = getSource();
    engine->connectSource(previousSource, this); // trigger caching, no need to handle data

    connect(&watcher, &KDirWatch::dirty,
            this, &PotdModule::fileChanged);
    watcher.addFile(configPath);
}

PotdModule::~PotdModule()
{
    delete consumer;
}

void PotdModule::fileChanged(const QString &path)
{
    Q_UNUSED(path);
    engine->disconnectSource(previousSource, this);
    previousSource = getSource();
    engine->connectSource(previousSource, this);

    // For some reason, Qt *rc files are always recreated instead of modified.
    // Recreated files were removed from watchers and have to be added again.
    watcher.addFile(configPath);
}

QString PotdModule::getSource()
{
    KConfig config(configPath);

    KConfigGroup greeterGroup = config.group(QStringLiteral("Greeter"));
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
