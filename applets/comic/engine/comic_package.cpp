/*
 *   SPDX-FileCopyrightText: 2008 Petri Damstén <damu@iki.fi>
 *   SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de<
 *
 *   SPDX-License-Identifier: LGPL-2.0-only
 */

#include <KConfigGroup>
#include <KDesktopFile>
#include <KPackage/Package>
#include <KPackage/PackageStructure>

class ComicPackage : public KPackage::PackageStructure
{
    Q_OBJECT
public:
    using KPackage::PackageStructure::PackageStructure;
    void initPackage(KPackage::Package *package) override
    {
        package->addDirectoryDefinition("images", QStringLiteral("images"));
        package->setMimeTypes("images", QStringList{QStringLiteral("image/svg+xml"), QStringLiteral("image/png"), QStringLiteral("image/jpeg")});

        package->addDirectoryDefinition("scripts", QStringLiteral("code"));
        package->setMimeTypes("scripts", QStringList{QStringLiteral("text/*")});

        package->addFileDefinition("mainscript", QStringLiteral("code/main"));
        package->addFileDefinition("metadata", QStringLiteral("metadata.desktop"));
        // package->setRequired("mainscript", true); Package::isValid() fails with this because of Kross and different file extensions
        package->setDefaultPackageRoot(QStringLiteral("plasma/comics/"));
    }
    void pathChanged(KPackage::Package *package) override
    {
        // qWarning() << Q_FUNC_INFO << package->isValid() << package->filePath("metadata", "metadata.desktop");
        if (const QString legacyPath = package->filePath("metadata"); !legacyPath.isEmpty() && legacyPath.endsWith(".desktop")) {
            KDesktopFile file(legacyPath);
            const KConfigGroup grp = file.desktopGroup();
            QJsonObject kplugin{{"Name", grp.readEntry("X-KDE-PluginInfo-Name")}, {"Id", grp.readEntry("X-KDE-PluginInfo-Name")}};
            QJsonObject obj{
                {"KPlugin", kplugin},
                {"X-KDE-PlasmaComicProvider-SuffixType", grp.readEntry("X-KDE-PlasmaComicProvider-SuffixType")},
                {"KPackageStructure", "Plasma/Comic"},
            };
            package->setMetadata(KPluginMetaData(obj, legacyPath));
        }
    }
};

K_PLUGIN_CLASS_WITH_JSON(ComicPackage, "plasma-packagestructure-comic.json")

#include "comic_package.moc"
